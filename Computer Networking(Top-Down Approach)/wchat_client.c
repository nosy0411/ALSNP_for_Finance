/*---------------------
���ϸ� : wchat_client.c  
	PC version console mode chat client, socket�� Non-blocking ���� ���
��  �� : ������ ������ �� Ű������ �Է��� ������ �����ϰ�, 
	     	�����κ��� ���� �޽����� ȭ�鿡 ����Ѵ�.
			gotoxy �� ����Ͽ� ȭ���� �����Ѵ�.
������ : 
���� : chat_client ip port name
-----------------------*/

#include <winsock.h>
#include <signal.h>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>


// ���� ����
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
void cls(int bg_color, int text_color) // ȭ�� �����
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}

void gotoxy(int x, int y) //���� ���ϴ� ��ġ�� Ŀ�� �̵�
{
	COORD pos = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API �Լ��Դϴ�. �̰� ���ʿ� �����
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
char name[256];  					/* ä�ÿ��� ����� �̸� */
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
	// winsock ����� ���� �ʼ�����
	WORD sversion;

	signal(SIGINT, exit_callback);
	sversion = MAKEWORD(1,1);
	WSAStartup(sversion, &wsadata);
}

int main(int argc, char *argv[]) {
	char line[MAXLINE], message[MAXLINE + 1];
	struct sockaddr_in server_addr;
	int s;  /* ������ ����� ���Ϲ�ȣ */
	u_long iMode = 1;
	char *ip_addr = "";
	int port_no = 30040;

	init_winsock();

	/*if(argc != 4) {
	printf("���� : %s sever_IP port name \n", argv[0]);
	exit(0);
	}*/

	/* ä�� ������ �̸� ����ü �ʱ�ȭ */
	//sprintf(name, "[%s]", argv[3]);
	printf("Enter name : ");
	gets(name);

	/* ���� ���� */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Client : Can't open stream socket.\n");
		exit(0);
	}
	main_socket = s;
	/* ä�� ������ �����ּ� ����ü server_addr �ʱ�ȭ */
	memset((char *)&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	server_addr.sin_port = htons(port_no);

	/* �����û */
	if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Client : Can't connect to server.\n");
		exit(0);
	} else {
		printf("������ ���ӵǾ����ϴ�.\n\n");
	}

	ioctlsocket(s,FIONBIO,&iMode); // read �� non-blocking ���� �����.

	while(1) {
		int size;
		if ((size = recv(s, message, MAXLINE, 0)) > 0)  { // non-blocking read
				//size = recv(s, message, MAXLINE, 0);
				message[size] = '\0';
				if (strstr(message, name)!=NULL) { // ������ message�� ��� ������ ���
					textcolor(YELLOW2, BLACK);
					sprintf(line, "%79s", message);
					printf("%s\n", line);
				} else { // �ٸ� ��� messae {
					textcolor(BLACK, WHITE);
					printf("%s\n", message);
				}
		}
		if (kbhit()) { // key �� ���������� read key --> write to chat server
			curpos.X = wherex(); // ���� cursor ��ġ�� ����Ѵ�.
			curpos.Y = wherey();
			gotoxy(0, 22); // ���� �Ʒ� �������� �̵�
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
			memset(message, ' ', strlen(message)); // ������ message�� �����.
			printf("%s", message);
			gotoxy(curpos.X, curpos.Y); // ���� ��ġ�� ���ư���.
		}
	}
}
