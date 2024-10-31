# MakeFile 형식
# Targer : DEPENDENCY
# 	command

#  컴파일러 : gcc 사용
CC = gcc

# 컴파일 옵션 : -g는 디버그 정보 / -Wall은 경고 출력
CFLAGS = -g -Wall

# 링커 옵션 : -lpthread  (pthreads 라이브러리 링크)
LDFLAGS = -lpthread

# 빌드 규칙 (proxy와 echoClient 타겟을 빌드)
all: proxy echoClient echoServer

# 첫 줄은 빌드 규칙
csapp.o: csapp.c csapp.h 
# 두번째 줄은 빌드 명령어 
	$(CC) $(CFLAGS) -c csapp.c

# echo
echo.o: echo.c csapp.h
	$(CC) $(CFLAGS) -c echo.c  # 탭으로 수정됨

# echoServer
echoServer.o: echoServer.c csapp.h
	$(CC) $(CFLAGS) -c echoServer.c

echoServer: echoServer.o csapp.o echo.o  
	$(CC) $(CFLAGS) echoServer.o csapp.o echo.o -o echoServer $(LDFLAGS)

# echoClient 
echoClient.o: echoClient.c csapp.h
	$(CC) $(CFLAGS) -c echoClient.c

echoClient: echoClient.o csapp.o
	$(CC) $(CFLAGS) echoClient.o csapp.o -o echoClient $(LDFLAGS)

# proxy
proxy.o: proxy.c csapp.h
	$(CC) $(CFLAGS) -c proxy.c

proxy: proxy.o csapp.o
	$(CC) $(CFLAGS) proxy.o csapp.o -o proxy $(LDFLAGS)


#--------------------------------------------------------------------------------------
# Creates a tarball in ../proxylab-handin.tar that you can then
# hand in. DO NOT MODIFY THIS!
# 프로젝트 제출을 위한 아카이브 파일 규칙
# 이 규칙을 실행하면 프로젝트의 필요하지 않은 파일들 정리 후 tar 명령어를 사용해 제출용 파일 생성
# handin:
# 	(make clean; cd ..; tar cvf $(USER)-proxylab-handin.tar proxylab-handout --exclude tiny --exclude nop-server.py --exclude proxy --exclude driver.sh --exclude port-for-user.pl --exclude free-port.sh --exclude ".*")
#--------------------------------------------------------------------------------------


#  clean 규칙을 실행해 불필요한 파일 삭제
clean:
	rm -f *~ *.o proxy core *.tar *.zip *.gzip *.bzip *.gz