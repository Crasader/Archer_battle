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
#include "ui/CocosGUI.h"
#include<list>
using namespace cocos2d::network;
USING_NS_CC;
using namespace std;
class MultiGameScene :public SingleGameScene
{
public:

	//��ʼ��
	CREATE_FUNC(MultiGameScene);
	virtual bool init();
	static Scene* CreateScene();

	//����ϵ��
	ODsocket* sock_client;//�ͻ����׽���
	void postMessage(float t);//������Ϣ
	void getMessage(float t);//������Ϣ
	std::string strmsg;//�ӷ��������յ�����Ϣ
	std::string	MessageToPost;//Ҫ���͸�����������Ϣ
	void StringToData();//����json��
	void DataToString();//�����ݷ�װ��json����ת����Ĵ�������MessageToPost��

	//�洢����
	//list<Arrow*>MyArrow;//����������ļ�
	//list<Arrow*>AllArrowList;
	Map<string, Person*> NameToPerson;//���������������
	set<string>dead;//��������
	Person* RoleModel;//���
	list<Person*>AllPersonList;//����������
	Vec2 LastPos;//��һ�ε�λ��
	Arrow* LastArrow;//�����ļ�

	//������д
	void InitAllPerson();
	virtual void Dead(float t);//�Լ�����������ת���������������а��ϳ���
	virtual void ArrowEnded(Touch* t, Event*e);//��Ҫ������ļ���ֵ��LastArrow

	virtual void Hurt(float t); //�ĸ�����������ʵ�ֵĺ���
	virtual void MoveArrow(float t);
	virtual void MovePerson(float t);
	virtual void ShowBlood(float t);

	//�༭��
	void textFieldEvent(Ref *pSender, cocos2d::ui::TextField::EventType type);
	cocos2d::ui::TextField* textField;//�༭�����

	//debug
	void test(float t);



};


