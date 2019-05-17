#pragma once
#include "cocos2d.h"
#include "TDpoint.h"
#include "Arrow.h"
#include "Person.h"
#include<vector>
#include<set>
USING_NS_CC;
class SingleGameScene :public Layer
{
public:
	//��ʼ��ϵ�к���
	CREATE_FUNC(SingleGameScene);
	bool init();
	static Scene* CreateScene();
	void InitAllPoint(TMXTiledMap*map);
	void InitValue();
	void initWeapon();//��ʼ������
private:


	//��ǩϵ��
	static const int MapTag = 88;//��ͼ�ı�ǩ
	static const int LayerTag = 120;//������ı�ǩ
	static const int RockerTag = 66;//ҡ�˱�ǩ
	static const int ModelTag = 99;//���Ǳ�ǩ
	static const int tag_rocker = 50;//ң�˱�ǩ
	static const int tag_rockerBG = 60;//ҡ�˱�����ǩ
	static const int is_stop = 15;//�ϰ���ͼ���GID
	static const int BloodTag = 68;


	//��������
	Vector<TDpoint*> allpoint;//�������е�
	Vector<Arrow*>AllArrow;//�������м�
	Vector<Person*>AllPerson;//����������
	//std::set<Arrow*>se;//������ֹ�ظ�addchild
	Map<Person*, Person*> hash_table;//ȷ��ÿ���˵��ƶ�����
	Map<Person*, Sprite*> blood_hash;//��ʾÿ�������Ѫ��
	Vector<Arrow*>AllWeapon;//����������������

	//ȫ������
	float R;//ҡ�˰뾶
	Vec2 current_point;//��ǰ������  �������������ƶ�
	Vec2 rockerBG_Position;//ҡ�����ĵ������
	Vec2 start;//���ʱ��ʼ�Ĵ�������
	TMXTiledMap* map;//��ͼ
	TMXLayer* stop; //�ϰ���
	TMXLayer* background;//������
	int height;//����������ĵ����������ε������ߵľ���
	int MonsterNumber;//ά�־��������ٵĹ������
	int low;//�����������ĺ���������Сֵ
	int high;//�����������ĺ�����������ֵ
	int attack_area;//��Բattack_area�ڣ�������ˣ���Ĭ�ϳ������ƶ�
	int min_attack_area;//����֮������������Сֵ
	Person* NullPerson;
	Vec2 One;//�ĸ����޵ĵ�ͼ�ǵ�����
	Vec2 Two;
	Vec2 Three;
	Vec2 Four;
	static const  int DY = 80;//Ѫ��������ͷ���ĸ߶�

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
	Vec2 exchange(Vec2 pos);//��openGL����ת��Ϊ��Ƭ��ͼ����
	bool check(Vec2 pos);//�ж��ܷ��ƶ�
	inline int random(int a, int b);//�����
	inline double distance(Vec2 pos1, Vec2 pos2);//openGL�������
	void GetPos();//��õ�ͼ���ĸ��ǵ�����
	void ShowBlood(float t);
	//���AI
	void CreateMonster(float t);
	void MoveDirect(float t);//ȷ��ÿ�����ƶ��ĳ���
	void MoveAllPerson(float t);//���ݳ�������ƶ�ÿһ����
	void Shoot(float t);
	void Hurt(float t);//���������ײ���
	void Dead(float t);//�˵������ж�����Դ����
	void InitMonster();
	//debug
	int counts;

};