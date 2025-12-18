# 🐚 MyShell: 리눅스 시스템 콜 기반 커스텀 쉘

> **동의대학교 컴퓨터소프트웨어공학과 3학년 2학기 오픈소스 프로젝트** > **리눅스 표준 입출력 및 프로세스 제어 기법을 활용한 쉘(Shell) 프로그램 구현**

![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat-square&logo=linux&logoColor=black)
![C](https://img.shields.io/badge/C-A8B9CC?style=flat-square&logo=c&logoColor=black)
![Shell](https://img.shields.io/badge/Shell_Script-121011?style=flat-square&logo=gnu-bash&logoColor=white)

## 📌 프로젝트 소개
이 프로젝트는 리눅스 운영체제의 핵심 인터페이스인 CLI 쉘(Shell)을 직접 설계하고 구현한 결과물입니다.  
단순히 외부 명령어(`/bin/*`)를 실행하는 것을 넘어, **시스템 콜(System Call)을 직접 활용**하여 파일 시스템 제어, 프로세스 생성, IPC(파이프, 리다이렉션), 시그널 핸들링 등의 운영체제 핵심 원리를 심도 있게 구현하였습니다.

특히, 단일 파일 구조를 탈피한 모듈화 설계(Main-Execute-Command)와 **Makefile을 통한 빌드 자동화**를 통해 유지보수성과 확장성을 극대화했습니다.

## 👥 팀원 구성 (7조)

| 학번 | 이름 | 역할 | 담당 업무 (Role) |
| :--- | :--- | :--- | :--- |
| **20212977** | **이규찬** | **팀장 (Main)** | • 전체 아키텍처 설계 및 일정 관리<br>• 메인 루프 및 입력 파싱(Parsing)<br>• 시그널 핸들링 (Shell 보호)<br>• 작업 관리 (`jobs` 명령어) |
| **20233065** | **이지민** | **팀원 (Execute)** | • 프로세스 생성 및 실행 제어 (`fork`, `exec`)<br>• IPC 구현 (파이프 , 리다이렉션 `>`, `<`)<br>• 백그라운드 프로세스 처리 (`&`) |
| **20212979** | **임진호** | **팀원 (Commands)** | • 내부 명령어 11종 시스템 콜 직접 구현<br>• 파일/디렉토리 제어 (`ls`, `cp`, `mv` 등)<br>• 텍스트 처리 (`grep`, `cat` 등) |

## ✨ 주요 기능 (Features)

### 1. 프로세스 제어 및 작업 관리
- **백그라운드 실행 (`&`):** 명령어 뒤에 `&`를 붙여 쉘을 멈추지 않고 프로세스 실행.
- **좀비 프로세스 방지:** `SIGCHLD` 핸들러를 통해 종료된 백그라운드 프로세스 자원 자동 회수.
- **작업 목록 (`jobs`):** 현재 실행 중인 백그라운드 작업 리스트 확인.

### 2. 강력한 입출력 제어 (IPC)
- **파이프 (`|`):** 다중 명령어의 표준 입출력을 연결 (예: `ls -l | grep .c`).
- **리다이렉션 (`>`, `<`):** 파일로 출력 저장 또는 파일 내용을 입력으로 사용.
- **복합 사용:** 파이프와 리다이렉션을 동시에 사용하여 복잡한 데이터 흐름 제어 가능.

### 3. 시그널 핸들링 (Signal Handling)
- **쉘 보호:** `Ctrl-C(SIGINT)`, `Ctrl-Z(SIGQUIT)` 입력 시 쉘이 종료되지 않음.
- **프로세스 제어:** 실행 중인 포그라운드 작업만 시그널에 의해 종료되도록 제어.

### 4. 내부 명령어 직접 구현 (Built-in Commands)
`exec` 계열 함수를 사용하지 않고, 운영체제 API를 사용하여 직접 구현하였습니다.
- **디렉토리:** `cd`, `pwd`, `mkdir`, `rmdir`
- **파일 조작:** `ls` (옵션 -a, -l 지원), `cp`, `mv`, `rm`, `ln`
- **텍스트:** `cat`, `grep`

## 🛠️ 설치 및 실행 방법 (Getting Started)

### 사전 요구 사항
- Linux 환경 (Ubuntu 권장)
- GCC 컴파일러

### 빌드 및 실행 (Bash)
```bash
# 1. 저장소 클론
git clone [https://github.com/leekyuchan0215/OpenSource_Project.git](https://github.com/leekyuchan0215/OpenSource_Project.git)
cd OpenSource_Project

# 2. 컴파일 (Makefile 사용)
make

# 3. 쉘 실행
./myshell
```

### 종료 및 청소
```bash
# 쉘 종료
myshell$ exit

# 빌드 파일 제거
make clean
```

## 📂 디렉토리 구조 (Architecture)

```plaintext
OpenSource_Project/
├─ main.c        # [Controller] 메인 루프, 파싱, 시그널 처리
├─ execute.c     # [Engine] 프로세스 실행, 파이프, 리다이렉션 로직
├─ commands.c    # [Tools] 내부 명령어(ls, cp, grep 등) 구현체
├─ myshell.h     # [Header] 구조체 정의 및 함수 프로토타입 선언
├─ Makefile      # [Build] 빌드 자동화 스크립트
└─ README.md     # 프로젝트 문서
```

## 🛠 개발 환경

- OS: Ubuntu Linux

- Language: C (Standard C99)

- Tools: GCC, Make, Vim/VSCode, Git
---
© 2025 Team 4. All Rights Reserved.
