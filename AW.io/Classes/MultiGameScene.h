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
#include<list>
using namespace cocos2d::network;
USING_NS_CC;
using namespace std;
class MultiGameScene :public SingleGameScene
{
public:
	
	
	CREATE_FUNC(MultiGameScene);
	virtual bool init();
	static Scene* CreateScene();

	//����ϵ��
	void postMessage(float t);
	void getMessage(float t);
	ODsocket* sock_client;//�ͻ����׽���
	std::string strmsg;//�ӷ��������յ�����Ϣ
	std::string	MessageToPost;//Ҫ���͸�����������Ϣ
	void StringToData();//����json��
	void DataToString();//�����ݷ�װ��json����ת����Ĵ�������MessageToPost��

	//���ݳ�Ա
	list<Arrow*>MyArrow;//����������ļ�
	Map<string, Person*> NameToPerson;//���������������
	set<string>dead;//��������
	list<Arrow*>AllArrowList;
	Person* RoleModel;
	list<Person*>AllPersonList;
	//������д
	void InitAllPerson();//���ݷ������㲥�������˵������������������˵ĳ�ʼλ��
	virtual void Shoot(float t);//ģ��������ҵ����
	virtual void Dead(float t);//�Լ�����������ת���������������а��ϳ���
	virtual void ArrowEnded(Touch* t, Event*e);//��Ҫ������ļ���ӽ�MyArrow������
	virtual void Hurt(float t);
	virtual void MoveArrow(float t);
	virtual void MovePerson(float t);
	virtual void ShowBlood(float t);
	void test(float t);
	Vec2 LastPos;
	Arrow* LastArrow;
	//���ܺ���
	Vec2 ToOpenGL(Vec2 pos);//��tile����ת��ΪOpenGL����
	//static void* AutoArrow(void* ptr);
	//debug
	int times = 0;
private:
};


