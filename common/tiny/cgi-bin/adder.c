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

  buf = getenv("QUERY_STRING");  
  /* Extract the two arguments */
  if (buf != NULL)
  {
    p = strchr(buf, '&'); // '&' 위치 찾기
    if (p != NULL)
    {
      *p = '\0'; // '&'를 '\0'로 바꿔서 첫 번째 숫자를 추출할 준비

      // '&'를 기준으로 숫자 두 개를 추출
      sscanf(buf,"num1=%s",arg1);
      sscanf(p+1,"num2=%s",arg2);
      
      n1 = atoi(arg1); // 문자열을 정수로 변환
      n2 = atoi(arg2);
    }
  }

  /* Make the response body */
  sprintf(content, "QUERY_STRING=%s\r\n", getenv("QUERY_STRING"));
  strcat(content, "Welcome to add.com: ");
  strcat(content, "THE Internet addition portal.\r\n<p>");
  sprintf(content + strlen(content), "The answer is: %d + %d = %d\r\n<p>", n1, n2, n1 + n2);
  strcat(content, "Thanks for visiting!\r\n");

  /* Generate the HTTP response */\
  printf("HTTP/1.0 200 OK\r\n");                     // 상태 코드 추가
  printf("Connection: close\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Content-type: text/html\r\n\r\n");
  printf("%s", content);
  fflush(stdout);

  exit(0);
}
