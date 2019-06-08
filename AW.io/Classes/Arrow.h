#pragma once
#include "cocos2d.h"
#include "Person.h"
USING_NS_CC;
class Person;
class Arrow :public Sprite
{
public:
	int arrow_attack;
	int arrow_size;//���ĳߴ磬�������������ײ���
	double dir;
	int speed;
	int range;//���
	bool is_weapon;
	bool is_attack;//����������
	bool is_range;//�������
	bool is_speed;//�����ٶ�
	Vec2 StartPosition;//������ʼλ
	Person* master;//��������
	static Arrow*CreateArrow(const std::string &filename);
	void InitArrow(const std::string &filename);
	inline double distance(Vec2 pos1, Vec2 pos2);
	std::string picture;
	CREATE_FUNC(Arrow);
};