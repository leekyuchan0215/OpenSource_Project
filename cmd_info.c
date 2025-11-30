/*
 * cmd_info.c
 *
 * [역할]
 * 디렉토리 조회 및 정보 출력 명령어 구현 (ls, pwd, cd, mkdir, cat, grep)
 *
 * [작성자]
 * 이지민
 */

#include "myshell.h"

/*
 * mode_to_string
 * [기능] 파일의 st_mode 값을 'drwxr-xr-x' 형태의 문자열로 변환합니다.
 * [인자] mode: 파일 모드, str: 결과를 저장할 문자열 버퍼
 */
void mode_to_string(mode_t mode, char str[]) {
    //TODO : 문자열 변환 구현
}

/*
 * cmd_ls
 * [기능] 디렉토리 목록 출력 명령어 (Usage: ls [-a] [-l] [dir])
 * [옵션]
 * -a : 숨김 파일(.으로 시작) 포함 모든 파일 출력
 * -l : 상세 정보 출력 (권한, 링크수, 소유자, 그룹, 크기, 시간 등)
 */
int cmd_ls(int argc, char *argv[]) {
    //TODO : ls 구현
}

/*
 * cmd_pwd
 * [기능] 현재 작업 디렉토리의 절대 경로를 출력합니다.
 * [설명] getcwd() 시스템 호출을 사용합니다.
 */
int cmd_pwd(int argc, char *argv[]) {
    //TODO : pwd 구현
}

/*
 * cmd_cd
 * [기능] 현재 작업 디렉토리를 변경합니다.
 * [설명] chdir() 시스템 호출을 사용하며, 인자가 없으면 에러를 출력합니다.
 * (실제 쉘에서는 HOME으로 이동하지만 여기서는 단순화)
 */
int cmd_cd(int argc, char *argv[]) {
    //TODO : cd 구현
}

/*
 * cmd_mkdir
 * [기능] 새로운 디렉토리를 생성합니다.
 * [설명] mkdir() 호출 시 0755 권한(rwxr-xr-x)을 기본으로 사용합니다.
 */
int cmd_mkdir(int argc, char *argv[]) {
    //TODO : mkdir 구현
}

/*
 * cmd_rmdir
 * [기능] 비어있는 디렉토리를 삭제합니다.
 */
int cmd_rmdir(int argc, char *argv[]) {
    //TODO : rmdir 구현
}

/*
 * cmd_cat
 * [기능] 파일의 내용을 표준 출력으로 보여줍니다.
 * [설명] 파일을 읽어서 STDOUT_FILENO로 write합니다.
 */
int cmd_cat(int argc, char *argv[]) {
    //TODO : cat 구현
}

/*
 * cmd_grep
 * [기능] 파일에서 특정 문자열 패턴을 검색합니다.
 * [설명] fopen/fgets를 사용하여 줄 단위로 읽은 후 strstr로 검색합니다.
 */
int cmd_grep(int argc, char *argv[]) {
    //TODO : grep 구현
}