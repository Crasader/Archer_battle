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
	//���ص�ͼ
	this->map = TMXTiledMap::create("Tank2.tmx");
	map->setTag(MapTag);
	//map->setPosition(Vec2(winsize.width / 2, winsize.height / 2));
	this->addChild(map,-1);
	//�������ж���
	InitAllPoint(map); 
	//�����ϰ���
	stop = map->getLayer("stop");

	//����Ļ�����봴������
	Person* model = Person::create("person.png");
	model->setTag(ModelTag);
	this->addChild(model);
	model->setPosition(Vec2(winsize.width / 2, winsize.height / 2));

	//����ҡ��
	auto rocker = Rocker::create("CloseSelected.png","rr.png", rockerBG_Position);
	rocker->setTag(RockerTag);
	this->addChild(rocker);
	rocker->StartRocker();

	//���������ƶ�
	auto MoveListen = EventListenerTouchOneByOne::create();
	MoveListen->setSwallowTouches(true);
	MoveListen->onTouchBegan = CC_CALLBACK_2(SingleGameScene::MoveBegan, this);
	MoveListen->onTouchMoved = CC_CALLBACK_2(SingleGameScene::MoveMoved, this);
	MoveListen->onTouchEnded = CC_CALLBACK_2(SingleGameScene::MoveEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(MoveListen, this);
	this->schedule(schedule_selector(SingleGameScene::MovePerson, this));


	//�������
	auto ArrowListen= EventListenerTouchOneByOne::create();
	ArrowListen->onTouchBegan = CC_CALLBACK_2(SingleGameScene::ArrowBegan, this);
	ArrowListen->onTouchMoved = CC_CALLBACK_2(SingleGameScene::ArrowMoved, this);
	ArrowListen->onTouchEnded = CC_CALLBACK_2(SingleGameScene::ArrowEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(ArrowListen, this);
	this->schedule(schedule_selector(SingleGameScene::MoveArrow, this));

	return true;
}

Vec2 SingleGameScene::exchange(Vec2 pos)
{
	Vec2 res;
	Vec2 anchor = map->getPosition();//�õ��������½ǵ����� woc���������ĵ������
	int anchor_y = map->getMapSize().height ;
	int anchor_x = 0;
	int dir_x = (pos.x - anchor.x)/map->getTileSize().width;
	int dir_y = (anchor.y - pos.y)/map->getTileSize().height;
	res.y = anchor_y + dir_y;
	res.x = anchor_x + dir_x;
	return res;
}

bool SingleGameScene::MoveBegan(Touch* t, Event* e)//�����ƶ�
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
	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	float dir = Rocker::getRad(rockerBG_Position, current_point);
	if (rockerBG_Position == current_point)return;
	float dx = model->speed * cos(dir);
	float dy = model->speed * sin(dir);
	Vec2 next_pos = Vec2(model->getPositionX() + dx, model->getPositionY() + dy);//������һ�̼����ƶ�����λ��

	Vec2 tile_next_pos = SingleGameScene::exchange(next_pos);//��һ��λ�õ���Ƭ��ͼ�����ʽ
	int gid = stop->getTileGIDAt(tile_next_pos);
	AllGid.push_back(gid);
	movement.push_back(p(std::make_pair(tile_next_pos.x, tile_next_pos.y), gid));
	if (AllGid.size() == 100)
	{
		log("DEBUG");
	}
	if (gid != is_stop)//��ײ���
	{
		map->setPosition(Vec2(map->getPositionX() - dx, map->getPositionY() - dy));
	}
	
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
void SingleGameScene::ArrowMoved(Touch* t, Event*e)//���
{
	
	
	//����ӣ�����׼ʱ����׼��
		
}
void SingleGameScene::ArrowEnded(Touch*t, Event*e)
{

	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	auto arrow = Arrow::create("CloseNormal.png");
	arrow->setPosition(model->getPosition());
	float dir = Rocker::getRad(start, t->getLocation());
	arrow->dir = dir;
	AllArrow.pushBack(arrow);
}
void SingleGameScene::MoveArrow(float t)
{
	//����ӣ����Լ�����ײ��⣬�Լ����١�ע�⣺�����ٵ�ʱ�����Ҫ������AllArrow��vector���Ƴ���Ҳ��Ҫ�������������ص�se��ɾ��
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
	value = map->getObjectGroup("obj")->getObjects();//�õ����������
	for (int i = 0; i < value.size(); i++)
	{
		ValueMap val = value.at(i).asValueMap();//��ȡ��i����
		TDpoint *newp = TDpoint::createPoint(val.at("x").asInt(), val.at("y").asInt());
		allpoint.pushBack(newp);
	}
}

