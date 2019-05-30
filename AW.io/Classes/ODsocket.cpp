#include "ODsocket.h"
#include<string>
#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#endif
ODsocket::ODsocket(SOCKET sock )
{
	m_sock = sock;
}


bool ODsocket::Create(int AF, int type, int protocal )
{
	m_sock = socket(AF, type, protocal);
	if (m_sock == INVALID_SOCKET)
	{
		return false;
	}
	return true;
}

bool ODsocket::Connect(const string &ip, unsigned short port)//�ͻ��������˷�����������
{
	struct sockaddr_in sevaddr;//����˵�ַ��Ϣ
	sevaddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	sevaddr.sin_family = AF_INET;
	sevaddr.sin_port = htons(port);
	memset(sevaddr.sin_zero, 0, sizeof(sevaddr.sin_zero));
	int ret = connect(m_sock, (struct sockaddr*)&sevaddr, sizeof(sevaddr));//���׽�����Ŀ�ĵذ���һ��
	if (ret == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}

bool ODsocket::Bind(unsigned short port)
{
	struct sockaddr_in sevaddr;//����˵�ַ��Ϣ
	sevaddr.sin_family = AF_INET;
	sevaddr.sin_addr.S_un.S_addr = INADDR_ANY;//bind����ֻ���Ķ˿ڲ����ĵ�ַ
	sevaddr.sin_port = htons(port);
	int opt = 1;
	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)	return false;
	int ret = bind(m_sock, (struct sockaddr*)&sevaddr, sizeof(sevaddr));
	if (ret == INVALID_SOCKET)
	{
		return false;
	}
	return true;
}

bool ODsocket::Listen(int backlog )
{
	
	if (listen(m_sock, backlog) == INVALID_SOCKET)
	{
		return false;
	}
	return true;
}

bool ODsocket::Accept(ODsocket &s, char *fromip )//����˴����������ȡ��һ���ͻ�����֮����ͨ�š����øú�����s�͸���������ÿͻ��˵�ͨ��
{
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	SOCKET socket = accept(m_sock, (struct sockaddr*)&cliaddr, &addrlen);
	if (socket == INVALID_SOCKET)
	{
		return false;
	}
	s = socket;
	if (fromip != NULL)
	{
		return true;
	}
}

int ODsocket::Send(char * buffer, int len, int flag)//����ʵ�ʷ��͵ĳ���
{
	int bytes;
	int count = 0;
	while (count < len)
	{
		bytes = send(m_sock, buffer + count, len - count, flag);
		if (bytes == 1 || !bytes)
		{
			return -1;
		}
		count += bytes;
	}
	return count;
}
int ODsocket::Recv(char *buffer, int len, int flag)
{
	return (recv(m_sock, buffer, len, flag));
}
int ODsocket::Close()
{
#ifdef WIN32
	m_sock = -1;
		return (closesocket(m_sock));
#else 
	return (close(m_sock));
#endif
}

int ODsocket::GetError()
{
#ifdef WIN32
	return (WSAGetLastError());
#else
	return (SOCKET_ERROR);
#endif
}

int ODsocket::Init()//0��ʾ��ʼ���ɹ���-1��ʾ��ʼ��ʧ��
{
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 0);
	int ret = WSAStartup(version, &wsaData);
	if (!ret)return 0;
	return -1;
}

int ODsocket::Clean()
{
#ifdef WIN32
	return (WSACleanup());
#endif
	return 0;
}
bool ODsocket::DnsParse(const char *domain, char * ip)//DNS����
{
	struct hostent* p;
	p = gethostbyname(domain);
	if (!p)return false;
	sprintf(ip, "%u.%u.%u.%u", (unsigned char)p->h_addr_list[0][0], (unsigned char)p->h_addr_list[0][1], (unsigned char)p->h_addr_list[0][2], (unsigned char)p->h_addr_list[0][3]);
	return true;
}

ODsocket& ODsocket::operator=(SOCKET s)//����ODsocket�Ⱥ������
{
	m_sock = s;
	return *this;
}
ODsocket::operator SOCKET()
{
	return m_sock;
}