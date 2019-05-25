#include "ODsocket.h"
#include<pthread.h>
#include<windows.h>
#include<list>
#include<iostream>
#include<string>
#include<cstring>
#pragma comment(lib, "pthreadVC2.lib")
list<ODsocket>AllClient;//�������пͻ���
list<string>AllMessage;//������յ���������Ϣ
using namespace std;
void* getMessage(void * ptr)
{
	ODsocket* nowsock = (ODsocket*)ptr;//ptr����������뵱ǰ�ͻ��˵�ͨ��
	char msg[1024];
	memset(msg, 0, sizeof(msg));
	while (1)
	{
		if (nowsock == NULL || nowsock->m_sock == NULL || nowsock->m_sock <= 0)
		{
			break;
		}
		nowsock->Recv(msg, sizeof(msg));
		//strcat(msg, "\n");
		//std::cout << "������Ϣ" << msg << endl;
		string msgstr = msg;
		AllMessage.push_front(msgstr);
	}
	return NULL;
}

void * DoSocket(void * ptr)//����������пͻ��˹㲥
{
	std::cout << "�㲥�߳��Ѿ�����" << endl;
	while (1)
	{
		if (AllClient.size())
		{
			if (AllMessage.size())
			{
				list<string>::iterator nowmsg;
				for (nowmsg = AllMessage.begin(); nowmsg != AllMessage.end(); nowmsg++)
				{
					std::string msg = *nowmsg;
					list<ODsocket>::iterator nowsock;
					for (nowsock = AllClient.begin(); nowsock != AllClient.end(); nowsock++)
					{
						if (nowsock->m_sock < 0) break;
						nowsock->Send((char*)msg.c_str(), msg.length(), 0);//�㲥
					}
				}
				AllMessage.clear();
			}
		}
		Sleep(1);
	}
}

int main()
{
	pthread_t get;//������Ϣ���߳�
	pthread_t post;//�㲥��Ϣ���߳�
	int ret;
	ret = pthread_create(&post, NULL, DoSocket, NULL);//���ڷ���������пͻ��˹㲥���߳�
	if (ret)
	{
		printf("�����㲥�߳�ʧ��\n");
		exit(1);
	}
	//1.���������socket
	ODsocket SockServer;
	SockServer.Init();
	bool res = SockServer.Create(AF_INET, SOCK_STREAM, 0);
	if (!res)
	{
		cout << "���������socketʧ��" << endl;
	}
	//2.�󶨶˿�
	res = SockServer.Bind(8867);
	if (!res)
	{
		cout << "�󶨶˿�ʧ��" << endl;
	}
	//3.����
	res = SockServer.Listen();
	if (!res)
	{
		cout << "����ʧ��" << endl;
	}
	char message[30];//message����û������ģ�ֻ�������赲return 0��
	memset(message, 0, sizeof(message));
	//4.���տͻ��˵�����
	while (1)
	{

		ODsocket* SockClient = new ODsocket();//�����뵱ǰ�ͻ��˵�ͨ��
		char *address = new char[20];
		std::cout << "accpt������" << endl;
		SockServer.Accept(*SockClient, address);//�˺�SockClient����������ÿͻ��˵�ͨ��,address����ͻ��˵�ַ
		std::cout << "���������" << endl;
		//SockClient->Send(message, sizeof(message));//ΪʲôҪ������Ϣ����
		ret = pthread_create(&get, NULL, getMessage, SockClient);//����������Ϣ���߳�
		AllClient.push_back(*SockClient);
		cin >> message;//�赲return 0��
		return 0;
	}
}