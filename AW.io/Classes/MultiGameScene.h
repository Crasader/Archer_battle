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
using namespace std;
class MultiGameScene :public SingleGameScene
{
public:
	
	
	CREATE_FUNC(MultiGameScene);
	virtual bool init();
	static Scene* CreateScene();
private:
	//����ϵ��
	void update(float t);
	//static void* getMessage(void*);//�ӷ���˽�����Ϣ
	//static void* postMessage(void*);//�����˷�����Ϣ
	void postMessage(float t);
	void getMessage(float t);
	ODsocket* sock_client;//�ͻ����׽���
	std::string strmsg;//�ӷ��������յ�����Ϣ
	std::string	MessageToPost;//Ҫ���͸�����������Ϣ
	void StringToData(float t);//����json��
	void DataToString();//�����ݷ�װ��json����ת����Ĵ�������MessageToPost��

	//���ݳ�Ա
	Vector<Arrow*>MyArrow;//����������ļ�
	Map<string, Person*> NameToPerson;//���������������
	set<string>dead;//��������
	//������д
	//void InitAllPerson();//���ݷ������㲥�������˵������������������˵ĳ�ʼλ��
	//virtual void MoveAllPerson(float t);//���������˵�λ��
	//virtual void Shoot(float t);//ģ��������ҵ����
	virtual void Dead(float t);//�Լ�����������ת���������������а��ϳ���
	virtual void ArrowEnded(Touch* t, Event*e);//��Ҫ������ļ���ӽ�MyArrow������

	//���ܺ���
	Vec2 ToOpenGL(Vec2 pos);//��tile����ת��ΪOpenGL����

	//debug
	int times = 0;
private:
	void Get(float t);
	void Post(float t);
	void onHttpRequestCompleted(HttpClient *sender, HttpResponse *response);
};
