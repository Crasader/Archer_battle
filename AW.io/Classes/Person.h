#pragma once
#include "cocos2d.h"
#include "Arrow.h"
#include <string>
USING_NS_CC;
class Person :public Sprite
{
public:
	static Person* create(const std::string& filename);//��������
	void set();//Ϊ����ĸ������Ը�ֵ
	int max_blood;//Ѫ������
	int blood;//��ǰѪ��
	int blue;//��ǰ����
	float attack;//�������Ŵ���
	float defence;//������
	int speed;//�ƶ��ٶ�
	int arrow_speed;//�����ƶ��ٶ�
	Arrow weapon;
};