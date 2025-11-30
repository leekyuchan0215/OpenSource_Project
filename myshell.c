/*
 * myshell.c
 *
 * [역할]
 * 쉘의 메인 루프, 명령어 파싱, 프로세스 생성 및 제어, 시그널 처리
 * 파이프라인 및 입출력 리다이렉션 처리를 담당합니다.
 *
 * [작성자]
 * 이규찬 (팀장)
 */

#include "myshell.h"

// ============================================================================
// 시그널 핸들러 함수
// ============================================================================

/*
 * handle_sigint
 * [기능] SIGINT (Ctrl-C) 시그널을 처리합니다.
 * [동작] 쉘이 종료되지 않고 줄을 바꾼 뒤 프롬프트를 다시 출력합니다.
 */
void handle_sigint(int signo) {
    printf("\n[SIGINT] 인터럽트 발생. 종료하려면 'exit'를 입력하세요.\n");
    printf("$ "); // 프롬프트 재출력
    fflush(stdout);
}

/*
 * handle_sigquit
 * [기능] SIGQUIT (Ctrl-Z) 시그널을 처리합니다.
 * [동작] 현재 실행 중인 포그라운드 작업을 중단하거나 무시합니다.
 */
void handle_sigquit(int signo) {
    printf("\n[SIGQUIT] 종료 시그널 수신.\n");
    printf("$ ");
    fflush(stdout);
}

/*
 * handle_sigchld
 * [기능] 자식 프로세스가 종료되었을 때 발생하는 SIGCHLD 시그널을 처리합니다.
 * [동작] waitpid를 WNOHANG 옵션으로 호출하여 좀비 프로세스를 제거합니다.
 */
void handle_sigchld(int signo) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// ============================================================================
// 헬퍼 함수 (파싱 및 유틸리티)
// ============================================================================

/*
 * tokenize
 * [기능] 입력된 명령어 문자열을 공백(스페이스, 탭, 개행) 기준으로 분리합니다.
 * [인자] cmd: 원본 명령어 문자열, argv: 분리된 토큰을 저장할 배열
 * [반환] 분리된 인자의 개수 (argc)
 */
int tokenize(char *cmd, char *argv[]) {
    int count = 0;
    char *token = strtok(cmd, " \t\n");
    
    while (token != NULL && count < MAX_ARGS - 1) {
        argv[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    argv[count] = NULL; // 인자 목록의 끝을 NULL로 표시
    return count;
}

/*
 * handle_redirection
 * [기능] 명령어 인자에서 '>', '<' 기호를 찾아 입출력 방향을 재설정합니다.
 * [설명] open(), dup2(), close() 시스템 호출을 사용하여 표준 입출력을 파일로 연결합니다.
 */
void handle_redirection(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], ">") == 0) {
            // [출력 재지향] stdout -> file
            if (i + 1 < argc) {
                // 파일 열기: 쓰기 전용, 생성, 내용 삭제 (권한 0644)
                int fd = open(argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("myshell: redirection open error");
                    exit(EXIT_FAILURE);
                }
                // 표준 출력을 파일 디스크립터로 복제
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("myshell: dup2 error");
                    exit(EXIT_FAILURE);
                }
                close(fd);
                argv[i] = NULL; // 리다이렉션 기호 이후는 명령어가 아님
            }
        } else if (strcmp(argv[i], "<") == 0) {
            // [입력 재지향] stdin <- file
            if (i + 1 < argc) {
                int fd = open(argv[i+1], O_RDONLY);
                if (fd < 0) {
                    perror("myshell: redirection open error");
                    exit(EXIT_FAILURE);
                }
                // 표준 입력을 파일 디스크립터로 복제
                if (dup2(fd, STDIN_FILENO) < 0) {
                    perror("myshell: dup2 error");
                    exit(EXIT_FAILURE);
                }
                close(fd);
                argv[i] = NULL;
            }
        }
    }
}

/*
 * execute_command
 * [기능] 파싱된 명령어를 실행합니다.
 * [동작] 
 * 1. 백그라운드 실행 여부 확인 (&)
 * 2. 파이프라인 처리 (|)
 * 3. 내장 명령어 처리 (exit, cd)
 * 4. 외부 명령어 실행 (fork -> execvp)
 */
void execute_command(char *cmd_line) {
    // 1. 백그라운드 실행 여부 확인
    int background = 0;
    size_t len = strlen(cmd_line);
    if (len > 0 && cmd_line[len - 1] == '&') {
        background = 1;
        cmd_line[len - 1] = '\0'; // & 기호 제거
    }

    // 2. 파이프라인 처리 (| 기호 확인)
    char *pipe_segments[2]; // 최대 2개의 명령어 파이프라인 지원
    int pipe_count = 0;
    char *ptr = strtok(cmd_line, "|");
    
    while (ptr != NULL && pipe_count < 2) {
        pipe_segments[pipe_count++] = ptr;
        ptr = strtok(NULL, "|");
    }

    // 파이프가 존재하는 경우 (명령어1 | 명령어2)
    if (pipe_count == 2) {
        int p[2];
        // pipe() 시스템 호출: 통신 채널 생성
        if (pipe(p) == -1) {
            perror("myshell: pipe failed");
            return;
        }

        pid_t pid1 = fork();
        if (pid1 == 0) {
            // [Writer Process] 첫 번째 명령어
            dup2(p[1], STDOUT_FILENO); // 표준 출력을 파이프의 쓰기 포트로 변경
            close(p[0]);
            close(p[1]);
            execute_command(pipe_segments[0]); // 재귀 호출로 실행
            exit(EXIT_SUCCESS);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            // [Reader Process] 두 번째 명령어
            dup2(p[0], STDIN_FILENO); // 표준 입력을 파이프의 읽기 포트로 변경
            close(p[0]);
            close(p[1]);
            execute_command(pipe_segments[1]); // 재귀 호출로 실행
            exit(EXIT_SUCCESS);
        }

        // 부모 프로세스는 파이프를 사용하지 않으므로 닫음
        close(p[0]);
        close(p[1]);
        
        // 백그라운드가 아니면 두 자식 프로세스가 끝날 때까지 대기
        if (!background) {
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
        return;
    }

    // 3. 일반 명령어 실행 (파이프 없음)
    char *argv[MAX_ARGS];
    int argc = tokenize(cmd_line, argv);
    
    if (argc == 0) return; // 빈 명령어

    // [내장 명령어] 부모 프로세스에서 실행해야 하는 명령어들
    if (strcmp(argv[0], "exit") == 0) {
        printf("쉘을 종료합니다... 안녕히 가세요!\n");
        exit(0);
    }
    if (strcmp(argv[0], "cd") == 0) {
        cmd_cd(argc, argv);
        return;
    }

    // [외부 명령어 및 사용자 구현 명령어] 자식 프로세스 생성
    pid_t pid = fork();
    if (pid < 0) {
        perror("myshell: fork failed");
        return;
    }

    if (pid == 0) {
        // --- Child Process ---
        handle_redirection(argc, argv); // 리다이렉션 설정

        // 4. 사용자 구현 명령어 매핑 (팀원들이 구현한 함수 호출)
        if (strcmp(argv[0], "ls") == 0) exit(cmd_ls(argc, argv));
        if (strcmp(argv[0], "pwd") == 0) exit(cmd_pwd(argc, argv));
        if (strcmp(argv[0], "mkdir") == 0) exit(cmd_mkdir(argc, argv));
        if (strcmp(argv[0], "rmdir") == 0) exit(cmd_rmdir(argc, argv));
        if (strcmp(argv[0], "ln") == 0) exit(cmd_ln(argc, argv));
        if (strcmp(argv[0], "cp") == 0) exit(cmd_cp(argc, argv));
        if (strcmp(argv[0], "rm") == 0) exit(cmd_rm(argc, argv));
        if (strcmp(argv[0], "mv") == 0) exit(cmd_mv(argc, argv));
        if (strcmp(argv[0], "cat") == 0) exit(cmd_cat(argc, argv));
        if (strcmp(argv[0], "grep") == 0) exit(cmd_grep(argc, argv));

        // 5. 그 외 시스템 명령어 (execvp 사용)
        // 예: vi, ps, date 등 시스템에 설치된 기본 명령어 실행
        if (execvp(argv[0], argv) == -1) {
            fprintf(stderr, "myshell: command not found: %s\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        // --- Parent Process ---
        if (!background) {
            // 포그라운드 실행: 자식이 끝날 때까지 대기
            int status;
            waitpid(pid, &status, 0);
        } else {
            // 백그라운드 실행: pid만 출력하고 즉시 프롬프트 반환
            printf("[Background PID: %d]\n", pid);
        }
    }
}

// ============================================================================
// 메인 함수
// ============================================================================
int main() {
    char cmd_line[MAX_CMD_LEN];
    char cwd[MAX_PATH_LEN];

    // 시그널 핸들러 등록 (sigaction 구조체 사용)
    struct sigaction sa_int, sa_quit, sa_chld;
    
    // SIGINT (Ctrl-C) 설정
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);

    // SIGQUIT (Ctrl-Z) 설정
    sa_quit.sa_handler = handle_sigquit;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = SA_RESTART;
    sigaction(SIGQUIT, &sa_quit, NULL);

    // SIGCHLD (자식 프로세스 종료) 설정
    sa_chld.sa_handler = handle_sigchld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa_chld, NULL);

    // 쉘 루프 시작
    while (1) {
        // 현재 디렉토리 경로 가져오기
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            strcpy(cwd, "unknown");
        }
        
        // 프롬프트 출력
        printf("%s $ ", cwd);
        
        // 사용자 명령어 입력
        if (fgets(cmd_line, MAX_CMD_LEN, stdin) == NULL) {
            printf("\n"); // Ctrl-D (EOF) 처리
            break;
        }

        // 입력된 명령어의 개행 문자 제거
        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        // 명령어가 입력된 경우에만 실행
        if (strlen(cmd_line) > 0) {
            execute_command(cmd_line);
        }
    }

    return 0;
}