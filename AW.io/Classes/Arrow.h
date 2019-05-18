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
	Vec2 StartPosition;//������ʼλ
	Person* master;//��������
	static Arrow*CreateArrow(const std::string &filename);
	void InitArrow(const std::string &filename);
	std::string picture;
	CREATE_FUNC(Arrow);
};