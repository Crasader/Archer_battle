#pragma once
#include "cocos2d.h"
#include "TDpoint.h"
#include "Arrow.h"
#include "Person.h"
USING_NS_CC;
class SingleGameScene :public Layer
{
public:
	//��ʼ��ϵ�к���
	CREATE_FUNC(SingleGameScene);
	bool init();
	static Scene* CreateScene();
	void InitAllPoint(TMXTiledMap*map);


	//��ǩϵ��
	static const int MapTag = 88;//��ͼ�ı�ǩ
	static const int LayerTag = 120;//������ı�ǩ
	static const int RockerTag = 66;//ҡ�˱�ǩ
	static const int ModelTag = 99;//���Ǳ�ǩ
	static const int tag_rocker = 50;//ң�˱�ǩ
	static const int tag_rockerBG = 60;//ҡ�˱�����ǩ

	//��������
	Vector<TDpoint*> allpoint;//�������е�
	Vector<Arrow*>AllArrow;//�������м�
	Vector<Person*>AllPerson;//����������
	std::set<Arrow*>se;//������ֹ�ظ�addchild

	//ȫ������
	float R;//ҡ�˰뾶
	Vec2 current_point;//��ǰ������  �������������ƶ�
	Vec2 rockerBG_Position;//ҡ�����ĵ������
	Vec2 start;//���ʱ��ʼ�Ĵ�������

	//�����ص�����ϵ��
	bool MoveBegan(Touch* t, Event* e);//�����ƶ�
	void MoveMoved(Touch* t, Event* e);
	void MoveEnded(Touch* t, Event* e);
	void MovePerson(float t);

	bool ArrowBegan(Touch* t, Event *e);//���
	void ArrowMoved(Touch* t, Event*e);
	void ArrowEnded(Touch* t, Event*e);
	void MoveArrow(float t);
	
	

	
};