/* myshell.h - 헤더 파일 */
#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define MAX_CMD_BUF 1024
#define MAX_ARG 64
#define MAX_HISTORY 100

// 백그라운드 작업을 위한 구조체
typedef struct {
    int id;
    pid_t pid;
    char command[MAX_CMD_BUF];
    int is_running;
} Job;

// 전역 변수 선언 (main.c에 정의됨)
extern Job jobs[MAX_HISTORY];
extern int job_count;

// --- 함수 프로토타입 선언 ---

// main.c
void add_job(pid_t pid, char *command);
void print_prompt();
int parse_input(char *cmd, char **argv);
void handle_signal(int signo);

// execute.c
void execute_command_line(char *input);
int handle_redirection(char **argv);
void execute_pipe(char **argv1, char **argv2);

// commands.c
int is_builtin(char *cmd);
void do_builtin(char **argv);
void cmd_ls(char **argv);
void cmd_cd(char **argv);
void cmd_mkdir(char **argv);
void cmd_rmdir(char **argv);
void cmd_cp(char **argv);
void cmd_mv(char **argv);
void cmd_rm(char **argv);
void cmd_cat(char **argv);
void cmd_pwd(char **argv);
void cmd_ln(char **argv);
void cmd_grep(char **argv);

#endif