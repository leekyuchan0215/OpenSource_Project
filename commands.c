<<<<<<< HEAD
/*
 * cmd_file.c
 *
 * [역할]
 * 파일 시스템 관련 명령어 구현 (pwd,cd,ls,mkdir,rmdir,cp,rm,mc,chat,ln,grep)
 *
 * [작성자]
 * 임진호
 */

#include "myshell.h"

// 빌트인 명령어인지 확인
int is_builtin(char *cmd) {
    if (cmd == NULL) return 0;
    if (strcmp(cmd, "ls") == 0) return 1;
    if (strcmp(cmd, "cd") == 0) return 1;
    if (strcmp(cmd, "pwd") == 0) return 1;
    if (strcmp(cmd, "mkdir") == 0) return 1;
    if (strcmp(cmd, "rmdir") == 0) return 1;
    if (strcmp(cmd, "cp") == 0) return 1;
    if (strcmp(cmd, "mv") == 0) return 1;
    if (strcmp(cmd, "rm") == 0) return 1;
    if (strcmp(cmd, "cat") == 0) return 1;
    if (strcmp(cmd, "ln") == 0) return 1;
    if (strcmp(cmd, "grep") == 0) return 1;
    return 0;
}
// 빌트인 명령어 분기 처리
void do_builtin(char **argv) {
    if (strcmp(argv[0], "ls") == 0) cmd_ls(argv);
    else if (strcmp(argv[0], "cd") == 0) cmd_cd(argv);
    else if (strcmp(argv[0], "pwd") == 0) cmd_pwd(argv);
    else if (strcmp(argv[0], "mkdir") == 0) cmd_mkdir(argv);
    else if (strcmp(argv[0], "rmdir") == 0) cmd_rmdir(argv);
    else if (strcmp(argv[0], "cp") == 0) cmd_cp(argv);
    else if (strcmp(argv[0], "mv") == 0) cmd_mv(argv);
    else if (strcmp(argv[0], "rm") == 0) cmd_rm(argv);
    else if (strcmp(argv[0], "cat") == 0) cmd_cat(argv);
    else if (strcmp(argv[0], "ln") == 0) cmd_ln(argv);
    else if (strcmp(argv[0], "grep") == 0) cmd_grep(argv);
}
// --- 명령어 상세 구현 ---

// 1. pwd 구현
void cmd_pwd(char **argv) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd error");
    }
}

// 2. cd 구현 (홈 디렉토리 지원)
void cmd_cd(char **argv) {
    char *path = argv[1];
    
    // 인자가 없으면 홈 디렉토리로 이동
    if (path == NULL) {
        path = getenv("HOME");
    }

    if (chdir(path) != 0) {
        perror("cd error");
    }
}

// 3. ls 구현 (옵션 -a, -l )
void cmd_ls(char **argv) {
    DIR *d;
    struct dirent *dir;
    struct stat fileStat;
    char buf[1024];
    int show_all = 0;
    int show_detail = 0;
    char *path = ".";

    // 옵션 파싱 (간단한 형태)
    for (int i = 1; argv[i] != NULL; i++) {
        if (argv[i][0] == '-') {
            if (strchr(argv[i], 'a')) show_all = 1;
            if (strchr(argv[i], 'l')) show_detail = 1;
        } else {
            path = argv[i];
        }
    }

    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // -a 옵션이 없고 .으로 시작하면 건너뜀
            if (!show_all && dir->d_name[0] == '.') continue;

            if (show_detail) {
                // 상세 정보 출력을 위한 stat 호출
                sprintf(buf, "%s/%s", path, dir->d_name);
                if (stat(buf, &fileStat) < 0) {
                    perror("stat error");
                    continue;
                }

                // 권한 출력 (rwx)
                printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
                printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
                printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
                printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
                printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
                printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
                printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
                printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
                printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
                
                // 크기 및 이름 출력
                printf("\t%ld\t%s\n", fileStat.st_size, dir->d_name);
            } else {
                // 기본 출력
                printf("%s  ", dir->d_name);
            }
        }
        if (!show_detail) printf("\n");
        closedir(d);
    } else {
        perror("ls error");
    }
}

// 4. mkdir 구현
void cmd_mkdir(char **argv) {
    if (argv[1] == NULL) {
        fprintf(stderr, "mkdir: missing operand\n");
        return;
    }
    if (mkdir(argv[1], 0755) != 0) {
        perror("mkdir error");
    }
}

// 5. rmdir 구현
void cmd_rmdir(char **argv) {
    if (argv[1] == NULL) {
        fprintf(stderr, "rmdir: missing operand\n");
        return;
    }
    if (rmdir(argv[1]) != 0) {
        perror("rmdir error");
    }
}

// 6. cp 구현 (파일 복사)
void cmd_cp(char **argv) {
    int fd_src, fd_dest;
    char buffer[4096];
    ssize_t nread;

    if (argv[1] == NULL || argv[2] == NULL) {
        fprintf(stderr, "usage: cp source_file dest_file\n");
        return;
    }

    fd_src = open(argv[1], O_RDONLY);
    if (fd_src == -1) {
        perror("cp source error");
        return;
    }

    fd_dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dest == -1) {
        perror("cp dest error");
        close(fd_src);
        return;
    }

    while ((nread = read(fd_src, buffer, sizeof(buffer))) > 0) {
        if (write(fd_dest, buffer, nread) != nread) {
            perror("cp write error");
            break;
        }
    }

    close(fd_src);
    close(fd_dest);
}

// 7. rm 구현 (파일 삭제)
void cmd_rm(char **argv) {
    if (argv[1] == NULL) {
        fprintf(stderr, "rm: missing operand\n");
        return;
    }
    if (unlink(argv[1]) != 0) {
        perror("rm error");
    }
}

// 8. mv 구현 (이동 또는 이름 변경)
void cmd_mv(char **argv) {
    if (argv[1] == NULL || argv[2] == NULL) {
        fprintf(stderr, "usage: mv source dest\n");
        return;
    }
    // 같은 파일 시스템 내에서는 rename으로 가능
    if (rename(argv[1], argv[2]) != 0) {
        perror("mv error");
    }
}

// 9. cat 구현
void cmd_cat(char **argv) {
    int fd;
    char buffer[4096];
    ssize_t nread;

    if (argv[1] == NULL) {
        fprintf(stderr, "cat: missing operand\n");
        return;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("cat error");
        return;
    }

    while ((nread = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, nread);
    }
    close(fd);
}

// 10. ln 구현
void cmd_ln(char **argv) {
    if (argv[1] == NULL || argv[2] == NULL) {
        fprintf(stderr, "usage: ln source dest\n");
        return;
    }
    if (link(argv[1], argv[2]) != 0) {
        perror("ln error");
    }
}

// 11. grep 구현 (파일 또는 표준 입력에서 문자열 검색)
void cmd_grep(char **argv) {
    if (argv[1] == NULL) {
        fprintf(stderr, "grep: usage: grep [pattern] [file]\n");
        return;
    }

    char *pattern = argv[1];
    char *filename = argv[2]; // 파일명이 없으면 NULL
    FILE *fp = stdin;         // 기본은 표준 입력(파이프 지원)

    // 파일명이 주어졌다면 파일을 엽니다.
    if (filename != NULL) {
        fp = fopen(filename, "r");
        if (fp == NULL) {
            perror("grep error");
            return;
        }
    }

    char buffer[MAX_CMD_BUF];
    // 한 줄씩 읽어서 패턴이 있는지 검사
    while (fgets(buffer, MAX_CMD_BUF, fp) != NULL) {
        if (strstr(buffer, pattern) != NULL) {
            printf("%s", buffer);
        }
    }

    // 파일을 열었던 경우에만 닫습니다.
    if (filename != NULL) fclose(fp);
}
=======
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
>>>>>>> cf41e1adf142d935c47afb3b23fc74c2914dbe52
