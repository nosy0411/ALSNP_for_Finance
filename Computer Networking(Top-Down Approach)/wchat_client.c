/*---------------------
파일명 : wchat_client.c  
	PC version console mode chat client, socket을 Non-blocking 으로 사용
기  능 : 서버에 접속한 후 키보드의 입력을 서버로 전달하고, 
	     	서버로부터 오는 메시지를 화면에 출력한다.
			gotoxy 를 사용하여 화면을 제어한다.
컴파일 : 
사용법 : chat_client ip port name
-----------------------*/

#include <winsock.h>
#include <signal.h>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>


// 색상 정의
#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7
#define GRAY2	8
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15

COORD curpos;
void cls(int bg_color, int text_color) // 화면 지우기
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}

void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
	COORD pos = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API 함수입니다. 이건 알필요 없어요
}
int wherex(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csb;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csb);
    return (csb.dwCursorPosition.X);
}
int wherey(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csb;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csb);
    return (csb.dwCursorPosition.Y);
}
void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), fg_color | bg_color<<4);
}

#define MAXLINE 128

char *escapechar = "exit";
char name[256];  					/* 채팅에서 사용할 이름 */
WSADATA wsadata;
int	main_socket;

void exit_callback(int sig)
{
	closesocket(main_socket);
	WSACleanup();
	exit(0);
}

void init_winsock()
{
	// winsock 사용을 위해 필수적임
	WORD sversion;

	signal(SIGINT, exit_callback);
	sversion = MAKEWORD(1,1);
	WSAStartup(sversion, &wsadata);
}

int main(int argc, char *argv[]) {
	char line[MAXLINE], message[MAXLINE + 1];
	struct sockaddr_in server_addr;
	int s;  /* 서버와 연결된 소켓번호 */
	u_long iMode = 1;
	char *ip_addr = "";
	int port_no = 30040;

	init_winsock();

	/*if(argc != 4) {
	printf("사용법 : %s sever_IP port name \n", argv[0]);
	exit(0);
	}*/

	/* 채팅 참가자 이름 구조체 초기화 */
	//sprintf(name, "[%s]", argv[3]);
	printf("Enter name : ");
	gets(name);

	/* 소켓 생성 */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Client : Can't open stream socket.\n");
		exit(0);
	}
	main_socket = s;
	/* 채팅 서버의 소켓주소 구조체 server_addr 초기화 */
	memset((char *)&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	server_addr.sin_port = htons(port_no);

	/* 연결요청 */
	if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Client : Can't connect to server.\n");
		exit(0);
	} else {
		printf("서버에 접속되었습니다.\n\n");
	}

	ioctlsocket(s,FIONBIO,&iMode); // read 를 non-blocking 으로 만든다.

	while(1) {
		int size;
		if ((size = recv(s, message, MAXLINE, 0)) > 0)  { // non-blocking read
				//size = recv(s, message, MAXLINE, 0);
				message[size] = '\0';
				if (strstr(message, name)!=NULL) { // 본인의 message인 경우 우측에 출력
					textcolor(YELLOW2, BLACK);
					sprintf(line, "%79s", message);
					printf("%s\n", line);
				} else { // 다른 사람 messae {
					textcolor(BLACK, WHITE);
					printf("%s\n", message);
				}
		}
		if (kbhit()) { // key 가 눌려있으면 read key --> write to chat server
			curpos.X = wherex(); // 원래 cursor 위치를 기억한다.
			curpos.Y = wherey();
			gotoxy(0, 22); // 제일 아래 라인으로 이동
			textcolor(BLACK, WHITE);
			if(gets(message)) {
				sprintf(line, "%s: %s\n", name, message);
				if (send(s, line, strlen(line)+1, 0) < 0)
					printf("Error : Write error on socket.\n");
				if (strstr(message, escapechar) != NULL ) {
					printf("Good bye.\n");
					closesocket(s);
					exit(0); 
				}
			}
			gotoxy(0, 22); 
			memset(message, ' ', strlen(message)); // 마지막 message를 지운다.
			printf("%s", message);
			gotoxy(curpos.X, curpos.Y); // 원래 위치로 돌아간다.
		}
	}
}
