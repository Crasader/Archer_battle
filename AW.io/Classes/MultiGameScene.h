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

	//初始化
	CREATE_FUNC(MultiGameScene);
	virtual bool init();
	static Scene* CreateScene();

	//联网系列
	ODsocket* sock_client;//客户端套接字
	void postMessage(float t);//发送信息
	void getMessage(float t);//接受信息
	std::string strmsg;//从服务器接收到的信息
	std::string	MessageToPost;//要发送给服务器的信息
	void StringToData();//解析json串
	void DataToString();//将数据封装成json串，转化后的串储存在MessageToPost中
	void connect();


	//存储容器
	//list<Arrow*>MyArrow;//保存我射出的箭
	//list<Arrow*>AllArrowList;
	Map<string, Person*> NameToPerson;//根据姓名查找玩家
	set<string>dead;//死亡名单
	Person* RoleModel;//玩家
	list<Person*>AllPersonList;//保存所有人
	Vec2 LastPos;//上一次的位置
	Arrow* LastArrow;//发出的箭
	string LastWord;
	string Ip;//服务器地址

	//函数重写
	void InitAllPerson();
	virtual void Dead(float t);//自己死，场景跳转，别人死，从排行榜上除名
	virtual void ArrowEnded(Touch* t, Event*e);//需要把射出的箭赋值给LastArrow

	virtual void Hurt(float t); //四个用链表重新实现的函数
	virtual void MoveArrow(float t);
	virtual void MovePerson(float t);
	virtual void ShowBlood(float t);

	
	//输入昵称编辑框
	void textFieldEvent(Ref *pSender, cocos2d::ui::TextField::EventType type);
	cocos2d::ui::TextField* textField;//编辑框对象

	void LandMsg(Ref* pSender);

	//聊天室
	list<string> AllWord;//保存聊天记录
	list<Label*> AllLabel;//显示聊天记录的label
	void ShowChat(bool is_clear);//更新label显示
	cocos2d::ui::TextField* ChatField;//聊天文本编辑框
	void chat(Ref *pSender, cocos2d::ui::TextField::EventType type);//对输入的内容进行处理
	cocos2d::ui::Button* back;//聊天室关闭按钮
	cocos2d::ui::Button* button;//聊天室开启按钮
	bool is_chatting;//是否处在聊天框界面
	//debug
	void test(float t);



};


