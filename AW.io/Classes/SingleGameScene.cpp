#include "SingleGameScene.h"
#include "HelloWorldScene.h"
#include "TDpoint.h"
#include "Person.h"
#include "Arrow.h"
#include "Rocker.h"
USING_NS_CC;
bool SingleGameScene::init()
{
	if (!Layer::init())return false;
	auto winsize = Director::getInstance()->getWinSize();
	this->rockerBG_Position = Vec2(100, 100);
	this->current_point = rockerBG_Position;
	//加载地图
	TMXTiledMap* map = TMXTiledMap::create("Tank2.tmx");
	map->setTag(MapTag);
	map->setPosition(Vec2(winsize.width / 2, winsize.height / 2));
	this->addChild(map,-1);
	//加载所有对象
	InitAllPoint(map);

	//在屏幕正中央创建主角
	Person* model = Person::create("person.png");
	model->setTag(ModelTag);
	this->addChild(model);
	model->setPosition(Vec2(winsize.width / 2, winsize.height / 2));

	//设置摇杆
	auto rocker = Rocker::create("CloseSelected.png","rr.png", rockerBG_Position);
	rocker->setTag(RockerTag);
	this->addChild(rocker);
	rocker->StartRocker();

	//控制人物移动
	auto MoveListen = EventListenerTouchOneByOne::create();
	MoveListen->onTouchBegan = CC_CALLBACK_2(SingleGameScene::MoveBegan, this);
	MoveListen->onTouchMoved = CC_CALLBACK_2(SingleGameScene::MoveMoved, this);
	MoveListen->onTouchEnded = CC_CALLBACK_2(SingleGameScene::MoveEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(MoveListen, this);
	this->schedule(schedule_selector(SingleGameScene::MovePerson, this));


	//控制射箭
	auto ArrowListen= EventListenerTouchOneByOne::create();
	ArrowListen->onTouchBegan = CC_CALLBACK_2(SingleGameScene::ArrowBegan, this);
	ArrowListen->onTouchMoved = CC_CALLBACK_2(SingleGameScene::ArrowMoved, this);
	ArrowListen->onTouchEnded = CC_CALLBACK_2(SingleGameScene::ArrowEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(ArrowListen, this);
	this->schedule(schedule_selector(SingleGameScene::MoveArrow, this));

	return true;
}
bool SingleGameScene::MoveBegan(Touch* t, Event* e)//人物移动
{
	auto rocker = this->getChildByTag(RockerTag);
	Sprite* rockerBG = static_cast<Sprite*>(rocker->getChildByTag(tag_rockerBG));
	Vec2 point = t->getLocation();
	if(rockerBG->getBoundingBox().containsPoint(point))
	return true;
	else return false;
}
void SingleGameScene::MoveMoved(Touch* t, Event* e)
{
	Vec2 point = t->getLocation();
	this->current_point = point;
	return;
}
void SingleGameScene::MoveEnded(Touch* t, Event *e)
{
	this->current_point = rockerBG_Position;
	return;
}
void SingleGameScene::MovePerson(float t)
{
	TMXTiledMap* map = static_cast<TMXTiledMap*>(this->getChildByTag(MapTag));
	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	float dir = Rocker::getRad(rockerBG_Position, current_point);
	if (rockerBG_Position == current_point)return;
	float dx = model->speed * cos(dir);
	float dy = model->speed * sin(dir);
	map->setPosition(Vec2(map->getPositionX() - dx, map->getPositionY() - dy));
}

bool SingleGameScene::ArrowBegan(Touch* t, Event *e)
{
	
	auto rocker = this->getChildByTag(RockerTag);
	Sprite* rockerBG = static_cast<Sprite*>(rocker->getChildByTag(tag_rockerBG));
	Vec2 point = t->getLocation();
	if (!rockerBG->getBoundingBox().containsPoint(point))
	{
		Vec2 point = t->getLocation();
		start = point;
		return true;
	}
		
	else return false;
}
void SingleGameScene::ArrowMoved(Touch* t, Event*e)//射箭
{
	
	
	//待添加：：瞄准时的瞄准线
		
}
void SingleGameScene::ArrowEnded(Touch*t, Event*e)
{

	TMXTiledMap* map = static_cast<TMXTiledMap*>(this->getChildByTag(MapTag));
	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	auto arrow = Arrow::create("CloseNormal.png");
	arrow->setPosition(model->getPosition());
	float dir = Rocker::getRad(start, t->getLocation());
	arrow->dir = dir;
	AllArrow.pushBack(arrow);
}
void SingleGameScene::MoveArrow(float t)
{
	//待添加：：对箭的碰撞检测，以及销毁。注意：：销毁的时候除了要将它从AllArrow的vector中移除，也需要将他从用来判重的se中删除
	for (auto nowArrow : AllArrow)
	{
		if (!se.count(nowArrow))
		{
			this->addChild(nowArrow);
			se.insert(nowArrow);
		}
		float dx = nowArrow->speed*cos(nowArrow->dir);
		float dy = nowArrow->speed*sin(nowArrow->dir);
		nowArrow->setPosition(Vec2(nowArrow->getPositionX() + dx, nowArrow->getPositionY() + dy));
#if 0
		for (auto x : allpoint)
		{
			x->
		}
#endif
	}
}
Scene* SingleGameScene::CreateScene()
{
	auto scene = Scene::create();
	auto layer = SingleGameScene::create();
	scene->addChild(layer);
	return scene;
}
void SingleGameScene::InitAllPoint(TMXTiledMap*map)
{
	ValueVector value;
	value = map->getObjectGroup("obj")->getObjects();//得到对象层数据
	for (int i = 0; i < value.size(); i++)
	{
		ValueMap val = value.at(i).asValueMap();//获取第i个点
		TDpoint *newp = TDpoint::createPoint(val.at("x").asInt(), val.at("y").asInt());
		allpoint.pushBack(newp);
	}
}

