#include "csapp.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


// 반복적 echo 서버 메인 루틴
void echo(int connfd);                  // 클라이언트와의 연결을 처리하는 echo 함수의 프로토타입 선언

int main(int argc, char **argv) {
    int listenfd, connfd;               // listenfd: 수신 소켓 디스크립터, connfd: 클라이언트와의 연결 디스크립터
    socklen_t clientlen;                // 클라이언트 주소 길이를 저장하는 변수
    struct sockaddr_storage clientaddr; // 클라이언트 주소 정보를 저장할 구조체, 모든 주소 형식과 호환
    char client_hostname[MAXLINE], client_port[MAXLINE]; // 클라이언트의 호스트 이름과 포트 번호를 저장할 배열

    // 명령행 인자 개수 확인: 포트 번호가 없다면 오류 메시지 출력 후 종료
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);  // 사용법 메시지 출력
        exit(0);  // 프로그램 종료
    }

    // 주어진 포트 번호로 서버 소켓을 열고, 해당 포트에서 들어오는 연결 요청을 대기
    listenfd = Open_listenfd(argv[1]);

    // 무한 루프를 통해 클라이언트의 연결 요청을 계속해서 수락하고 처리
    while (1) {
        clientlen = sizeof(struct sockaddr_storage); // 클라이언트 주소 구조체 크기 초기화

        // connect fd는 클라이언트와 서버 사이에 성립된 연결의 끝점
        // 서버가 연결 요청을 수락할 때 마다 생성되며,
        // 서버가 클라이언트에 서비스하는 동안에만 존재
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // 클라이언트의 연결 요청 수락

        printf("Server connect File descriptor : %d \n", connfd);
        
        
        // 클라이언트 주소를 호스트 이름과 포트 번호로 변환하여 사람이 읽을 수 있는 형식으로 저장
        Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE, 
                    client_port, MAXLINE, 0);
        
        // 연결된 클라이언트 정보 출력
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        
        echo(connfd);  // 클라이언트와의 연결에서 데이터를 송수신하고 응답하는 echo 함수 호출
        Close(connfd); // 클라이언트와의 연결 종료
    }
    exit(0);  // 프로그램 종료 (실제로는 실행되지 않음, 무한 루프이기 때문)
}
