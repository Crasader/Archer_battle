#pragma once
#include "cocos2d.h"
#include "cocos2d\cocos\network\HttpClient.h" 
#include "cocos2d\cocos\network\HttpRequest.h"
#include "cocos2d\cocos\network\HttpResponse.h"
#include "ODsocket.h"
#include "Person.h"
#include "SingleGameScene.h"
#include "Arrow.h"
#include "Rocker.h"
using namespace cocos2d::network;
USING_NS_CC;
class MultiGameScene :public SingleGameScene
{
public:
	
	
	CREATE_FUNC(MultiGameScene);
	virtual bool init();//�麯����д��ʹ�ö�̬
	static Scene* CreateScene();
private:
	//����ϵ��
	void update(float t);
	static void* getMessage(void*);//�ӷ���˽�����Ϣ
	static void* postMessage(void*);//�����˷�����Ϣ
	ODsocket* sock_client;//�ͻ����׽���
	std::string strmsg;//�ӷ��������յ�����Ϣ
	std::string	MessageToPost;//Ҫ���͸�����������Ϣ
	void StringToData(const std::string &s);//����json��
	void DataToString();//�����ݷ�װ��json����ת����Ĵ�������MessageToPost��


	//���ݳ�Ա
	Vector<Arrow*>MyArrow;//����������ļ�
	
	void InitAllPerson();//���ݷ������㲥�������˵������������������˵ĳ�ʼλ��
private:
	void Get(float t);
	void Post(float t);
	void onHttpRequestCompleted(HttpClient *sender, HttpResponse *response);
};
