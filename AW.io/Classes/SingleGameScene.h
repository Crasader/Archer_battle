#pragma once
#include "cocos2d.h"
#include "TDpoint.h"
#include "Arrow.h"
#include "Person.h"
#include<vector>
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
	static const int is_stop = 15;//�ϰ���ͼ���GID

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
	TMXTiledMap* map;//��ͼ
	TMXLayer* stop; //�ϰ���


	//�����ص�����ϵ��
	bool MoveBegan(Touch* t, Event* e);//�����ƶ�
	void MoveMoved(Touch* t, Event* e);
	void MoveEnded(Touch* t, Event* e);
	void MovePerson(float t);

	bool ArrowBegan(Touch* t, Event *e);//���
	void ArrowMoved(Touch* t, Event*e);
	void ArrowEnded(Touch* t, Event*e);
	void MoveArrow(float t);
	
	
	//���ܺ���
	Vec2 exchange(Vec2 pos);

	//��ʱ��debug�ã�
	std::vector<int>AllGid;
	struct p
	{
		std::pair<int, int>xy;
		int gid;
		p(std::pair<int, int> xy, int gid) :xy(xy), gid(gid) {}
	};
	std::vector<p>movement;
};