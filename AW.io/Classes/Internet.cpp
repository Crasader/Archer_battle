#if 0
#include "ODSocket"
USING_NS_CC;
void Internet::connectServer()
{
	// ��ʼ��
	ODSocket socket;
	socket.Init();
	socket.Create(AF_INET, SOCK_STREAM, 0);

	// ���÷�������IP��ַ���˿ں�
	// �����ӷ����� Connect
	const char* ip = "127.0.0.1";
	int port = 12345;
	bool result = socket.Connect(ip, port);

	// �������� Send
	socket.Send("login", 5);

	if (result) {
		CCLOG("connect to server success!");
		// �������̣߳������߳��У���������
		std::thread recvThread = std::thread(&HelloWorld::receiveData, this);
		recvThread.detach(); // �����̷߳���
	}
	else {
		CCLOG("can not connect to server");
		return;
	}
}
#endif