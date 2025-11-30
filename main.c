/*
 * main.c
 *
 * [역할]
 * 메인 루프, 시그널, 파싱
 *
 * [작성자]
 * 20212977 이규찬
 */
#include "myshell.h"

// 전역 변수 정의
Job jobs[MAX_HISTORY];
int job_count = 0;

// 시그널 핸들러: Ctrl-C (SIGINT)
void handle_sigint(int signo) {
    printf("\n[Shell] Ctrl-C (SIGINT) detected. Use 'exit' to quit.\n");
    print_prompt();
    fflush(stdout);
}

// 시그널 핸들러: Ctrl-Z (SIGQUIT)
void handle_sigquit(int signo) {
    printf("\n[Shell] Ctrl-Z (SIGQUIT) detected. Process ignored.\n");
    print_prompt();
    fflush(stdout);
}

// 시그널 핸들러: 자식 프로세스 종료 처리 (좀비 방지)
void handle_sigchld(int signo) {
    int status;
    pid_t pid;
    
    // 종료된 자식 프로세스를 비동기적으로 회수
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < job_count; i++) {
            if (jobs[i].pid == pid && jobs[i].is_running) {
                jobs[i].is_running = 0;
                // printf("\n[Job %d] Done: %s\n", jobs[i].id, jobs[i].command); // 프롬프트 깨짐 방지 위해 주석
                break;
            }
        }
    }
}

// 백그라운드 작업 추가 함수
void add_job(pid_t pid, char *command) {
    if (job_count < MAX_HISTORY) {
        jobs[job_count].id = job_count + 1;
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].command, command, MAX_CMD_BUF);
        jobs[job_count].is_running = 1;
        printf("[%d] %d\n", jobs[job_count].id, pid);
        job_count++;
    } else {
        fprintf(stderr, "Error: Job history full.\n");
    }
}

// jobs 명령어 구현
void print_jobs() {
    printf("JOB ID\tPID\tSTATUS\tCOMMAND\n");
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].is_running) {
            printf("[%d]\t%d\tRunning\t%s\n", jobs[i].id, jobs[i].pid, jobs[i].command);
        } else {
            printf("[%d]\t%d\tDone\t%s\n", jobs[i].id, jobs[i].pid, jobs[i].command);
        }
    }
}

// 프롬프트 출력
void print_prompt() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        // 색상 코드를 사용하여 쉘처럼 꾸밈
        printf("\033[1;32mmyshell\033[0m:\033[1;34m%s\033[0m$ ", cwd);
    } else {
        perror("getcwd error");
    }
}

// 입력 파싱 함수: 공백을 기준으로 문자열 분리
int parse_input(char *cmd, char **argv) {
    int count = 0;
    char *token = strtok(cmd, " \t\n");
    
    while (token != NULL && count < MAX_ARG - 1) {
        argv[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    argv[count] = NULL;
    return count;
}

int main() {
    char cmd_buf[MAX_CMD_BUF];
    struct sigaction sa_int, sa_quit, sa_chld;

    // 1. 시그널 핸들러 등록
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_quit.sa_handler = handle_sigquit;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);

    sa_chld.sa_handler = handle_sigchld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART; // 시스템 콜 자동 재시작
    sigaction(SIGCHLD, &sa_chld, NULL);

    printf("=========================================\n");
    printf(" Welcome to MyShell (Team Project)\n");
    printf(" Type 'exit' to quit.\n");
    printf("=========================================\n");

    // 2. 메인 루프
    while (1) {
        print_prompt();

        if (fgets(cmd_buf, MAX_CMD_BUF, stdin) == NULL) {
            if (ferror(stdin)) {
                clearerr(stdin); // 에러 상태를 초기화하고
                printf("\n");
                continue;        // 루프의 처음으로 돌아가서 다시 입력받음
            }
            
            // 진짜 종료 신호(Ctrl-D)가 들어온 경우
            printf("\nExiting shell...\n");
            break;
        }

        // 개행 문자 제거
        cmd_buf[strcspn(cmd_buf, "\n")] = '\0';

        if (strlen(cmd_buf) == 0) continue;

        // exit 명령 처리
        if (strcmp(cmd_buf, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }
        
        // jobs 명령 처리
        if (strcmp(cmd_buf, "jobs") == 0) {
            print_jobs();
            continue;
        }

        // 3. 실행 로직 위임 
        execute_command_line(cmd_buf);
    }

    return 0;
}