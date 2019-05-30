#include "ODsocket.h"
#include<pthread.h>
#include<windows.h>
#include<list>
#include<iostream>
#include<string>
#include<cstring>
#include<vector>
#pragma comment(lib, "pthreadVC2.lib")
vector<ODsocket>AllClient;//�������пͻ���
list<string>AllMessage;//������յ���������Ϣ
using namespace std;
void* getMessage(void * ptr)
{
	ODsocket* nowsock = (ODsocket*)ptr;//ptr����������뵱ǰ�ͻ��˵�ͨ��
	char msg[1024];
	int count = 0;
	while (1)
	{
		if (nowsock == NULL || nowsock->m_sock == NULL || nowsock->m_sock <= 0)//����ͻ��˶Ͽ����ӣ�ֹͣ�ý���
		{
			cout<<"client close";
			break;
		}
		memset(msg, 0, sizeof(msg));
		nowsock->Recv(msg, sizeof(msg));
		string msgstr = msg;
		AllMessage.push_back(msgstr);
	}
	return NULL;
}

void * DoSocket(void * ptr)//����������пͻ��˹㲥
{
	std::cout << "�㲥�߳��Ѿ�����" << endl;
	int count = 0;
	while (1)
	{
		if (AllClient.size()&&AllMessage.size())
		{
		
			for (auto nowsock : AllClient)
			{
				if (nowsock.m_sock < 0)
				{
					cout<<"cloent close";
				}
				
				for (list<string>::iterator it=AllMessage.begin();it!=AllMessage.end();++it)
				{
					string nowmsg = *it;
					nowsock.Send((char*)nowmsg.c_str(), nowmsg.length(), 0);//�㲥
					cout << "��������" << ++count << "�ι㲥��Ϣ:" << nowmsg << endl;
				}
			}
			AllMessage.clear();
		}
	}
}

int main()
{
	pthread_t get;//������Ϣ���߳�
	pthread_t post;//�㲥��Ϣ���߳�
	int ret;
	//AllMessage.reserve(3000);
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
		ret = pthread_create(&get, NULL, getMessage, SockClient);//�������ոÿͻ��˷��͵���Ϣ���߳�
		AllClient.push_back(*SockClient);
		
	}
	cin >> message;//�赲return 0��
	return 0;
}