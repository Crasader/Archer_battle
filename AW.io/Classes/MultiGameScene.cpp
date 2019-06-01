#include "MultiGameScene.h"
#include "HelloWorldScene.h"
#include"ODsocket.h"
#include<pthread.h>
#include<cstring>
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "Person.h"
#include "Arrow.h"
#include "Rocker.h"
#pragma comment(lib, "pthreadVC2.lib")
using namespace  rapidjson;
using namespace std;
USING_NS_CC;
static MultiGameScene* now;

Scene* MultiGameScene::CreateScene()
{
	auto scene = Scene::create();
	auto layer = MultiGameScene::create();
	scene->addChild(layer);
	return scene;
}
bool MultiGameScene::init()
{
	if (!Layer::init())return false;
	//初始化
	auto winsize = Director::getInstance()->getWinSize();
	this->InitValue();
	this->initWeapon();
	this->LastPos = Vec2(0, 0);
	this->map = TMXTiledMap::create("Tank2.tmx");
	map->setPosition(Vec2(-1500, -1500));
	this->addChild(map, -1);
	GetPos();
	InitMap();
	//创建主角
	auto model = Person::CreatePerson("person.png");
	model->setTag(ModelTag);
	this->addChild(model);
	model->setPosition(Vec2(winsize.width / 2, winsize.height / 2));
	AllPersonList.push_back(model);
	RoleModel = model;
	//通信
	now = this;
	sock_client = new ODsocket();
	sock_client->Init();
	bool res = sock_client->Create(AF_INET, SOCK_STREAM, 0);
	res = sock_client->Connect("100.67.186.176", 8867);
	if (res)
	{
		pthread_t gost;
		bool ok_1 = pthread_create(&gost, NULL, MultiGameScene::postMessage, NULL);//创建发送信息的线程
		pthread_t get;
		bool ok_2 = pthread_create(&get, NULL, MultiGameScene::getMessage, NULL);//创建接受信息的线程
	}



	//加载所有玩家
	InitAllPerson();

	//创建星星和心
	InitHeartStar();

	//设置摇杆
	auto rocker = Rocker::create("CloseSelected.png", "rr.png", rockerBG_Position);
	rocker->setTag(RockerTag);
	this->addChild(rocker);
	rocker->StartRocker();

	//游戏逻辑

	//this->schedule(schedule_selector(MultiGameScene::Shoot, this));
	this->schedule(schedule_selector(MultiGameScene::Hurt, this));//我方箭与其他玩家的碰撞检测
	this->schedule(schedule_selector(MultiGameScene::Dead, this));

	//星星和心的补充
	this->schedule(schedule_selector(SingleGameScene::SupplyHeartStar, this));

	//控制人物移动
	auto MoveListen = EventListenerTouchOneByOne::create();
	MoveListen->setSwallowTouches(true);
	MoveListen->onTouchBegan = CC_CALLBACK_2(SingleGameScene::MoveBegan, this);
	MoveListen->onTouchMoved = CC_CALLBACK_2(SingleGameScene::MoveMoved, this);
	MoveListen->onTouchEnded = CC_CALLBACK_2(SingleGameScene::MoveEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(MoveListen, this);
	this->schedule(schedule_selector(MultiGameScene::MovePerson, this));


	//控制射箭
	auto ArrowListen = EventListenerTouchOneByOne::create();
	ArrowListen->onTouchBegan = CC_CALLBACK_2(SingleGameScene::ArrowBegan, this);
	ArrowListen->onTouchMoved = CC_CALLBACK_2(SingleGameScene::ArrowMoved, this);
	ArrowListen->onTouchEnded = CC_CALLBACK_2(MultiGameScene::ArrowEnded, this);//重写-->完成
	_eventDispatcher->addEventListenerWithSceneGraphPriority(ArrowListen, this);
	this->schedule(schedule_selector(MultiGameScene::MoveArrow, this));//我方箭的移动


	//显示血量
	this->schedule(schedule_selector(MultiGameScene::ShowBlood, this));
	this->schedule(schedule_selector(SingleGameScene::ChangeWeapon, this));
	this->schedule(schedule_selector(MultiGameScene::test, this));
	return true;
}

//联网模块
void* MultiGameScene::postMessage(void*)
{

	Person* model = static_cast<Person*>(now->getChildByTag(ModelTag));
	while (true)
	{
		Vec2 NowPos = now->map->getPosition();
		if (NowPos == now->LastPos&&now->MyArrow.empty())
		{
			continue;
		}
		now->LastPos = NowPos;
		now->DataToString();
		now->sock_client->Send((char*)now->MessageToPost.c_str(), now->MessageToPost.length(), 0);
	}

	return NULL;
}

void* MultiGameScene::getMessage(void*)

{
	char buffer[1024];
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		now->sock_client->Recv(buffer, sizeof(buffer));
		now->strmsg = buffer;
		now->StringToData();
	}
	return NULL;
}
void MultiGameScene::StringToData()
{

	rapidjson::Document document;
	document.Parse<0>(now->strmsg.c_str());//将字符串加载进document中
	if (document.HasParseError()) { //打印解析错误
		log("jiexicuowu");
		return;
	}
	if (document.IsObject()) {
		if (!document.HasMember("Name")) {
			log("json parse error!");
			return;
		}

		string name = document["Name"].GetString();
		if (dead.count(name))//如果该信息是由已死亡的玩家发送过来的，不予处理
		{
			return;
		}
		if (name == RoleModel->name)//如果传回来的是自己的数据无需处理
		{
			return;
		}
		Person*NowPerson = NameToPerson.at(name);
		if (!NowPerson)//如果没有找到这个人，就把他创建出来
		{
			return;
		}
		NowPerson->blood = document["Blood"].GetInt();
		NowPerson->blue = document["Blue"].GetInt();
		float deltaX = document["deltaX"].GetFloat();
		float deltaY = document["deltaY"].GetFloat();
		Vec2 Pos = Vec2(map->getPositionX() + deltaX, map->getPositionY() + deltaY);
		NowPerson->setPosition(Pos);

		const rapidjson::Value& a = document["Arrow"];
		assert(a.IsArray());
		for (auto& v : a.GetArray())
		{
			string picture = v["picture"].GetString();
			auto arrow = Arrow::CreateArrow(picture);
			arrow->arrow_attack = v["attack"].GetInt();
			arrow->dir = v["dir"].GetDouble();
			arrow->speed = v["speed"].GetInt();
			arrow->range = v["range"].GetInt();
			arrow->StartPosition = Vec2(NowPerson->getPositionX() + height, NowPerson->getPositionY() + height);
			arrow->setPosition(Vec2(NowPerson->getPositionX() + height, NowPerson->getPositionY() + height));
			arrow->master = NowPerson;
			this->addChild(arrow);
			AllArrowList.push_back(arrow);
		}
	}
}
void MultiGameScene::DataToString()//数据转化成json字符串
{

	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	float deltaX = RoleModel->getPositionX() - map->getPositionX();
	float deltaY = RoleModel->getPositionY() - map->getPositionY();
	document.AddMember("Blood", RoleModel->blood, allocator);
	document.AddMember("Blue", RoleModel->blue, allocator);
	document.AddMember("deltaX", deltaX, allocator);
	document.AddMember("deltaY", deltaY, allocator);
	const string & name = RoleModel->name;
	rapidjson::Value val;
	document.AddMember("Name", val.SetString(name.c_str(), allocator), allocator);



	rapidjson::Value ArrayForArrow(rapidjson::kArrayType);
	for (list<Arrow*>::iterator it = MyArrow.begin(); it != MyArrow.end(); ++it)//将我方这段时间内射击信息上传
	{
		Arrow* NowArrow = *it;
		rapidjson::Value object(rapidjson::kObjectType);
		object.AddMember("attack", NowArrow->arrow_attack, allocator);
		object.AddMember("dir", NowArrow->dir, allocator);
		object.AddMember("speed", NowArrow->speed, allocator);
		object.AddMember("range", NowArrow->range, allocator);
		const string & pic = NowArrow->picture;
		rapidjson::Value val;
		object.AddMember("picture", val.SetString(pic.c_str(), allocator), allocator);
		ArrayForArrow.PushBack(object, allocator);
	}
	MyArrow.clear();
	document.AddMember("Arrow", ArrayForArrow, allocator);

	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writer(buffer);
	document.Accept(writer);
	MessageToPost = buffer.GetString();
	now->MessageToPost = buffer.GetString();
	now->MessageToPost += '\n';
}
#if false
void* MultiGameScene::AutoArrow(void* ptr)
{
	Arrow* NowArrow = static_cast<Arrow*>(ptr);

	while (NowArrow)
	{
		float x = NowArrow->getPositionX() + NowArrow->speed*cos(NowArrow->dir);
		float y = NowArrow->getPositionY() + NowArrow->speed*sin(NowArrow->dir);
		NowArrow->setPosition(Vec2(x, y));
		if (distance(NowArrow->getPosition(), NowArrow->StartPosition) > NowArrow->range)//判断出界
		{
			NowArrow->removeFromParent();
		}
		for (list<Person*>::iterator it = AllPersonList.begin(); it != AllPersonList.end(); ++it)//判断碰撞
		{
			Person* NowPerson = *it;
			Rect NowPerson_pos = Rect(NowPerson->getPositionX(), NowPerson->getPositionY(), 20 * 2, 20 * 2);
			Rect NowArrow_pos = Rect(NowArrow->getPositionX(), NowArrow->getPositionY(), NowArrow->arrow_size, NowArrow->arrow_size);
			if (NowPerson_pos.intersectsRect(NowArrow_pos))//箭射中人
			{

				if (NowArrow->master == NowPerson)continue;
				NowArrow->master->person_score++;
				NowPerson->blood -= NowArrow->arrow_attack*NowArrow->master->attack;//掉血
				NowArrow->removeFromParent();
			}

		}
		if (!NowArrow) {
			break;
		}
		Sleep(10);
	}
	return NULL;
}
#endif
void MultiGameScene::ShowBlood(float t)
{
	for (auto NowPerson : AllPersonList)
	{
		Sprite* now_blood = static_cast<Sprite*>(blood_hash.at(NowPerson));
		if (now_blood)
		{
			this->removeChild(now_blood);
		}
		Sprite* new_blood = nullptr;
		int blood = NowPerson->blood;
		if (blood <= 100 && blood >= 80)
		{
			new_blood = Sprite::create("blood_5.png");
		}
		else if (blood < 80 && blood >= 60)
		{
			new_blood = Sprite::create("blood_4.png");
		}
		else if (blood < 60 && blood >= 40)
		{
			new_blood = Sprite::create("blood_3.png");
		}
		else if (blood < 40 && blood >= 20)
		{
			new_blood = Sprite::create("blood_2.png");
		}
		else
		{
			new_blood = Sprite::create("blood_1.png");
		}
		new_blood->setPosition(Vec2(NowPerson->getPositionX(), NowPerson->getPositionY() + DY));
		blood_hash.insert(NowPerson, new_blood);
		this->addChild(new_blood);
	}
}
void MultiGameScene::InitAllPerson()//根据服务器广播的所有人的行列坐标设置所有人的初始位置
{
	auto person = Person::CreatePerson("monster.png");
	person->name = "JesPark";
	person->setPosition(RoleModel->getPosition());
	AllPersonList.push_back(person);
	NameToPerson.insert("JesPark", person);
	this->addChild(person);
}
void MultiGameScene::Shoot(float t)//模拟其余玩家的射箭
{

}
void MultiGameScene::MovePerson(float t)
{
	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	Vector<Sprite*>ToEraseStar, ToEraseHeart;
	float dir = Rocker::getRad(rockerBG_Position, current_point);
	if (rockerBG_Position == current_point)return;
	float dx = model->speed * cos(dir);
	float dy = model->speed * sin(dir);
	Vec2 NowPos = exchange(Vec2(model->getPositionX() + height, model->getPositionY() + height));//获得当前人物所处的tile坐标

	Vec2 next_pos = Vec2(model->getPositionX() + height * (cos(dir)), model->getPositionY() + height * (sin(dir)));//人物下一刻即将移动到的位置  将人物视为一个圆柱体
	for (auto NowStar : AllStar)
	{
		if (distance(NowStar->getPosition(), model->getPosition()) < 30)
		{
			ToEraseStar.pushBack(NowStar);
			model->blue += 5;
			LabelMagic->setString(String::createWithFormat("Magic:%d", model->blue)->_string);
			this->removeChild(NowStar);
		}
	}
	for (auto NowHeart : AllHeart)
	{
		if (distance(NowHeart->getPosition(), model->getPosition()) < 30)
		{
			ToEraseHeart.pushBack(NowHeart);
			model->blood += 5;
			if (model->blood > 100)model->blood = 100;
			this->removeChild(NowHeart);
		}
	}
	for (auto x : ToEraseStar)
	{
		AllStar.eraseObject(x);
	}
	for (auto x : ToEraseHeart)
	{
		AllHeart.eraseObject(x);
	}
	Vec2 next_pos_right = Vec2(model->getPositionX() + height, model->getPositionY());
	if (dx < 0)
	{
		next_pos_right = Vec2(model->getPositionX() - height, model->getPositionY());
	}
	Vec2 next_pos_up = Vec2(model->getPositionX(), model->getPositionY() + height);
	if (dy < 0)
	{
		next_pos_up = Vec2(model->getPositionX(), model->getPositionY() - height);
	}
	Vec2 SetPos = Vec2(0, 0);
	if (check(next_pos))//碰撞检测
	{
		SetPos = Vec2(-dx, -dy);
	}
	else if (check(next_pos_right))
	{
		SetPos = Vec2(-dx, 0);
	}
	else if (check(next_pos_up))
	{
		SetPos = Vec2(0, -dy);
	}
	//地图上其他精灵跟随移动
	map->setPosition(Vec2(map->getPositionX() + SetPos.x, map->getPositionY() + SetPos.y));
	for (auto NowPerson : AllPersonList)
	{
		if (NowPerson == model)continue;
		NowPerson->setPosition(Vec2(NowPerson->getPositionX() + SetPos.x, NowPerson->getPositionY() + SetPos.y));
	}
	for (auto NowStar : AllStar)
	{
		NowStar->setPosition(Vec2(NowStar->getPositionX() + SetPos.x, NowStar->getPositionY() + SetPos.y));
	}
	for (auto NowHeart : AllHeart)
	{
		NowHeart->setPosition(Vec2(NowHeart->getPositionX() + SetPos.x, NowHeart->getPositionY() + SetPos.y));
	}
}
void MultiGameScene::Dead(float t)//自己死，场景跳转，别人死，从排行榜上除名
{
	list<Person*>ToErase;

	for (auto NowPerson : AllPersonList)
	{
		if (NowPerson->blood <= 0)
		{
			auto Model = getChildByTag(ModelTag);
			if (NowPerson == Model)
			{
				log("Model is dead!!!");
			}
			auto blood = blood_hash.at(NowPerson);
			this->removeChild(blood);
			ToErase.push_back(NowPerson);
			NowPerson->removeFromParent();
		}
	}
	for (auto x : ToErase)
	{
		AllPersonList.remove(x);
		dead.insert(x->name);//将已死亡的玩家加入死亡名单
	}
}
void MultiGameScene::ArrowEnded(Touch* t, Event*e)//需要把射出的箭添加进MyArrow数组中
{
	if (start == t->getLocation())return;
	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	std::string picture = model->weapon->picture;
	//创建一只箭，用人物的weapon对箭进行赋值
	auto arrow = Arrow::CreateArrow(picture);
	arrow->arrow_attack = model->weapon->arrow_attack;
	arrow->speed = model->weapon->speed;
	arrow->range = model->weapon->range;
	arrow->arrow_size = model->weapon->arrow_size;
	arrow->setPosition(model->getPosition());
	arrow->StartPosition = model->getPosition() + Vec2(height, height);
	arrow->master = model;
	float NowDir = -model->getRotation();
	while (NowDir < -180)
	{
		NowDir += 360;
	}
	arrow->dir = NowDir * 3.1415926 / 180;
	AllArrowList.push_back(arrow);
	MyArrow.push_back(arrow);
	this->addChild(arrow);
}
void MultiGameScene::Hurt(float t)
{
	list<Arrow*>ToErase;
	Person* model = static_cast<Person*>(getChildByTag(ModelTag));
	for (list<Person*>::iterator it = AllPersonList.begin(); it != AllPersonList.end(); ++it)
	{
		Person* NowPerson = *it;
		Rect NowPerson_pos = Rect(NowPerson->getPositionX(), NowPerson->getPositionY(), height * 2, height * 2);
		for (list<Arrow*>::iterator it = AllArrowList.begin(); it != AllArrowList.end(); ++it)
		{
			Arrow* NowArrow = *it;
			Rect NowArrow_pos = Rect(NowArrow->getPositionX(), NowArrow->getPositionY(), NowArrow->arrow_size, NowArrow->arrow_size);
			if (NowPerson_pos.intersectsRect(NowArrow_pos))//箭射中人
			{

				if (NowArrow->master == NowPerson)continue;
				NowArrow->master->person_score++;
				if (NowArrow->master == model)
				{
					score++;
					LabelScore->setString(String::createWithFormat("Score:%d", score)->_string);
				}
				NowPerson->blood -= NowArrow->arrow_attack*NowArrow->master->attack;//掉血
				float dx = NowArrow->speed*cos(NowArrow->dir);//受击后产生位移
				float dy = NowArrow->speed*sin(NowArrow->dir);
				Vec2 next_pos = Vec2(NowPerson->getPositionX() + dx, NowPerson->getPositionY() + dy);
				if (check(next_pos))
				{
					if (NowPerson == model)
					{
						map->setPosition(map->getPositionX() - dx, map->getPositionY() - dy);
						for (auto person : AllPersonList)
						{
							if (person != model)
							{
								person->setPosition(person->getPositionX() - dx, person->getPositionY() - dy);
							}
						}
					}
					else NowPerson->setPosition(next_pos);
				}

				ToErase.push_back(NowArrow);
				NowArrow->removeFromParent();
			}
		}
	}
	for (auto x : ToErase)//删除箭
	{
		AllArrowList.remove(x);
	}
}
void MultiGameScene::MoveArrow(float t)
{
	list<Arrow*>ToErase;
	for (list<Arrow*>::iterator it = AllArrowList.begin(); it != AllArrowList.end(); it++)
	{
		Arrow* nowArrow = *it;
		double dis = distance(nowArrow->getPosition(), nowArrow->StartPosition);
		if (dis >= nowArrow->range)
		{
			this->removeChild(nowArrow);
			ToErase.push_back(nowArrow);
			continue;
		}
		float dx = nowArrow->speed*cos(nowArrow->dir);//移动箭
		float dy = nowArrow->speed*sin(nowArrow->dir);
		Vec2 next_pos = Vec2(nowArrow->getPositionX() + dx, nowArrow->getPositionY() + dy);
		if (!check(next_pos))//箭与墙壁的碰撞检测
		{
			this->removeChild(nowArrow);
			ToErase.push_back(nowArrow);
		}
		else
			nowArrow->setPosition(next_pos);
	}
	for (auto x : ToErase)
	{
		AllArrowList.remove(x);
	}
}
void MultiGameScene::test(float t)
{
	log("arrow number:%d", AllArrowList.size());
	for (auto x : AllArrowList)
	{
		log("%s", x->picture.c_str());
	}
}
Vec2 MultiGameScene::ToOpenGL(Vec2 pos)
{
	Vec2 anchor = map->getPosition();
	float y = anchor.y + 3000 - 30 * pos.x;
	float x = anchor.x + 30 * pos.y;
	return Vec2(x, y);
}


