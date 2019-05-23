#pragma once
#include<winsock.h>
#include<string>
typedef int socklen_t;
//#include<iostram>
using namespace std;
class ODsocket
{
public:
	ODsocket(SOCKET sock = INVALID_SOCKET);//���캯��

	int SocketID;

	bool Create(int AF, int type, int protocal = 0);

	bool Connect(const string &ip, unsigned short port);

	bool Bind(unsigned short port);

	bool Listen(int backlog = 5);

	bool Accept(ODsocket &s, char *fromip = NULL);

	int Send(char * buffer, int len, int flag = 0);

	int Recv(char *buffer, int len, int flag=0);
	
	int Close();

	int GetError();

	static int Init();

	static int Clean();

	static bool DnsParse(const char *domain, char * ip);//DNS����

	ODsocket&  operator=(SOCKET s);//����ODsocket�Ⱥ������

	operator SOCKET();

	SOCKET m_sock;

};

