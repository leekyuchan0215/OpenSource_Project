/*
 * cmd_file.c
 *
 * [역할]
 * 파일 시스템 관련 명령어 구현 (cp, rm, mv, ln)
 *
 * [작성자]
 * 임진호
 */

#include "myshell.h"

/*
 * cmd_cp
 * [기능] 파일 복사 명령어 (Usage: cp <src> <dest>)
 * [설명] open(), read(), write() 시스템 호출을 사용하여 파일을 복사합니다.
 * 대상 파일이 없을 경우 생성하고, 있을 경우 내용을 덮어씁니다.
 * 대용량 파일도 처리할 수 있도록 4KB 버퍼를 사용합니다.
 */
int cmd_cp(int argc, char *argv[]) {
    //TODO : cp 구현 
}

/*
 * cmd_rm
 * [기능] 파일 삭제 명령어 (Usage: rm <file>...)
 * [설명] unlink() 시스템 호출을 사용하여 파일 시스템에서 링크를 해제합니다.
 * 여러 개의 파일을 한 번에 삭제할 수 있습니다.
 */
int cmd_rm(int argc, char *argv[]) {
    //TODO : rm 구현
}

/*
 * cmd_mv
 * [기능] 파일 이동 및 이름 변경 명령어 (Usage: mv <src> <dest>)
 * [설명] rename() 시스템 호출을 사용하여 파일의 경로를 변경합니다.
 */
int cmd_mv(int argc, char *argv[]) {
    //TODO : mv 구현
}

/*
 * cmd_ln
 * [기능] 하드 링크 생성 명령어 (Usage: ln <target> <link_name>)
 * [설명] link() 시스템 호출을 사용하여 기존 파일에 대한 새로운 하드 링크를 생성합니다.
 */
int cmd_ln(int argc, char *argv[]) {
    //TODO : ln 구현
}