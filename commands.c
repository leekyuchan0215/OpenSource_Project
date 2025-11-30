// 진호 파트
#include "myshell.h"

int is_builtin(char *cmd) {
    // 테스트를 위해 일단 0 반환 (외부 명령어처럼 처리됨)
    return 0; 
}

void do_builtin(char **argv) {
    printf("[Debug] do_builtin called (Not implemented yet)\n");
}

// 빈 함수들 정의 (링킹 에러 방지용)
void cmd_ls(char **argv) {}
void cmd_cd(char **argv) {}
void cmd_mkdir(char **argv) {}
void cmd_rmdir(char **argv) {}
void cmd_cp(char **argv) {}
void cmd_mv(char **argv) {}
void cmd_rm(char **argv) {}
void cmd_cat(char **argv) {}
void cmd_pwd(char **argv) {}
void cmd_ln(char **argv) {}
void cmd_grep(char **argv) {}