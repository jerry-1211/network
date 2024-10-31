#include "csapp.h"

#include <sys/types.h>
#include <sys/socket.h>

// command 127.0.0.1 8080
// argc : command, 127.0.0.1, 8080  => 3개
// argv : command, 127.0.0.1, 8080  => 문자열 그대로

// 에코 클라이언트의 메인 루틴
int main(int argc, char **argv) {       // argc->들어오는 명령어의 인자 개수, **argv->들어오는 명령어들
    
    int clientfd;                       // 서버와의 연결을 위한 클라이언트 소켓 디스크립터/ 식별자 역할
    char *host, *port, buf[MAXLINE];    // 서버의 호스트 이름, 포트 번호, 그리고 데이터를 저장할 버퍼
    rio_t rio;                          // rio 라이브러리의 구조체, 데이터 송수신에서 오류가 발생하지 않도록 도움

    // 예외처리
    if (argc != 3) {                    // 명령, IP, 포트 번호 총 3개 필요
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]); // 오류 메시지 출력
        exit(0);                        // 프로그램 종료
    }
                                        // 0 번째 명령어
    host = argv[1];                     // 첫 번째 명령행 인자를 호스트 이름으로 설정
    port = argv[2];                     // 두 번째 명령행 인자를 포트 번호로 설정

    clientfd = Open_clientfd(host, port);// 서버와의 연결을 생성, 실패 시 오류 메시지 출력 및 종료
    Rio_readinitb(&rio, clientfd);       // 소켓을 사용할 Rio 구조체 초기화, 버퍼링된 I/O 지원

    while (Fgets(buf, MAXLINE, stdin) != NULL) { // 표준 입력으로부터 한 줄씩 읽어옴
                                            //표준 입력에서 한 줄씩 읽기: Fgets 함수는 표준 입력(stdin)으로부터 한 줄씩 읽어와 buf에 저장하며,
                                            // NULL일 경우 EOF를 의미합니다. 반복문이 계속 실행되면서 사용자 입력이 버퍼에 저장됩니다.
        Rio_writen(clientfd, buf, strlen(buf));  // 입력된 줄을 서버에 보냄
        Rio_readlineb(&rio, buf, MAXLINE);       // 서버로부터 응답을 읽어옴, 줄 단위로 버퍼링됨
        Fputs(buf, stdout);                      // 서버 응답을 표준 출력에 출력
    }
    Close(clientfd);                    // 서버와의 연결 종료
    exit(0);                            // 프로그램 정상 종료
}