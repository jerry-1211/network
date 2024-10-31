// /*
//  * 각 함수의 역할과 구현 방향
//  *
//  * 1. doit(int fd):
//  *    - 클라이언트의 요청을 처리하는 주요 함수입니다.
//  *    - HTTP 요청을 읽고 파싱한 다음, 요청된 파일이 정적 콘텐츠(파일)인지 동적 콘텐츠(CGI 프로그램)인지 구분하고,
//  *      적절한 함수를 호출하여 응답합니다.
//  *
//  * 2. read_requesthdrs(rio_t *rp):
//  *    - 클라이언트가 보낸 HTTP 헤더를 읽어오는 함수입니다.
//  *    - Tiny 서버는 요청 헤더를 사용하지 않으므로, 이 함수는 헤더를 읽기만 하고 무시하는 역할을 합니다.
//  *
//  * 3. parse_uri(char *uri, char *filename, char *cgiargs):
//  *    - 요청된 URI를 분석하여 요청이 정적 파일을 위한 것인지, 동적 콘텐츠(CGI 프로그램)를 위한 것인지를 판별합니다.
//  *    - filename과 cgiargs를 구분하고, 정적 콘텐츠와 동적 콘텐츠를 구분하는 규칙을 적용하여 필요한 정보를 반환합니다.
//  *
//  * 4. serve_static(int fd, char *filename, int filesize):
//  *    - 정적 콘텐츠 파일을 읽어 클라이언트에게 전달하는 함수입니다.
//  *    - 파일 크기와 콘텐츠 타입을 응답 헤더로 보내고, 실제 파일 데이터를 응답 본문으로 전송합니다.
//  *
//  * 5. get_filetype(char *filename, char *filetype):
//  *    - 요청된 파일의 확장자를 확인하여 MIME 타입을 결정합니다.
//  *    - .html, .gif, .jpg, .png 등의 확장자를 인식하고, 해당하는 MIME 타입을 설정합니다.
//  *
//  * 6. serve_dynamic(int fd, char *filename, char *cgiargs):
//  *    - 동적 콘텐츠(CGI 스크립트)를 실행하고, 그 결과를 클라이언트에 전송하는 함수입니다.
//  *    - fork()와 execve()를 통해 CGI 프로그램을 실행하고, 결과를 클라이언트에 전달합니다.
//  *
//  * 7. clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg):
//  *    - 요청에 문제가 있을 경우 클라이언트에게 HTTP 에러 응답을 전송하는 함수입니다.
//  *    - 예를 들어, 잘못된 HTTP 메서드(예: POST 요청) 등에서 에러 메시지를 생성하고 이를 응답으로 보냅니다.
//  *
//  * 다음 단계
//  * 1. 각 함수의 역할과 요청/응답의 흐름을 파악하면서, 하나씩 구현해보세요.
//  * 2. main 함수의 흐름을 유지하면서 각 함수가 제대로 동작하는지 테스트해보세요.
//  * 3. HTTP 프로토콜 규격에 맞는 응답을 보낼 수 있도록 상태 코드, 헤더, 본문을 정확하게 구현하는 것도 중요합니다.
//  */

#include "csapp.h"

void doit(int fd); 
void read_requesthdrs(rio_t *rp); 
int parse_uri(char *uri, char *filename, char *cgiargs); 
void serve_static(int fd, char *filename, int filesize); 
void get_filetype(char *filename, char *filetype); 
void serve_dynamic(int fd, char *filename, char *cgiargs); 
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, 
                 char *longmsg);

int main(int argc, char **argv) {
    int listenfd, connfd;               // 서버의 수신 소켓과 클라이언트와의 연결 소켓 파일 디스크립터
    char hostname[MAXLINE], port[MAXLINE];  // 클라이언트의 호스트 이름과 포트 번호를 저장할 배열
    socklen_t clientlen;                // 클라이언트 주소 구조체 크기를 저장하는 변수
    struct sockaddr_storage clientaddr; // 클라이언트의 주소 정보를 저장하는 구조체

    /* 명령줄 인자 확인 */
    // 서버 실행 시 포트 번호를 인자로 받아야 함. 인자 개수가 2개가 아니면 오류 메시지 출력 후 종료
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    /* 수신 소켓 열기 */
    // Open_listenfd 함수로 입력받은 포트 번호에 바인딩된 수신 소켓을 생성
    // listenfd는 서버가 클라이언트 연결 요청을 대기하는 데 사용될 파일 디스크립터
    listenfd = Open_listenfd(argv[1]);

    /* 무한 루프를 통해 클라이언트 요청을 처리 */
    while (1) {
        clientlen = sizeof(clientaddr);   // clientaddr 구조체의 크기를 clientlen에 설정
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);  // 클라이언트 연결 요청 수락
        // Accept가 성공하면 새롭게 할당된 소켓 connfd가 반환되며, 클라이언트와의 개별 통신에 사용됨

        /* 클라이언트 정보 출력 */
        // 클라이언트의 호스트 이름과 포트 번호를 알아내어 출력
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);

        /* 요청 처리 */
        // doit 함수 호출로 클라이언트 요청을 처리
        // doit 함수는 클라이언트의 요청을 읽고, 정적 또는 동적 콘텐츠를 전송하는 작업을 수행
        doit(connfd);

        /* 연결 종료 */
        // doit 함수가 완료되면 클라이언트와의 연결을 닫고 다음 연결 요청을 대기
        Close(connfd);
    }
}

// 클라이언트의 요청을 처리 - HTTP요청을 읽고 파싱 -> 정적콘텐츠, 동적콘텐츠 구분 및 적절 함수 호출
void doit(int fd) {
    int is_static;                            // 요청된 콘텐츠가 정적 콘텐츠인지 동적 콘텐츠인지 구분하는 플래그 변수
    struct stat sbuf;                         // 파일의 상태 정보를 저장하는 구조체
    char buf[MAXLINE];                        // 클라이언트의 요청을 저장할 버퍼
    char method[MAXLINE];                     // 요청 메서드(GET, POST 등)를 저장할 문자열
    char uri[MAXLINE];                        // 요청된 URI(파일 경로)를 저장할 문자열
    char version[MAXLINE];                    // HTTP 버전 정보를 저장할 문자열
    char filename[MAXLINE];                   // 요청된 URI로부터 파싱한 파일 이름을 저장할 문자열
    char cgiargs[MAXLINE];                    // 동적 콘텐츠 요청 시 사용될 CGI 인수를 저장할 문자열
    rio_t rio;                                // Robust I/O 구조체, 읽기 버퍼 역할을 수행


    /* Step 1: 요청 라인 및 헤더 읽기 */
    Rio_readinitb(&rio, fd);                  // 소켓과 연결된 fd를 초기화하여 읽기 버퍼로 설정
    Rio_readlineb(&rio, buf, MAXLINE);        // 요청 라인의 첫 번째 줄을 읽어 buf에 저장
    printf("Request headers:\n");             // 요청 헤더 출력 시작
    printf("%s", buf);                        // 요청 라인(첫 줄) 출력
    sscanf(buf, "%s %s %s", method, uri, version); // 요청 라인에서 메서드, URI, 버전을 추출

    // printf("***************************\n");
    // printf("the version : %s \n",version);
    // printf("***************************\n");


    /* Step 2: HTTP 메서드 확인 */
    // Tiny 서버는 GET 메서드만 지원하므로, 다른 메서드가 요청될 경우 오류 메시지를 반환하고 종료
    // strcasecmp 함수는 두 문자열을 대소문자 구분하지 않고 같은지 확인
    //  같은 경우 0 반환/ s1이 더 큰 경우 양수 / s1이 더 작은 경우 음수
    if (strcasecmp(method, "GET")) {          // 요청 메서드가 "GET"이 아닐 경우
       clienterror(fd, method, "501", "Not implemented",
                    "Tiny does not implement this method"); // 501 오류 반환: 지원하지 않는 메서드
       return;                                // 함수 종료
    }


    // Step 3: 헤더 읽기
    read_requesthdrs(&rio);                   // 나머지 요청 헤더들을 읽어오지만 Tiny 서버에서는 사용하지 않으므로 무시


    /* Step 4: URI 파싱하여 파일 이름 및 CGI 인수 추출 */
    // URI를 분석하여 요청이 정적 파일인지, 동적 콘텐츠(CGI 프로그램)인지를 구분하고, 필요한 정보를 설정
    is_static = parse_uri(uri, filename, cgiargs); // 정적/동적 콘텐츠 구분, filename과 cgiargs 추출

 ;
    

    if (stat(filename, &sbuf) < 0) {           // 요청된 파일의 상태를 검사, 파일이 없으면 -1 반환
        clienterror(fd, filename, "404", "Not found",
                    "Tiny couldn’t find this file"); // 404 오류 반환: 요청한 파일을 찾을 수 없음
        return;                                // 함수 종료
    }


    /* Step 5: 정적 콘텐츠 처리 */
    // 요청된 파일이 정적 콘텐츠일 경우, 파일이 일반 파일이며 읽기 권한이 있는지 확인
    if (is_static) {                           // is_static이 참일 경우 정적 콘텐츠로 간주
        if (!(S_ISREG(sbuf.st_mode)) ||        // 일반 파일인지 확인
            !(S_IRUSR & sbuf.st_mode)) {       // 읽기 권한이 있는지 확인
            clienterror(fd, filename, "403", "Forbidden",
                        "Tiny couldn’t read the file"); // 403 오류 반환: 읽기 권한 없음
            return;                            // 함수 종료
        }
        serve_static(fd, filename, sbuf.st_size); // 정적 콘텐츠 전송 함수 호출, 파일 크기를 인수로 전달
    }
    /* Step 6: 동적 콘텐츠 처리 */
    // 요청된 파일이 동적 콘텐츠일 경우, 파일이 일반 파일이며 실행 권한이 있는지 확인
    else {
        if (!(S_ISREG(sbuf.st_mode)) ||        // 일반 파일인지 확인
            !(S_IXUSR & sbuf.st_mode)) {       // 실행 권한이 있는지 확인
            clienterror(fd, filename, "403", "Forbidden",
                        "Tiny couldn’t run the CGI program"); // 403 오류 반환: 실행 권한 없음
            return;                            // 함수 종료
        }
        serve_dynamic(fd, filename, cgiargs);  // 동적 콘텐츠 전송 함수 호출, CGI 인수 전달
    }
}

// 클라이언트가 보낸 HTTP헤더를 읽어오는 함수
void read_requesthdrs(rio_t *rp) {
    char buf[MAXLINE];

    /* 헤더 읽기 */
    // 빈 줄("\r\n")이 나올 때까지 한 줄씩 읽어 무시함
    Rio_readlineb(rp, buf, MAXLINE);
    
    // 문자열 비교 함수 (대소문자 구분)
    // 동일한 경우 0 
    while (strcmp(buf, "\r\n")) {        // 빈 줄이 아니면 계속해서 헤더를 읽어들임
        printf("%s", buf);               // 읽은 헤더 내용을 출력 (디버깅용)
        Rio_readlineb(rp, buf, MAXLINE); // 다음 줄 읽기
    }
    return;
}
// 요청된 URI를 분석하여 요청이 정적 파일을 위한 것인지, 동적 콘텐츠(CGI 프로그램)를 위한 것인지를 판별함
int parse_uri(char *uri, char *filename, char *cgiargs) {
    char *ptr;                   // '?' 위치를 가리키는 포인터

    /* Step 1: URI가 CGI-bin(동적 콘텐츠) 요청인지 확인 */
    // Tiny 서버에서는 "/cgi-bin/" 문자열을 포함하는 URI를 동적 콘텐츠 요청으로 처리합니다.
    // strstr  함수는 문자열 검색 함수로 문자열 내에서 특정 부분 문자열 찾는데 사용
    if (!strstr(uri, "cgi-bin")) {          // "/cgi-bin/"이 없으면 정적 콘텐츠로 간주
        strcpy(cgiargs, "");                // 정적 콘텐츠이므로 CGI 인수는 빈 문자열로 설정
        // 아래 과정을 통해서 ./index.html 이런 경로 완성 
        strcpy(filename, ".");              // 현재 디렉터리로부터 파일 경로 생성
        strcat(filename, uri);              // filename에 URI 추가하여 경로 생성

        /* URI가 '/'로 끝나면 기본 파일 이름을 추가 */
        if (uri[strlen(uri) - 1] == '/')    // URI가 "/"로 끝나면 기본 파일 설정
            strcat(filename, "home.html");  // 기본 파일 이름 "home.html"을 추가
        return 1;                           // 정적 콘텐츠인 경우 1 반환
    } 
    /* Step 2: 동적 콘텐츠 요청일 경우 */
    else {                                  // URI에 "/cgi-bin/"이 포함되어 있으면 동적 콘텐츠
        ptr = strchr(uri, '?');             // '?' 위치를 찾아서 인수를 구분
        if (ptr) {                          // '?'가 있으면 CGI 인수 추출
            strcpy(cgiargs, ptr + 1);       // '?' 다음의 문자열을 cgiargs에 복사
            *ptr = '\0';                    // '?' 위치를 null로 변경하여 파일 이름만 남김
        } else {                            // '?'가 없으면 CGI 인수는 없음
            strcpy(cgiargs, "");             // CGI 인수는 빈 문자열로 설정
        }
        strcpy(filename, ".");              // 현재 디렉터리로부터 파일 경로 생성
        strcat(filename, uri);              // filename에 URI 추가하여 경로 생성
        return 0;                           // 동적 콘텐츠인 경우 0 반환
    }
}
// 정적 콘텐츠 파일을 읽어 클라이언트에게 전달하는 함수, 파일 크기와 콘텐츠 타입을 응답 헤더로 보내고, 실제 파일 데이터를 응답 본문으로 전송
void serve_static(int fd, char *filename, int filesize) {
    int srcfd;                              // 정적 파일을 가리킬 파일 디스크립터
    char *srcp, filetype[MAXLINE], buf[MAXBUF]; // MIME 타입과 전송할 데이터를 담을 버퍼
    rio_t rio;

    /* Step 1: 파일의 MIME 타입 결정 */
    get_filetype(filename, filetype);       // 파일의 확장자에 따라 MIME 타입을 결정


    /* Step 2: 응답 헤더 작성 */
    sprintf(buf, "HTTP/1.0 200 OK\r\n");    // HTTP 상태 코드 200 OK 설정
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf); // 서버 정보 헤더 추가
    sprintf(buf, "%sConnection: close\r\n", buf);       // 연결 종료 헤더 추가
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize); // 콘텐츠 길이 설정
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype); // MIME 타입 설정
    Rio_writen(fd, buf, strlen(buf));       // 클라이언트로 응답 헤더 전송

    printf("Response headers:\n%s", buf);   // 헤더 내용을 서버 콘솔에 출력 (디버깅용)

    // /* Step 3: 파일 내용을 클라이언트에 전송 */
    // srcfd = Open(filename, O_RDONLY, 0);    // filename을 읽기 전용으로 열기
    // // void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
    // srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); // 파일을 메모리에 매핑
    // Close(srcfd);                           // 파일 디스크립터 닫기 (매핑 후 필요 없음)
    // Rio_writen(fd, srcp, filesize);         // 매핑된 파일 내용을 클라이언트로 전송
    // Munmap(srcp, filesize);                 // 매핑 해제하여 메모리 자원 반환} 


    // OPEN 함수는 파일 디스크립터를 생성하여 반환
    // 파일에 접근할 수 있지만, 내용 자체를 읽거나 쓰지는 않음 
    srcfd = Open(filename,O_RDONLY,0);  

    // 크기 할당 
    srcp = Malloc(filesize);  

    // 여기서 Rio_readn은 파일 디스크립터에서 비디오 파일 내용을 읽기 위해 사용
    Rio_readn(srcfd, srcp, filesize);  

    Close(srcfd);
    Rio_writen(fd,srcp,filesize);
    Free(srcp);

}

// 요청된 파일의 확장자를 확인하여 MIME 타입을 결정합니다. .html, .gif, .jpg, .png 등의 확장자를 인식하고, 해당하는 MIME 타입을 설정
void get_filetype(char *filename, char *filetype) {
    /* 파일 확장자 확인을 통한 MIME 타입 결정 */
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");          // HTML 파일
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");          // GIF 이미지 파일
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");          // PNG 이미지 파일
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");         // JPEG 이미지 파일
    else if (strstr(filename,"mp4"))
        strcpy(filetype, "video/mp4");          //  mp4 비디오 파일
    else
        strcpy(filetype, "text/plain");         // 기본값: 일반 텍스트 파일
}

// 동적 콘텐츠(CGI 스크립트)를 실행하고, 그 결과를 클라이언트에 전송하는 함수, fork()와 execve()를 통해 CGI 프로그램을 실행하고, 결과를 클라이언트에 전달
void serve_dynamic(int fd, char *filename, char *cgiargs) {
    char buf[MAXLINE], *emptylist[] = { NULL }; // 클라이언트로 전송할 응답 헤더와 execve 인자 리스트

    /* Step 1: HTTP 응답 헤더 작성 */
    // sprintf(buf, "HTTP/1.0 200 OK\r\n");               // 상태 코드 200 OK
    // sprintf(buf, "%sServer: Tiny Web Server\r\n", buf); // 서버 정보 헤더
    // Rio_writen(fd, buf, strlen(buf));                   // 응답 헤더를 클라이언트에 전송

    /* Step 2: 자식 프로세스 생성 및 CGI 프로그램 실행 */
    // 자식 프로세스 경우 0 반환, 부모 프로세스 경우 자식 PID 반환
    if (Fork() == 0) {             // 자식 프로세스 생성, 자식 프로세스가 실행 
    
            
        // CGI 인수를 환경 변수 QUERY_STRING에 설정
        // 마지막 인지 1은 기존에 동일한 환경 변수 있어도 덮어씀 
        setenv("QUERY_STRING", cgiargs, 1);       

        // 파일 디스크립터를 복사하여 프로세스의 표준 출력을 특정 파일 또는 소켓으로 리다이렉션
        // int dup2(int oldfd, int newfd);
        // 자식 프로세스가 CGI 프로그램을 실행하면 CGI 프로그램의 모든 출력이 클라이언트에게 전달
        // 모든 표준 출력 함수 (printf, puts, putchar 등)는 클라이언트 소켓으로 출력
        // STDOUT_FILENO는 표준 파일 디스크립터  (0:입력 / 1:출력 / 2:오류)
        Dup2(fd, STDOUT_FILENO);                  

        // printf("*****************\n");
        // printf("이게 되나 ?? \n");
        // printf("*****************\n");

        // execve는 자식 프로세스에서 현재 프로세스를 새로운 프로그램으로 대체
        // 새로운 자식 프로세스 만든 후 자식 프로세스를 CGI 프로그램으로 대체(execve) 실행
        // 부모 프로세스 영향 없이 다른 작업 수행 가능 
        Execve(filename, emptylist, environ);     // CGI 프로그램 실행 (filename에 지정된 파일)
        
    }

    /* Step 3: 부모 프로세스가 자식 프로세스 종료 대기 */
    Wait(NULL);    // 부모 프로세스는 자식이 종료될 때까지 대기
}

// 요청에 문제가 있을 경우 클라이언트에게 HTTP 에러 응답을 전송하는 함수
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
    char buf[MAXLINE], body[MAXBUF];

    /* Step 1: HTTP 응답 본문 생성 */
    sprintf(body, "<html><title>Tiny Error</title>");                 // HTML 페이지 시작
    sprintf(body, "%s<body bgcolor=\"ffffff\">\r\n", body);           // 흰색 배경 설정
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);            // 에러 코드와 간단한 메시지 추가
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);           // 상세 에러 메시지와 원인 추가
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);    // 페이지 하단에 서버 이름 추가

    /* Step 2: HTTP 응답 헤더 작성 및 전송 */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);             // 상태 코드와 간단한 메시지
    Rio_writen(fd, buf, strlen(buf));                                 // 응답 헤더 전송
    sprintf(buf, "Content-type: text/html\r\n");                      // 콘텐츠 타입 설정
    Rio_writen(fd, buf, strlen(buf));                                 // 응답 헤더 전송
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));    // 콘텐츠 길이 설정
    Rio_writen(fd, buf, strlen(buf));                                 // 응답 헤더 전송

    /* Step 3: HTTP 응답 본문 전송 */
    Rio_writen(fd, body, strlen(body));                               // 에러 응답 본문 전송
}

