#include "SingleGameScene.h"
#include "HelloWorldScene.h"
#include "TDpoint.h"
#include "Person.h"
#include "Arrow.h"
#include "Rocker.h"
#include <ctime>
#include<cstring>
USING_NS_CC;

bool SingleGameScene::init()
{
	if (!Layer::init())return false;

	//��ʼ��
	auto winsize = Director::getInstance()->getWinSize();
	this->InitValue();
	this->initWeapon();

	//���ص�ͼ
	this->map = TMXTiledMap::create("Tank2.tmx");
	map->setTag(MapTag);
	map->setPosition(Vec2(224,-2074));
	this->addChild(map,-1);
	GetPos();
	InitMap();
	//�����ϰ���ͱ�����
	stop = map->getLayer("stop");
	stop->retain();
	background = map->getLayer("background");
	background->retain();
	star = map->getLayer("star");
	star->retain();

	//����Ļ�����봴������
	auto model = Person::CreatePerson("person.png");
	model->setTag(ModelTag);
	model->blood = 100;
	this->addChild(model);
	model->setPosition(Vec2(winsize.width / 2, winsize.height / 2));
	AllPerson.pushBack(model);


	//��������
	InitMonster();


	//����ҡ��
	auto rocker = Rocker::create("CloseSelected.png","rr.png", rockerBG_Position);
	rocker->setTag(RockerTag);
	this->addChild(rocker);
	rocker->StartRocker();


	//ʵ�ֹ��ﲹ�䣬�ƶ����������ܻ���⣬����
	this->schedule(schedule_selector(SingleGameScene::CreateMonster), 1);
	this->schedule(schedule_selector(SingleGameScene::MoveDirect, this));
	this->schedule(schedule_selector(SingleGameScene::MoveAllPerson, this));
	this->schedule(schedule_selector(SingleGameScene::Shoot), 3);
	this->schedule(schedule_selector(SingleGameScene::MoveArrow, this));
	this->schedule(schedule_selector(SingleGameScene::Hurt, this));
	this->schedule(schedule_selector(SingleGameScene::Dead, this));


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


	//��ʾѪ��
	this->schedule(schedule_selector(SingleGameScene::ShowBlood, this));
	this->schedule(schedule_selector(SingleGameScene::ChangeWeapon, this));
	//this->schedule(schedule_selector(SingleGameScene::ChangeWeapon), 1);
	

	return true;
}

//���ܺ���
Vec2 SingleGameScene::exchange(Vec2 pos)
{

	Vec2 anchor = map->getPosition();//�õ��������½ǵ����� 
	int height = 3000;
	float delta_y = height + anchor.y - pos.y;
	float delta_x = pos.x - anchor.x;
	int row = delta_y / 30;
	int col = delta_x /30;

	return Vec2(row, col);
}
bool SingleGameScene::check(Vec2 pos)
{
	Vec2 tile_pos = exchange(pos);
	return !StopCheck.count(tile_pos);
}
void SingleGameScene::GetPos()
{
	Three = map->getPosition();
	Three.x += 200, Three.y +=200;
	Two = Three + Vec2(0, 2400);
	One = Two + Vec2(2400, 0);
	Four = Three + Vec2(2400,0);
}
inline double SingleGameScene::distance(Vec2 pos1, Vec2 pos2)
{
	double ans = (pos1.x - pos2.x)*(pos1.x - pos2.x) + (pos1.y - pos2.y)*(pos1.y - pos2.y);
	return sqrt(ans);
}
void SingleGameScene::ShowBlood(float t)
{
	


	for (auto NowPerson : AllPerson)
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
		else if (blood < 60 && blood>=40)
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
void SingleGameScene::ChangeWeapon(float t)
{
	srand(time(NULL));
	Person* model = static_cast<Person*>(getChildByTag(ModelTag));
	if (model->blue < 100)
	{
		//model->blue += 5;
		//log("blue:%d", model->blue);
		return;
	}
	Vector<MenuItem*>v;
	for (int i=1; i <= 3; i++)
	{

		int num = random(0, AllWeapon.size() );
		Vector<Arrow*>::iterator it = AllWeapon.begin();
		while (num--)
		{
			it++;
		}
		auto menu = MenuItemImage::create((*it)->picture, (*it)->picture, CC_CALLBACK_1(SingleGameScene::MenuCallBack, this));
		if (i == 1)
		{
			menu->setPosition(Vec2(model->getPositionX() - 600, model->getPositionY() - 350));
			menu->setTag(1);
			ArrowCopy(NowWeapon_1, *it);
		}
		else if (i == 2)
		{
			menu->setPosition(Vec2(model->getPositionX()-500, model->getPositionY() - 350));
			menu->setTag(2);
			ArrowCopy(NowWeapon_2, *it);
		}
		else
		{
			menu->setPosition(Vec2(model->getPositionX() -400, model->getPositionY() - 350));
			menu->setTag(3);
			ArrowCopy(NowWeapon_3, *it);
		}
		v.pushBack(menu);
	}
	
	auto M = Menu::createWithArray(v);
	M->setTag(MenuTag);
	this->addChild(M);
	model->blue = 0;

}
void SingleGameScene::ArrowCopy(Arrow* first,Arrow* second)
{
	first->speed = second->speed;
	first->range = second->range;
	first->arrow_attack = second->arrow_attack;
	first->picture = second->picture;
}
inline int SingleGameScene::random(int a, int b)
{
	return (((double)rand() / RAND_MAX) * (b - a) + a);
}



//�ƶ�ģ��

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
	Vec2 NowPos = exchange(Vec2(model->getPositionX()+height,model->getPositionY()+height));//��õ�ǰ����������tile����
#if 0
	int gid = star->getTileGIDAt(NowPos);
	if (star->getTileGIDAt(NowPos) == is_blue&&!se.count(NowPos))
	{
		model->blue += 5;
		Sprite* tile = star->getTileAt(NowPos);
		tile->setVisible(false);
		se.insert(NowPos);
		log("%d", model->blue);
	}
	if (star->getTileGIDAt(NowPos) == is_blood && !se.count(NowPos))
	{
		model->blood += 5;
		if (model->blood > 100)
		{
			model->blood = 100;
		}
		Sprite* tile = star->getTileAt(NowPos);
		tile->setVisible(false);
		se.insert(NowPos);
		log("%d", model->blood);
	}
#endif
	Vec2 next_pos = Vec2(model->getPositionX() +height*(cos(dir)), model->getPositionY() +height*(sin(dir)));//������һ�̼����ƶ�����λ��  ��������Ϊһ��Բ����

	Vec2 next_pos_right= Vec2(model->getPositionX() + height , model->getPositionY()  ); 
	if (dx < 0)
	{
		next_pos_right = Vec2(model->getPositionX() -height, model->getPositionY() );
	}
	Vec2 next_pos_up= Vec2(model->getPositionX() , model->getPositionY() + height );
	if (dy < 0)
	{
		next_pos_up = Vec2(model->getPositionX(), model->getPositionY() -height);
	}
	
	if (check(next_pos))//��ײ���
	{
		map->setPosition(Vec2(map->getPositionX() - dx, map->getPositionY() - dy));
		for (auto NowPerson : AllPerson)
		{
			if (NowPerson == model)continue;
			NowPerson->setPosition(Vec2(NowPerson->getPositionX() - dx, NowPerson->getPositionY() - dy));
		}
	}
	else if(check(next_pos_right))
	{
		map->setPositionX(map->getPositionX() - dx);
		for (auto NowPerson : AllPerson)
		{
			if (NowPerson == model)continue;
			NowPerson->setPositionX(NowPerson->getPositionX() - dx);
		}
	}
	else if (check(next_pos_up))
	{
		map->setPositionY(map->getPositionY() - dy);
		for (auto NowPerson : AllPerson)
		{
			if (NowPerson == model)continue;
			NowPerson->setPositionY(NowPerson->getPositionY() - dy);
		}
	}


}


//���ģ��

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

	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	Vec2 NowPos = t->getLocation();
	Vec2 ModelPos = model->getPosition();
	float dir = Rocker::getRad(ModelPos, NowPos);
	dir = dir * 180 / 3.1415926;
	model->setRotation(-dir);

}
void SingleGameScene::ArrowEnded(Touch*t, Event*e)
{

	if (start == t->getLocation())return;
	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	std::string picture = model->weapon->picture;
	//����һֻ�����������weapon�Լ����и�ֵ
	auto arrow = Arrow::CreateArrow(picture);
	arrow->arrow_attack = model->weapon->arrow_attack;
	arrow->speed = model->weapon->speed;
	arrow->range = model->weapon->range;
	arrow->arrow_size = model->weapon->arrow_size;
	arrow->setPosition(model->getPosition());
	arrow->StartPosition = model->getPosition()+Vec2(height,height);
	arrow->master = model;
	float NowDir = -model->getRotation();
	while (NowDir < -180)
	{
		NowDir += 360;
	}
	arrow->dir = NowDir*3.1415926/180;
	AllArrow.pushBack(arrow);
	this->addChild(arrow);
}
void SingleGameScene::MoveArrow(float t)
{
	Vector<Arrow*>ToErase;
	for (auto nowArrow : AllArrow)
	{
		double dis = distance(nowArrow->getPosition(), nowArrow->StartPosition);
		if (dis >= nowArrow->range)
		{
			this->removeChild(nowArrow);
			ToErase.pushBack(nowArrow);
			continue;
		}
		float dx = nowArrow->speed*cos(nowArrow->dir);//�ƶ���
		float dy = nowArrow->speed*sin(nowArrow->dir);
		Vec2 next_pos = Vec2(nowArrow->getPositionX() + dx, nowArrow->getPositionY() + dy);
		if (!check(next_pos))//����ǽ�ڵ���ײ���
		{
			this->removeChild(nowArrow);
			ToErase.pushBack(nowArrow);
		}
		else
		nowArrow->setPosition(next_pos);
	}
	for (auto x : ToErase)
	{
		AllArrow.eraseObject(x);
	}
}


//AIģ��
void SingleGameScene::InitMonster()
{
	std::vector<Vec2>v;
	v.push_back(One);
	v.push_back(Two);
	v.push_back(Three);
	v.push_back(Four);
	for (auto x : v)
	{
		auto monster = Person::CreatePerson("monster.png");
		monster->setPosition(x);
		this->addChild(monster);
		AllPerson.pushBack(monster);
	}
}
void SingleGameScene::CreateMonster(float t)
{
	if (AllPerson.size() > MonsterNumber)
	{
		return;
	}
	GetPos();
	Person* model = static_cast<Person*>(getChildByTag(ModelTag));
	auto monster = Person::CreatePerson("monster.png");
	Vec2 pos;
	int maxv = 0;
	std::vector<Vec2>v;
	v.push_back(One);
	v.push_back(Two);
	v.push_back(Three);
	v.push_back(Four);
	for (auto x : v)
	{
		auto dis = distance(x, model->getPosition());
		if (dis > maxv)
		{
			maxv = dis;
			pos = x;
		}
	}
	monster->setPosition(pos);
	AllPerson.pushBack(monster);
	this->addChild(monster);
	
}
void SingleGameScene::MoveDirect(float t)
{
	
	auto model = getChildByTag(ModelTag);
	for (auto first:AllPerson)//���ｫѡ�����Լ�����Ͻ���
	{
	
		if (first == model)continue;
		bool flag = false;
		for (auto target : AllPerson)//������ԣ��γ������Կ��ľ���
		{
			if (target != first && hash_table.at(target) == first)
			{
				hash_table.insert(first, target);
				flag = true;
			}
		}
		double minv = 10000000000.0;
		if (!flag)
		{
			for (auto second : AllPerson)
			{
				if (first != second)
				{
					double dis = this->distance(first->getPosition(), second->getPosition());
					if (dis< minv)
					{
						hash_table.insert(first, second);//�ҵ�������С�ĵ㣬���Ҹ���С����С��attack_area
						minv = dis;
					}
				}
			}
			if (minv < attack_area)
			{
				flag = true;
			}
			flag = true;
		}
		if (!flag)
		{
			hash_table.insert(first, NullPerson);
		}
	}
	
}
void SingleGameScene::Shoot(float t)
{
	auto model = getChildByTag(ModelTag);
	for (auto NowPerson : AllPerson)
	{
		if (NowPerson == model)continue;
		if (hash_table.at(NowPerson) != NullPerson)
		{
			auto target = hash_table.at(NowPerson);
			auto arrow = Arrow::CreateArrow("CloseNormal.png");
			arrow->setPosition(NowPerson->getPosition());
			arrow->StartPosition = NowPerson->getPosition();
			arrow->master = NowPerson;
			float dir = Rocker::getRad(NowPerson->getPosition(), target->getPosition());
			arrow->dir = dir;
			AllArrow.pushBack(arrow);
			this->addChild(arrow);
		}
	}
}
void SingleGameScene::MoveAllPerson(float t)
{
	srand(time(NULL));
	auto model = getChildByTag(ModelTag);
	int dx[4] = { -1,0,1,0 };
	int dy[4] = { 0,1,0,-1 };
	for (auto NowPerson:AllPerson)
	{
		if (NowPerson == model)continue;
		if (hash_table.at(NowPerson)!=NullPerson)
		{
			auto direct = hash_table.at(NowPerson);
			if(NowPerson->getPosition()==direct->getPosition())
			{
				log("debug");
			}
			float dir = Rocker::getRad(NowPerson->getPosition(), direct->getPosition());
			float dx = NowPerson->speed * cos(dir);
			float dy = NowPerson->speed * sin(dir);
			Vec2 next_pos = Vec2(NowPerson->getPositionX() + height*cos(dir), NowPerson->getPositionY() + height*sin(dir));
			auto dis = distance(NowPerson->getPosition(), direct->getPosition());
			if (dis < min_attack_area)
			{
				next_pos= Vec2(NowPerson->getPositionX() - height*cos(dir), NowPerson->getPositionY() - height*sin(dir));
				dx = -dx, dy = -dy;
			}
			if (check(next_pos))
			{
				NowPerson->setPosition(Vec2(NowPerson->getPositionX()+dx,NowPerson->getPositionY()+dy));
			}
/*
			else
			{
				if (!check(NowPerson->getPosition()))
				{
					for (int dir = 0; dir < 4; dir++)
					{
						next_pos = Vec2(NowPerson->getPositionX() + dx[dir], NowPerson->getPositionY() + dy[dir]);
						if (check(next_pos))
						{
							NowPerson->setPosition(next_pos);
							break;
						}
					}
				}
			}
*/
		//}
		//else//���value��NullPerson������ƶ�
		//{
/*
			for (int dir = 0; dir < 4; dir++)
			{
				Vec2 next_pos = Vec2(NowPerson->getPositionX() + dx[dir], NowPerson->getPositionY() + dy[dir]);
				if (check(next_pos))
				{
					NowPerson->setPosition(next_pos);
					break;
				}
			}
*/

			/*
			int dir = random(0, 3);
			Vec2 next_pos = Vec2(NowPerson->getPositionX() + NowPerson->speed*dx[dir], NowPerson->getPositionY() + NowPerson->speed*dy[dir]);
			if (check(next_pos))
			{
				NowPerson->setPosition(next_pos);
			}
			*/
		}
	}
}
void SingleGameScene::Hurt(float t)
{
	Vector<Arrow*>ToErase;
	Person* model = static_cast<Person*>(getChildByTag(ModelTag));
	for (auto NowPerson : AllPerson)
	{
		Rect NowPerson_pos = Rect(NowPerson->getPositionX(), NowPerson->getPositionY(), height * 2, height * 2);
		for (auto NowArrow : AllArrow)
		{
			Rect NowArrow_pos = Rect(NowArrow->getPositionX(), NowArrow->getPositionY(), NowArrow->arrow_size, NowArrow->arrow_size);
			if (NowPerson_pos.intersectsRect(NowArrow_pos))//��������
			{
				//log("%d",model->blood);
			
				if (NowArrow->master == NowPerson)continue;
				NowPerson->blood -= NowArrow->arrow_attack*NowArrow->master->attack;//��Ѫ
				float dx = NowArrow->speed*cos(NowArrow->dir);//�ܻ������λ��
				float dy = NowArrow->speed*sin(NowArrow->dir);
				Vec2 next_pos = Vec2(NowPerson->getPositionX() + dx, NowPerson->getPositionY() + dy);
				if (check(next_pos))
				{
					if (NowPerson == model)
					{
						map->setPosition(map->getPositionX() - dx, map->getPositionY() - dy);
						for (auto person : AllPerson)
						{
							if (person != model)
							{
								person->setPosition(person->getPositionX() - dx, person->getPositionY() - dy);
							}
						}
					}
					else NowPerson->setPosition(next_pos);
				}
				
				ToErase.pushBack(NowArrow);
				NowArrow->removeFromParent();
			}
		}
	}
	for (auto x : ToErase)//ɾ����
	{
		AllArrow.eraseObject(x);
	}
}
void SingleGameScene::Dead(float t)
{
	Vector<Person*>ToErase;
	
	for (auto NowPerson : AllPerson)
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
			ToErase.pushBack(NowPerson);
			NowPerson->removeFromParent();
		}
	}
	for (auto x : ToErase)
	{
		AllPerson.eraseObject(x);
		auto res = hash_table.find(x);
		if (res != hash_table.cend())
		{
			hash_table.erase(hash_table.find(x));//��key��x�Ĺ�ϣɾ��
		}
		
	}
}

//��Ʒģ��

void SingleGameScene::MenuCallBack(cocos2d::Ref* pSender)
{

	auto NowItem = static_cast<MenuItem*>(pSender);//�õ���ǰ����Ĳ˵���
	int tag = NowItem->getTag();
	Person* model = static_cast<Person*>(getChildByTag(ModelTag));
	switch (tag)
	{
	case 1:
		ArrowCopy(model->weapon, NowWeapon_1);
		break;
	case 2:
		ArrowCopy(model->weapon, NowWeapon_2);
		break;
	case 3:
		ArrowCopy(model->weapon, NowWeapon_3);
		break;
	}
	Menu * M = static_cast<Menu*> (getChildByTag(MenuTag));
	M->removeFromParent();
	log("debug");

}

//��ʼ��ģ��
Scene* SingleGameScene::CreateScene()
{
	auto scene = Scene::create();
	auto layer = SingleGameScene::create();
	scene->addChild(layer);
	return scene;
}
void SingleGameScene::InitAllPoint(TMXTiledMap*map)
{
	Vec2 map_pos = map->getPosition();
	int height = map->getTileSize().height*map->getMapSize().height;
	int width = map->getTileSize().width*map->getMapSize().width;
	//test
	ValueVector value;
	value = map->getObjectGroup("obj")->getObjects();//�õ����������
	for (int i = 0; i < value.size(); i++)
	{
		ValueMap val = value.at(i).asValueMap();//��ȡ��i����
		TDpoint *newp = TDpoint::createPoint(val.at("x").asInt(), val.at("y").asInt());
		allpoint.pushBack(newp);
	}

	
}
void SingleGameScene::InitValue()
{
	this->rockerBG_Position = Vec2(100, 100);
	this->current_point = rockerBG_Position;
	this->MonsterNumber = 5;
	height = 20;
	attack_area = 500;
	min_attack_area = 250;
	counts = 0;
	NullPerson = Person::CreatePerson("person.png");
	NullPerson->retain();
	NowWeapon_1 = Arrow::CreateArrow("CloseNormal.png");
	NowWeapon_1->retain();
	NowWeapon_2 = Arrow::CreateArrow("CloseNormal.png");
	NowWeapon_2->retain();
	NowWeapon_3 = Arrow::CreateArrow("CloseNormal.png");
	NowWeapon_3->retain();
}
void SingleGameScene::initWeapon()
{
	//����
	auto ax = Arrow::CreateArrow("ax.png");
	ax->arrow_attack = 20;
	ax->speed = 8;
	ax->range = 200;
	ax->picture = "ax.png";
	ax->retain();
	AllWeapon.pushBack(ax);

	//����
	auto fire = Arrow::CreateArrow("fire.png");
	fire->arrow_attack = 15;
	fire->speed = 20;
	fire->range = 200;
	fire->picture = "fire.png";
	fire->retain();
	AllWeapon.pushBack(fire);

	//����
	auto laser = Arrow::CreateArrow("laser.png");
	laser->arrow_attack = 30;
	laser->speed = 5;
	laser->range = 50;
	laser->picture = "laser.png";
	laser->retain();
	AllWeapon.pushBack(laser);

}
void SingleGameScene::InitMap()
{
	for (int i = 3; i <= 90; i++)
	{
		StopCheck.insert(Vec2(3, i));
	}
	for (int i = 3; i <= 96; i++)
	{
		StopCheck.insert(Vec2(i, 3));
	}
	for (int i = 3; i <= 96; i++)
	{
		StopCheck.insert(Vec2(96, i));
	}
	for (int i = 3; i <= 96; i++)
	{
		StopCheck.insert(Vec2(i, 90));
	}
	for (int i = 20; i <= 70; i++)
	{
		StopCheck.insert(Vec2(85, i));
	}
	for (int i = 80; i <= 90; i++)
	{
		StopCheck.insert(Vec2(i, 32));
		StopCheck.insert(Vec2(i, 63));
	}
}