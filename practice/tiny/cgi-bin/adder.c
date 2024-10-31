/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */

/* $end adder */

#include "csapp.h"

int main(void)
{
  char *buf, *p;
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int n1 = 0, n2 = 0;

  //  getenv 함수는 클라이언트가 URL에 추가한 쿼리 문자열 반환
  //  ex) http://example.com/cgi-bin/script?name=Alice&age=25
  //  -> name=Alice&age=25 반환 
  buf = getenv("QUERY_STRING"); 
  printf("what is buf :  %s \n" ,buf);
  
  if ( buf != NULL)
  {
    printf("**************************\n");
    printf("this is getenv value : %s \n",buf);
    printf("**************************\n");


    
    p = strchr(buf, '&'); // & 위치 p로 반환
    if (p != NULL)
    {
      *p = '\0'; // '&'를 '\0'로 바꿔서 첫 번째 숫자를 추출할 준비

      // '&'를 기준으로 숫자 두 개를 추출
      strcpy(arg1, buf);   // 첫 번째 숫자 추출  ex) name=Alice
      strcpy(arg2, p + 1); // 두 번째 숫자 추출  ex) age=25

      // (숫자 섞여있는) 문자열을 정수로 변환
      n1 = atoi(arg1); 
      n2 = atoi(arg2);
    }
  }


  /* Make the response body */
  // sprinf 함수는 결과를 표준 출력이 아닌 문자열 버퍼에 저장
  // 문자열 생성(형식화된 데이터로 파일명 or 메시지 생성에 유용)
  // int sprintf(char *str, const char* format, ... )
  // 출력된 전체 문자의 개수 반환 
  sprintf(content, "QUERY_STRING=%s", getenv("QUERY_STRING"));
  sprintf(content, "Welcome to add.com: ");
  sprintf(content, "%sTHE Internet addition portal.\r\n<p>", content);
  sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>",
          content, n1, n2, n1 + n2);
  sprintf(content, "%sThanks for visiting!\r\n", content);
                               
  /* Generate the HTTP response */
  printf("Connection: close\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Content-type: text/html\r\n\r\n");
  printf("%s", content);

  // 출력 버퍼를 비우기 위해 사용 
  fflush(stdout);

  exit(0);
}