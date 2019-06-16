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
#include "ui/CocosGUI.h"
#include "GameOverScene.h"
#pragma comment(lib, "pthreadVC2.lib")
using namespace  rapidjson;
using namespace std;
using namespace cocos2d::ui;
USING_NS_CC;
static MultiGameScene* now;

//��ʼ��ģ��
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

	//��ʼ��
	auto winsize = Director::getInstance()->getWinSize();
	this->InitValue();
	this->initWeapon();
	this->LastPos = Vec2(0, 0);
	this->LastArrow = NULL;
	this->map = TMXTiledMap::create("map.tmx");
	float x = static_cast<float>(random(-1000, -2000));
	float y = static_cast<float>(random(-1000, -2000));
	map->setPosition(Vec2(x, y));
	this->addChild(map, -1);
	GetPos();
	InitMap();
	is_chatting = false;

	//��������
	auto model = Person::CreatePerson("person.png");
	model->setTag(ModelTag);
	this->addChild(model);
	model->setPosition(Vec2(winsize.width / 2, winsize.height / 2));
	AllPersonList.push_back(model);
	RoleModel = model;

	//�ǳƱ༭��-->��������land��ʵ��
/*
	textField = cocos2d::ui::TextField::create("input you name", "Arial", 30);
	textField->setMaxLengthEnabled(true);
	textField->setMaxLength(8);
	textField->setPosition(Vec2(480,400));
	textField->addEventListener(CC_CALLBACK_2(MultiGameScene::textFieldEvent, this));
	textField->retain();
	this->addChild(textField);
*/

	//����༭��
	ChatField=cocos2d::ui::TextField::create("input", "Arial", 30);
	ChatField->setMaxLength(25);
	ChatField->setMaxLength(true);
	ChatField->setPosition(Vec2(480, 100));
	ChatField->addEventListener(CC_CALLBACK_2(MultiGameScene::chat, this));
	ChatField->setVisible(false);
	ChatField->setZOrder(150);
	this->addChild(ChatField);

	//�����ҷ��ذ�ť
	back = Button::create("back.png", "back.png", "back.png");
	back->setPosition(Vec2(900, 100));
	back->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
			switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			ShowChat(true);
			ChatField->setVisible(false);
			Sprite* bg = static_cast<Sprite*>(getChildByTag(133));
			bg->removeFromParent();
			back->setVisible(false);
			button->setVisible(true);
			is_chatting = false;
		}
	});
	back->setVisible(false);
	back->setZOrder(150);
	this->addChild(back);
	//����������水ť
	
	button = Button::create("chat.png", "chat.png", "chat.png");
	button->setPosition(Vec2(900, 400));
	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			//չʾ��ʷ�����¼
			ShowChat(false);
			//��ӱ���ͼƬ
			auto bg = Sprite::create("background_edit.png");
			bg->setPosition(Vec2(480, 240));
			bg->setZOrder(100);
			this->addChild(bg);
			bg->setTag(133);
			//��ӷ��ذ�ť
			back->setVisible(true);
			//����ı��༭��
			ChatField->setVisible(true);
			//��������
			button->setVisible(false);
			//��������״̬
			is_chatting = true;
			break;
		
		}
	});
	this->addChild(button);
	
	//����Label
	for (int i = 1; i <= 7; i++)
	{
		auto label = Label::createWithTTF("", "fonts/Marker Felt.ttf", 32);
		label->setPosition(Vec2(480, 100+i*50));
		label->setColor(Color3B::RED);
		label->setZOrder(150);
		this->addChild(label);
		AllLabel.push_back(label);
	}
	
	//ͨ��
	now = this;
	this->schedule(schedule_selector(MultiGameScene::postMessage, this));
	this->schedule(schedule_selector(MultiGameScene::getMessage, this));
	


	//�����������
	//InitAllPerson();

	//�������Ǻ���
	InitHeartStar();

	//����ҡ�� 
	auto rocker = Rocker::create("button.png", "RockerBG.png", rockerBG_Position,Vec2(130,130));
	rocker->setTag(RockerTag);
	this->addChild(rocker);
	rocker->StartRocker();

	//��Ϸ�߼�


	this->schedule(schedule_selector(MultiGameScene::Dead, this));

	//���Ǻ��ĵĲ���
	this->schedule(schedule_selector(SingleGameScene::SupplyHeartStar, this));

	//���������ƶ�
	auto MoveListen = EventListenerTouchOneByOne::create();
	MoveListen->setSwallowTouches(true);
	MoveListen->onTouchBegan = CC_CALLBACK_2(SingleGameScene::MoveBegan, this);
	MoveListen->onTouchMoved = CC_CALLBACK_2(SingleGameScene::MoveMoved, this);
	MoveListen->onTouchEnded = CC_CALLBACK_2(SingleGameScene::MoveEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(MoveListen, this);
	this->schedule(schedule_selector(MultiGameScene::MovePerson, this));


	//�������
	auto ArrowListen = EventListenerTouchOneByOne::create();
	ArrowListen->onTouchBegan = CC_CALLBACK_2(SingleGameScene::ArrowBegan, this);
	ArrowListen->onTouchMoved = CC_CALLBACK_2(SingleGameScene::ArrowMoved, this);
	ArrowListen->onTouchEnded = CC_CALLBACK_2(MultiGameScene::ArrowEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(ArrowListen, this);
	this->schedule(schedule_selector(MultiGameScene::MoveArrow, this));//�ҷ������ƶ�


	//��ʾѪ��
	this->schedule(schedule_selector(MultiGameScene::ShowBlood, this));
	this->schedule(schedule_selector(SingleGameScene::ChangeWeapon, this));
	//this->schedule(schedule_selector(MultiGameScene::test, this));
	this->schedule(schedule_selector(MultiGameScene::Hurt, this));//�ҷ�����������ҵ���ײ���
	return true;
}

//������

void MultiGameScene::ShowChat(bool is_clear)//true:labelȫ����Ϊ���ַ�����������ǣ�������ʾ
{
	if (is_clear)
	{
		for (auto NowLabel : AllLabel)
		{
			NowLabel->setString(String::createWithFormat("")->_string);
		}
		return;
	}
	list<string>::iterator word = AllWord.begin();
	list<Label*>::iterator label = AllLabel.begin();
	for (;word!=AllWord.end()&&label!=AllLabel.end();word++,label++)
	{
		string NowWord = *word;
		Label* NowLabel = *label;
		NowLabel->setString(String::createWithFormat("%s",NowWord.c_str())->_string);
	}
}
void MultiGameScene::chat(Ref *pSender, cocos2d::ui::TextField::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::TextField::EventType::ATTACH_WITH_IME://����༭��
	{
	}
	break;

	case cocos2d::ui::TextField::EventType::DETACH_WITH_IME://����༭�������λ��
	{
		auto s = static_cast<TextField*>(pSender);
		this->LastWord = RoleModel->name+"  :  "+s->getString();
		AllWord.push_front(LastWord);
		ShowChat(false);
		s->setText("");
	}
	break;

	case cocos2d::ui::TextField::EventType::INSERT_TEXT://��������
	{
	}
	break;

	case cocos2d::ui::TextField::EventType::DELETE_BACKWARD://����ɾ��
	{
		
	}
	break;

	default:
		break;
	}
}

//�ǳƱ༭��ص�����
void MultiGameScene::textFieldEvent(Ref *pSender, cocos2d::ui::TextField::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::TextField::EventType::ATTACH_WITH_IME://����༭��
	{
	}
	break;

	case cocos2d::ui::TextField::EventType::DETACH_WITH_IME://����༭�������λ��
	{
		//this->textField->setVisible(false);//����Ϊ���ɼ���ֱ��ɾ��������
		this->textField->removeFromParent();//������ɣ�ɾ���༭��
	}
	break;

	case cocos2d::ui::TextField::EventType::INSERT_TEXT://��������
	{
		auto  s = static_cast<TextField*>(pSender);
		RoleModel->name = s->getString();
	}
	break;

	case cocos2d::ui::TextField::EventType::DELETE_BACKWARD://����ɾ��
	{
		RoleModel->name = RoleModel->name.substr(0, RoleModel->name.length() - 1);
	}
	break;

	default:
		break;
	}
}
void MultiGameScene::LandMsg(Ref* pSender) {
	auto msg = (String*)pSender;
	log("testMsg: %s", msg->getCString());
}

//����ģ��
void MultiGameScene::connect()
{
	sock_client = new ODsocket();
	sock_client->Init();
	bool res = sock_client->Create(AF_INET, SOCK_STREAM, 0);
	res = sock_client->Connect(this->Ip, 8867);
	this->sock_client->setTimeOut(10);//����recv��ʱʱ��
}
void MultiGameScene::postMessage(float t)
{
	if (this->Ip.empty())return;
	Person* model = static_cast<Person*>(now->getChildByTag(ModelTag));
	Vec2 NowPos = now->map->getPosition();
	if (NowPos == now->LastPos && !now->LastArrow&&LastWord.empty())
	{
		return;
	}
	now->LastPos = NowPos;
	now->DataToString();
	now->sock_client->Send((char*)now->MessageToPost.c_str(), now->MessageToPost.length(), 0);



}
void MultiGameScene::getMessage(float t)

{
	if (this->Ip.empty())return;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	now->sock_client->Recv(buffer, sizeof(buffer));
	now->strmsg = buffer;
	now->StringToData();
}
void MultiGameScene::StringToData()
{
	if (now->strmsg.empty())
	{
		return;
	}
	rapidjson::Document document;
	document.Parse<0>(now->strmsg.c_str());//���ַ������ؽ�document��
	if (document.HasParseError()) { //��ӡ��������
		log("jiexicuowu");
		return;
	}
	if (document.IsObject()) {
		if (!document.HasMember("Name")) {
			log("json parse error!");
			return;
		}

		string name = document["Name"].GetString();
		if (dead.count(name))//�������Ϣ��������������ҷ��͹����ģ����账��
		{
			return;
		}
		if (name == RoleModel->name)//��������������Լ����������账��
		{
			return;
		}
		Person*NowPerson = NameToPerson.at(name);
		if (!NowPerson)//���û���ҵ�����ˣ��Ͱ�����������
		{
			auto person = Person::CreatePerson("monster.png");
			addChild(person);
			AllPersonList.push_back(person);
			NameToPerson.insert(name, person);
			NowPerson = person;
		}
		NowPerson->blood = document["Blood"].GetInt();
		NowPerson->blue = document["Blue"].GetInt();
		float deltaX = document["deltaX"].GetFloat();
		float deltaY = document["deltaY"].GetFloat();
		Vec2 Pos = Vec2(map->getPositionX() + deltaX, map->getPositionY() + deltaY);
		NowPerson->setPosition(Pos);

		/*
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
			AllArrow.pushBack(arrow);
		}*/
		if (document.HasMember("Word"))
		{
			string word = document["Word"].GetString();
			AllWord.push_front(word);
			if (is_chatting)
			{
				ShowChat(false);
			}
		}
		if (document.HasMember("Arrow")) {
			rapidjson::Value o;      //ʹ��һ���µ�rapidjson::Value�����object
			o = document["Arrow"];

			//ȷ������һ��Object
			if (o.IsObject()) {

				string picture = o["picture"].GetString();
				auto arrow = Arrow::CreateArrow(picture);
				arrow->arrow_attack = o["attack"].GetInt();
				arrow->dir = o["dir"].GetDouble();
				arrow->speed = o["speed"].GetInt();
				arrow->range = o["range"].GetInt();
				arrow->StartPosition = Vec2(NowPerson->getPositionX() + height, NowPerson->getPositionY() + height);
				arrow->setPosition(Vec2(NowPerson->getPositionX() + height, NowPerson->getPositionY() + height));
				arrow->master = NowPerson;
				this->addChild(arrow);
				AllArrow.pushBack(arrow);
				NowPerson->setRotation(o["dir"].GetDouble()*-180 / 3.1415926);
			}
			else
			{
				return;
			}

		}
		else return;

	}
}
void MultiGameScene::DataToString()//����ת����json�ַ���
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
	rapidjson::Value val;
	const string & name = RoleModel->name;
	document.AddMember("Name", val.SetString(name.c_str(), allocator), allocator);
	if (LastWord.size())
	{
		const string &word = LastWord;
		document.AddMember("Word", val.SetString(word.c_str(), allocator), allocator);//��������˾ʹ���
		LastWord.clear();
	}
	if (LastArrow)
	{
		rapidjson::Value arrow(kObjectType);
		arrow.AddMember("attack", LastArrow->arrow_attack, allocator);
		arrow.AddMember("dir", LastArrow->dir, allocator);
		arrow.AddMember("speed", LastArrow->speed, allocator);
		arrow.AddMember("range", LastArrow->range, allocator);
		const string & pic = LastArrow->picture;
		arrow.AddMember("picture", val.SetString(pic.c_str(), allocator), allocator);
		document.AddMember("Arrow", arrow, allocator);
		LastArrow = NULL;
	}

	/*
		rapidjson::Value ArrayForArrow(rapidjson::kArrayType);
		for (list<Arrow*>::iterator it = MyArrow.begin(); it != MyArrow.end(); ++it)//���ҷ����ʱ���������Ϣ�ϴ�
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
		*/
	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writer(buffer);
	document.Accept(writer);
	MessageToPost = buffer.GetString();
	now->MessageToPost = buffer.GetString();
	now->MessageToPost += '\n';
}


//������д
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
void MultiGameScene::InitAllPerson()//���ݷ������㲥�������˵������������������˵ĳ�ʼλ��
{
	auto person = Person::CreatePerson("monster.png");
	person->name = "JesPark";
	person->setPosition(RoleModel->getPosition());
	AllPersonList.push_back(person);
	NameToPerson.insert("JesPark", person);
	this->addChild(person);
}

void MultiGameScene::MovePerson(float t)
{
	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	Vector<Sprite*>ToEraseStar, ToEraseHeart;
	float dir = Rocker::getRad(rockerBG_Position, current_point);
	if (rockerBG_Position == current_point)return;
	float dx = model->speed * cos(dir);
	float dy = model->speed * sin(dir);
	Vec2 NowPos = exchange(Vec2(model->getPositionX() + height, model->getPositionY() + height));//��õ�ǰ����������tile����

	Vec2 next_pos = Vec2(model->getPositionX() + height * (cos(dir)), model->getPositionY() + height * (sin(dir)));//������һ�̼����ƶ�����λ��  ��������Ϊһ��Բ����
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
	if (check(next_pos))//��ײ���
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
	//��ͼ��������������ƶ�
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
void MultiGameScene::Dead(float t)//�Լ�����������ת���������������а��ϳ���
{
	list<Person*>ToErase;
	Person* Model = static_cast<Person*>(getChildByTag(ModelTag));
	for (auto NowPerson : AllPersonList)
	{
		if (NowPerson->blood <= 0)
		{
			
			if (NowPerson == Model)
			{
				Director::getInstance()->replaceScene(GameOverScene::CreateScene());
			}
			auto blood = blood_hash.at(NowPerson);
			this->removeChild(blood);
			ToErase.push_back(NowPerson);
			dead.insert(NowPerson->name);//��Ҫ�������ڴ滹û���ͷ�֮ǰ������������ӽ���������
			NowPerson->removeFromParent();
		}
	}
	for (auto x : ToErase)
	{
		AllPersonList.remove(x);
	}
}
void MultiGameScene::ArrowEnded(Touch* t, Event*e)//��Ҫ������ļ���ӽ�MyArrow������
{
	if (start == t->getLocation())return;
	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	if (!model)
	{
		Director::getInstance()->replaceScene(GameOverScene::CreateScene());
	}
	std::string picture = model->weapon->picture;
	//����һֻ�����������weapon�Լ����и�ֵ
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
	AllArrow.pushBack(arrow);
	LastArrow = arrow;
	this->addChild(arrow);
}
void MultiGameScene::Hurt(float t)
{
	list<Arrow*>ToErase;
	Person* model = static_cast<Person*>(getChildByTag(ModelTag));
	if (!model)
	{
		Director::getInstance()->replaceScene(GameOverScene::CreateScene());
	}
	for (list<Person*>::iterator it = AllPersonList.begin(); it != AllPersonList.end(); ++it)
	{
		Person* NowPerson = *it;
		Rect NowPerson_pos = Rect(NowPerson->getPositionX(), NowPerson->getPositionY(), height * 2, height * 2);
		for (Vector<Arrow*>::iterator i = AllArrow.begin(); i != AllArrow.end(); ++i)
		{
			Arrow* NowArrow = *i;
			Rect NowArrow_pos = Rect(NowArrow->getPositionX(), NowArrow->getPositionY(), NowArrow->arrow_size, NowArrow->arrow_size);
			if (NowPerson_pos.intersectsRect(NowArrow_pos))//��������
			{

				if (NowArrow->master == NowPerson)continue;
				//NowArrow->master->person_score++;
				if (NowArrow->master == model)//���������б���-->�ӷ�
				{
					score++;
					LabelScore->setString(String::createWithFormat("Score:%d", score)->_string);
				}
				if (NowPerson == model)//�����ұ�����-->��Ѫ
				{
					model->blood-= NowArrow->arrow_attack*NowArrow->master->attack;
				}
				float dx = NowArrow->speed*cos(NowArrow->dir);//�ܻ������λ��
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
	for (auto x : ToErase)//ɾ����
	{
		AllArrow.eraseObject(x);
	}
}
void MultiGameScene::MoveArrow(float t)
{
	list<Arrow*>ToErase;
	for (Vector<Arrow*>::iterator it = AllArrow.begin(); it != AllArrow.end(); it++)
	{
		Arrow* nowArrow = *it;
		double dis = distance(nowArrow->getPosition(), nowArrow->StartPosition);
		if (dis >= nowArrow->range)
		{
			this->removeChild(nowArrow);
			ToErase.push_back(nowArrow);
			continue;
		}
		float dx = nowArrow->speed*cos(nowArrow->dir);//�ƶ���
		float dy = nowArrow->speed*sin(nowArrow->dir);
		Vec2 next_pos = Vec2(nowArrow->getPositionX() + dx, nowArrow->getPositionY() + dy);
		if (!check(next_pos))//����ǽ�ڵ���ײ���
		{
			this->removeChild(nowArrow);
			ToErase.push_back(nowArrow);
		}
		else
			nowArrow->setPosition(next_pos);
	}
	for (auto x : ToErase)
	{
		AllArrow.eraseObject(x);
	}
}
void MultiGameScene::test(float t)
{
	log("%s", RoleModel->name.c_str());
}



