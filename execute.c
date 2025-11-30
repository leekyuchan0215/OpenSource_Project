/*
 *
 * [역할]
 *  한 줄의 명령어 입력을 파싱하여 빌트인/외부 명령을 실행하는 모듈
 *  리다이렉션 처리
 *  파이프 처리
 *  백그라운드 실행 처리 및 job 등록
 *
 * [작성자]
 *  20233065 이지민
 */
#include "myshell.h"

// 리다이렉션 처리 함수 (>, <)
// 반환값: 리다이렉션 기호의 인덱스, 없으면 -1
int handle_redirection(char **argv) {
    int fd;
    int i = 0;
    
    while (argv[i] != NULL) {
        if (strcmp(argv[i], ">") == 0) {
            // 출력 리다이렉션
            if (argv[i+1] == NULL) {
                fprintf(stderr, "Syntax error: Expected file after >\n");
                return -1;
            }
            // 파일 오픈 - 생성, 쓰기전용, 잘라내기
            fd = open(argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open failed");
                return -1;
            }
            // 표준 출력을 파일로 변경
            dup2(fd, STDOUT_FILENO);
            close(fd);
            
            // 리다이렉션 기호와 파일명을 argv에서 제거
            argv[i] = NULL;
            return 1;
        } 
        else if (strcmp(argv[i], "<") == 0) {
            // 입력 리다이렉션
            if (argv[i+1] == NULL) {
                fprintf(stderr, "Syntax error: Expected file after <\n");
                return -1;
            }
            fd = open(argv[i+1], O_RDONLY);
            if (fd == -1) {
                perror("open failed");
                return -1;
            }
            // 표준 입력을 파일로 변경
            dup2(fd, STDIN_FILENO);
            close(fd);
            
            argv[i] = NULL;
            return 1;
        }
        i++;
    }
    return 0;
}

// 파이프 처리 함수
void execute_pipe(char **argv1, char **argv2) {
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return;
    }

    // 첫 번째 명령 실행 - 파이프 쓰기
    p1 = fork();
    if (p1 == 0) {
        close(pipefd[0]); // 읽기 닫기
        dup2(pipefd[1], STDOUT_FILENO); // 표준 출력을 파이프로
        close(pipefd[1]);
        
        // 내부/외부 명령어 실행
        if (is_builtin(argv1[0])) {
            do_builtin(argv1);
            exit(0);
        } else {
            execvp(argv1[0], argv1);
            perror("execvp failed (cmd1)");
            exit(1);
        }
    }

    // 두 번째 명령 실행 - 파이프 읽기
    p2 = fork();
    if (p2 == 0) {
        close(pipefd[1]); // 쓰기 닫기
        dup2(pipefd[0], STDIN_FILENO); // 표준 입력을 파이프로
        close(pipefd[0]);

        if (is_builtin(argv2[0])) {
            do_builtin(argv2);
            exit(0);
        } else {
            execvp(argv2[0], argv2);
            perror("execvp failed (cmd2)");
            exit(1);
        }
    }

    // 부모 프로세스: 파이프 닫고 자식 대기
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}

// 명령줄 전체 실행 로직
void execute_command_line(char *input) {
    char *argv[MAX_ARG];
    char *argv_pipe[MAX_ARG];
    int is_bg = 0;
    
    // 1. 파이프 확인 및 분리
    char *pipe_pos = strchr(input, '|');
    if (pipe_pos != NULL) {
        *pipe_pos = '\0';
        char *cmd2 = pipe_pos + 1;
        
        parse_input(input, argv);
        parse_input(cmd2, argv_pipe);
        
        execute_pipe(argv, argv_pipe);
        return;
    }

    // 2. 일반 파싱
    int argc = parse_input(input, argv);
    if (argc == 0) return;

    // 3. 백그라운드(&) 확인
    if (strcmp(argv[argc-1], "&") == 0) {
        is_bg = 1;
        argv[argc-1] = NULL;
    }

    // 4. 빌트인 명령어 확인 및 실행
    if (is_builtin(argv[0])) {
        // [수정된 부분] 빌트인 명령어도 리다이렉션 처리 (화면 복구 기능 추가)
        int original_stdin = dup(STDIN_FILENO);
        int original_stdout = dup(STDOUT_FILENO);

        // 리다이렉션 처리 (argv에서 >, < 기호 제거됨)
        if (handle_redirection(argv) == -1) {
            // 에러 나면 복구하고 종료
            dup2(original_stdin, STDIN_FILENO);
            dup2(original_stdout, STDOUT_FILENO);
            return;
        }

        // 명령어 실행
        do_builtin(argv);

        // [중요] 표준 입출력을 원래대로(화면/키보드) 복구
        dup2(original_stdin, STDIN_FILENO);
        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdin);
        close(original_stdout);
        return;
    }

    // 5. 외부 명령어 실행 (fork & exec)
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) { // 자식 프로세스
        if (!is_bg) {
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
        } else {
            signal(SIGINT, SIG_IGN);
            signal(SIGQUIT, SIG_IGN);
        }

        // 리다이렉션 처리
        handle_redirection(argv);

        execvp(argv[0], argv);
        
        fprintf(stderr, "%s: command not found\n", argv[0]);
        exit(1);
    } else { // 부모 프로세스
        if (is_bg) {
            add_job(pid, input);
        } else {
            waitpid(pid, NULL, 0);
        }
    }
}