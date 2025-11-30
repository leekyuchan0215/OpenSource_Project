/*
 * myshell.h
 * 시스템 프로그래밍 프로젝트: MYSHELL
 *
 * [프로젝트 정보]
 * - 팀명: 4조
 * - 팀장: 이규찬 (Main Shell Logic, Process Control)
 * - 팀원: 임진호 (File Management Implementation)
 * - 팀원: 이지민 (Information & Directory Implementation)
 *
 * [개요]
 * 이 헤더 파일은 쉘 프로그램 전체에서 사용되는 표준 라이브러리,
 * 상수 정의, 전역 변수 및 함수 프로토타입을 포함합니다.
 */

#ifndef MYSHELL_H
#define MYSHELL_H

// ==========================================
// 시스템 헤더 파일 포함
// ==========================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

// ==========================================
// 상수 및 매크로 정의
// ==========================================
#define MAX_CMD_LEN 2048    // 명령어 최대 길이
#define MAX_ARGS 128        // 최대 인자 개수
#define MAX_PATH_LEN 1024   // 최대 경로 길이

// ==========================================
// 함수 프로토타입 선언
// ==========================================

// ------------------------------------------
// [cmd_file.c] 담당: 임진호
// 파일 복사, 삭제, 이동, 링크 등 파일 조작 명령어
// ------------------------------------------
int cmd_cp(int argc, char *argv[]);   // 파일 복사
int cmd_rm(int argc, char *argv[]);   // 파일 삭제
int cmd_mv(int argc, char *argv[]);   // 파일 이동 및 이름 변경
int cmd_ln(int argc, char *argv[]);   // 하드 링크 생성

// ------------------------------------------
// [cmd_info.c] 담당: 이지민
// 디렉토리 조회, 정보 출력, 검색 명령어
// ------------------------------------------
int cmd_ls(int argc, char *argv[]);   // 디렉토리 목록 조회
int cmd_pwd(int argc, char *argv[]);  // 현재 경로 출력
int cmd_cd(int argc, char *argv[]);   // 디렉토리 변경
int cmd_mkdir(int argc, char *argv[]); // 디렉토리 생성
int cmd_rmdir(int argc, char *argv[]); // 디렉토리 삭제
int cmd_cat(int argc, char *argv[]);  // 파일 내용 출력
int cmd_grep(int argc, char *argv[]); // 파일 내 문자열 검색

#endif