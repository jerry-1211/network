#include <stdio.h>

int main() {
    printf("Hello, World!\n");        // 줄바꿈만
    printf("Hello, World!\r\n");      // 줄바꿈과 함께 커서를 맨 앞으로 이동
    printf("Hello, \rOverwritten!\n"); // \r로 앞부분을 덮어씀
    printf("Hello,\nWorld!\n");       // \n을 사용하여 새로운 줄에 출력

    return 0;
}
