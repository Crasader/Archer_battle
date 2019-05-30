#pragma once
#include "cocos2d.h"
#include "Arrow.h"
#include <string>
USING_NS_CC;
using namespace std;
class Arrow;
class Person :public Sprite
{
public:
	static Person* CreatePerson(const std::string& filename);//��������
	void InitPerson(const std::string &filename);//Ϊ����ĸ������Ը�ֵ
	int max_blood;//Ѫ������
	int blood;//��ǰѪ��
	int blue;//��ǰ����
	float attack;//�������Ŵ���
	float defence;//������
	int speed;//�ƶ��ٶ�
	int arrow_speed;//�����ƶ��ٶ�
	int person_score;//����÷�
	string name;//�������
	CREATE_FUNC(Person);
	Arrow* weapon;
};