#include "MultiGameScene.h"
#include "HelloWorldScene.h"
#include"ODsocket.h"
#include<pthread.h>
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
	//��ʼ��
	auto winsize = Director::getInstance()->getWinSize();
	this->InitValue();
	this->initWeapon();
	this->map = TMXTiledMap::create("Tank2.tmx");
	map->setPosition(Vec2(224, -2074));
	this->addChild(map, -1);
	GetPos();
	InitMap();
	/*
	auto label = LabelTTF::create("HelloWorld", "Arial", 24);
	label->setTag(111);
	label->setPosition(Vec2(480, 240));
	addChild(label, 1);
	*/
	//��������
	auto model = Person::CreatePerson("person.png");
	model->setTag(ModelTag);
	this->addChild(model);
	model->setPosition(Vec2(winsize.width / 2, winsize.height / 2));
	AllPerson.pushBack(model);
	//ͨ��
	now = this;
	sock_client = new ODsocket();
	sock_client->Init();
	bool res = sock_client->Create(AF_INET, SOCK_STREAM, 0);
	res = sock_client->Connect("192.168.1.103", 8867);
	if (res)
	{
		/*pthread_t gost;
		bool ok_1 = pthread_create(&gost, NULL, MultiGameScene::postMessage, NULL);//����������Ϣ���߳�
		pthread_t get;
		bool ok_2 = pthread_create(&get, NULL, MultiGameScene::getMessage, NULL);//����������Ϣ���߳�
		*/
		this->schedule(schedule_selector(MultiGameScene::postMessage, this));
		this->schedule(schedule_selector(MultiGameScene::getMessage, this));
	}

	
	//NameToPerson.insert(model->name.model);//��ʵ���üӣ���Ϊ�����ѯ�Լ�
	//�����������
	//InitAllPerson();
	//�������Ǻ���
	InitHeartStar();
	//����ҡ��
	auto rocker = Rocker::create("CloseSelected.png", "rr.png", rockerBG_Position);
	rocker->setTag(RockerTag);
	this->addChild(rocker);
	rocker->StartRocker();

	//this->schedule(schedule_selector(MultiGameScene::StringToData, this));//ģ����Ϸ����
	//ʵ����Ϸ����
	/*
	this->schedule(schedule_selector(MultiGameScene::MoveAllPerson, this));
	this->schedule(schedule_selector(MultiGameScene::Shoot, this));
	this->schedule(schedule_selector(SingleGameScene::Hurt, this));
	this->schedule(schedule_selector(MultiGameScene::Dead, this));
	*/
	//���Ǻ��ĵĲ���
	this->schedule(schedule_selector(SingleGameScene::SupplyHeartStar, this));

	//���������ƶ�
	auto MoveListen = EventListenerTouchOneByOne::create();
	MoveListen->setSwallowTouches(true);
	MoveListen->onTouchBegan = CC_CALLBACK_2(SingleGameScene::MoveBegan, this);
	MoveListen->onTouchMoved = CC_CALLBACK_2(SingleGameScene::MoveMoved, this);
	MoveListen->onTouchEnded = CC_CALLBACK_2(SingleGameScene::MoveEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(MoveListen, this);
	this->schedule(schedule_selector(SingleGameScene::MovePerson, this));


	//�������
	auto ArrowListen = EventListenerTouchOneByOne::create();
	ArrowListen->onTouchBegan = CC_CALLBACK_2(SingleGameScene::ArrowBegan, this);
	ArrowListen->onTouchMoved = CC_CALLBACK_2(SingleGameScene::ArrowMoved, this);
	ArrowListen->onTouchEnded = CC_CALLBACK_2(MultiGameScene::ArrowEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(ArrowListen, this);
	this->schedule(schedule_selector(SingleGameScene::MoveArrow, this));


	//��ʾѪ��
	this->schedule(schedule_selector(SingleGameScene::ShowBlood, this));
	this->schedule(schedule_selector(SingleGameScene::ChangeWeapon, this));

	return true;
}





/*
//��ʼ��ϵ�к���
void MultiGameScene::InitValue()
{
	//��������
	model = Person::CreatePerson("person.png");
	this->addChild(model);
	model->setPosition(Vec2(winsize.width / 2, winsize.height / 2));
	AllPerson.pushBack(model);
	this->rockerBG_Position = Vec2(100, 100);
	this->current_point = rockerBG_Position;
	this->score = 0;
	height = 20;
	counts = 0;
	LabelScore = Label::createWithTTF("Score:0", "fonts/Marker Felt.ttf", 32);
	LabelScore->setPosition(Vec2(100, 400));
	LabelScore->setColor(Color3B::RED);
	LabelMagic = Label::createWithTTF("Magic:0", "fonts/Marker Felt.ttf", 32);
	LabelMagic->setPosition(Vec2(100, 350));
	LabelMagic->setColor(Color3B::RED);
	this->addChild(LabelScore);
	this->addChild(LabelMagic);
	NowWeapon_1 = Arrow::CreateArrow("CloseNormal.png");
	NowWeapon_1->retain();
	NowWeapon_2 = Arrow::CreateArrow("CloseNormal.png");
	NowWeapon_2->retain();
	NowWeapon_3 = Arrow::CreateArrow("CloseNormal.png");
	NowWeapon_3->retain();
}
void MultiGameScene::initWeapon()
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
void MultiGameScene::InitMap()
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
*/
/*
//��Ʒģ��
void MultiGameScene::ShowBlood(float t)
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
void MultiGameScene::ChangeWeapon(float t)
{
	srand(time(NULL));
	Person* model = static_cast<Person*>(getChildByTag(ModelTag));
	if (!model)
	{
		log("wtf");
	}
	if (model->blue < 100)
	{
		//model->blue += 5;
		//log("blue:%d", model->blue);
		return;
	}
	Vector<MenuItem*>v;
	for (int i = 1; i <= 3; i++)
	{

		int num = random(0, AllWeapon.size());
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
			menu->setPosition(Vec2(model->getPositionX() - 500, model->getPositionY() - 350));
			menu->setTag(2);
			ArrowCopy(NowWeapon_2, *it);
		}
		else
		{
			menu->setPosition(Vec2(model->getPositionX() - 400, model->getPositionY() - 350));
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
void MultiGameScene::MenuCallBack(cocos2d::Ref*pSender)
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
}
void MultiGameScene::InitHeartStar()
{
	for (int i = 1; i <= 40; i++)
	{
		CreateHeartStar("heart.png");
		CreateHeartStar("star.png");
	}
}
void MultiGameScene::CreateHeartStar(const std::string &filename)
{
	srand(counts++);
	int dx[2] = { 30,0 };
	int dy[2] = { 0,30 };
	int star_dir = random(0, 2);

	GetPos();
	Vec2 anchor = map->getPosition();
	Vec2 star_start = Vec2(random(Two.x, One.x), random(Three.y, One.y));
	for (int k = 0; k < 5; k++)
	{
		Sprite* sprite = Sprite::create(filename);
		float x = star_start.x + dx[star_dir] * k;
		float y = star_start.y + dy[star_dir] * k;
		sprite->setPosition(Vec2(x, y));
		if (check(Vec2(x, y)) && IsInBound(Vec2(x, y)))
		{

			this->addChild(sprite);
			if (filename == "star.png")
			{
				AllStar.pushBack(sprite);
			}
			else AllHeart.pushBack(sprite);
		}
	}
}
void MultiGameScene::SupplyHeartStar(float t);
*/

//����ģ��
void MultiGameScene::postMessage(float t)
{
	
	now->DataToString();
	now->sock_client->Send((char*)now->MessageToPost.c_str(), now->MessageToPost.length(), 0);
	return;
}
void MultiGameScene::getMessage(float t)

{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));	
	now->sock_client->Recv(buffer, sizeof(buffer));
	now->strmsg = buffer;
	//����Ϊdebug
	rapidjson::Document document;
	document.Parse<0>(now->strmsg.c_str());//���ַ������ؽ�document��
	if (document.HasParseError()) { //��ӡ��������
		log("jiexicuowu");
	}
	if (document.IsObject()) {
		if (!document.HasMember("Name")) {
			log("json parse error!");
			return;
		}
		Person* model = static_cast<Person*>(getChildByTag(ModelTag));
		string name = document["Name"].GetString();
		if (dead.count(name))//�������Ϣ��������������ҷ��͹����ģ����账��
		{
			return;
		}
		if (name == model->name)//��������������Լ����������账��
		{
			return;
		}
		Person*NowPerson = NameToPerson.at(name);
		if (!NowPerson)//���û���ҵ�����ˣ��Ͱ�����������
		{
			NowPerson = Person::CreatePerson("person.png");
			this->addChild(NowPerson);
			AllPerson.pushBack(NowPerson);
			NameToPerson.insert(name, NowPerson);
		}
		NowPerson->blood = document["Blood"].GetInt();
		NowPerson->blue = document["Blue"].GetInt();
		Vec2 NowPos;
		NowPos.x = document["Row"].GetInt();
		NowPos.y = document["Col"].GetInt();
		NowPerson->setPosition(ToOpenGL(NowPos));

		const rapidjson::Value& a = document["Arrow"];
		assert(a.IsArray());
		for (auto& v : a.GetArray())
		{
			string picture = v["picture"].GetString();
			auto arrow = Arrow::CreateArrow(picture);
			arrow->arrow_attack = v["attack"].GetInt();
			arrow->dir = v["dir"].GetDouble();
			arrow->speed = v["speed"].GetInt();
			arrow->setPosition(Vec2(v["start_x"].GetFloat(), v["start_y"].GetFloat()));
			arrow->master = NowPerson;
			this->addChild(arrow);
			AllArrow.pushBack(arrow);
		}
	}
}
void MultiGameScene::StringToData(float t)
{
	
	rapidjson::Document document;
	document.Parse<0>(now->strmsg.c_str());//���ַ������ؽ�document��
	if (document.HasParseError()) { //��ӡ��������
		log("jiexicuowu");
	}
	if (document.IsObject()) {
		if (!document.HasMember("Name")) {
			log("json parse error!");
			return;
		}
		Person* model = static_cast<Person*>(getChildByTag(ModelTag));
		string name = document["Name"].GetString();
		if (dead.count(name))//�������Ϣ��������������ҷ��͹����ģ����账��
		{
			return;
		}
		if (name == model->name)//��������������Լ����������账��
		{
			return;
		}
		Person*NowPerson = NameToPerson.at(name);
		if (!NowPerson)//���û���ҵ�����ˣ��Ͱ�����������
		{
			NowPerson = Person::CreatePerson("person.png");
			this->addChild(NowPerson);
			AllPerson.pushBack(NowPerson);
			NameToPerson.insert(name, NowPerson);
		}
		NowPerson->blood = document["Blood"].GetInt();
		NowPerson->blue = document["Blue"].GetInt();
		Vec2 NowPos;
		NowPos.x = document["Row"].GetInt();
		NowPos.y = document["Col"].GetInt();
		NowPerson->setPosition(ToOpenGL(NowPos));

		const rapidjson::Value& a = document["Arrow"];
		assert(a.IsArray());
		for (auto& v : a.GetArray())
		{
			string picture = v["picture"].GetString();
			auto arrow = Arrow::CreateArrow(picture);
			arrow->arrow_attack = v["attack"].GetInt();
			arrow->dir = v["dir"].GetDouble();
			arrow->speed = v["speed"].GetInt();
			arrow->setPosition(Vec2(v["start_x"].GetFloat(), v["start_y"].GetFloat()));
			arrow->master = NowPerson;
			this->addChild(arrow);
			AllArrow.pushBack(arrow);
		}
	}
}
void MultiGameScene::DataToString()//����ת����json�ַ���
{
	//log("using DataToString %d", times++);
	Person* model = static_cast<Person*>(this->getChildByTag(ModelTag));
	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	Vec2 pos = exchange(model->getPosition());
	
	document.AddMember("Blood", model->blood, allocator);
	document.AddMember("Blue", model->blue, allocator);
	document.AddMember("Row", static_cast<int>(pos.x), allocator);
	document.AddMember("Col", static_cast<int>(pos.y), allocator);
	const string & name = model->name;
	rapidjson::Value val;
	document.AddMember("Name", val.SetString(name.c_str(), allocator), allocator);

	
	
	rapidjson::Value ArrayForArrow(rapidjson::kArrayType);
	for (auto NowArrow : MyArrow)//���ҷ����ʱ���������Ϣ�ϴ�
	{
		rapidjson::Value object(rapidjson::kObjectType);
		object.AddMember("attack", NowArrow->arrow_attack, allocator);
		object.AddMember("dir", NowArrow->dir, allocator);
		object.AddMember("speed", NowArrow->speed, allocator);
		object.AddMember("start_x", NowArrow->StartPosition.x, allocator);
		object.AddMember("start_y", NowArrow->StartPosition.y, allocator);
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
	MessageToPost=buffer.GetString();
	now->MessageToPost = buffer.GetString();//now��������ʲô��
}
void MultiGameScene::update(float t)
{
	LabelTTF* label = (LabelTTF*)getChildByTag(111);
	label->setString(now->strmsg.c_str());
}


//������д
/*
void MultiGameScene::InitAllPerson()//���ݷ������㲥�������˵������������������˵ĳ�ʼλ��
{

}
void MultiGameScene::MoveAllPerson(float t)//���������˵�λ��
{

}
void MultiGameScene::Shoot(float t)//ģ��������ҵ����
{

}
*/
void MultiGameScene::Dead(float t)//�Լ�����������ת���������������а��ϳ���
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
		dead.insert(x->name);//������������Ҽ�����������
	}
}
void MultiGameScene::ArrowEnded(Touch* t, Event*e)//��Ҫ������ļ���ӽ�MyArrow������
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
	arrow->StartPosition = model->getPosition() + Vec2(height, height);
	arrow->master = model;
	float NowDir = -model->getRotation();
	while (NowDir < -180)
	{
		NowDir += 360;
	}
	arrow->dir = NowDir * 3.1415926 / 180;
	AllArrow.pushBack(arrow);
	MyArrow.pushBack(arrow);
	this->addChild(arrow);
}

Vec2 MultiGameScene::ToOpenGL(Vec2 pos)
{
	Vec2 anchor = map->getPosition();
	float y = anchor.y + 3000 - 30 * pos.x;
	float x = anchor.x + 30 + pos.y;
	return Vec2(x, y);
}

void MultiGameScene::Get(float t)
{
	HttpRequest* request = new HttpRequest();//����һ��������� 
	request->setUrl("https://www.baidu.com/");//����request���ʵ�URL 
	request->setRequestType(HttpRequest::Type::GET);//����Ϊgetͨ�ŷ�ʽ 
	//char buffer[256];
	//request->setRequestData(buffer, 256);
	request->setResponseCallback(this, httpresponse_selector(MultiGameScene::onHttpRequestCompleted));
	request->setTag("GET test1");
	HttpClient::getInstance()->send(request);//�������� 
	request->release();//�ͷ� 

}
void MultiGameScene::Post(float t)
{
	HttpRequest* request = new HttpRequest();
	request->setUrl("http://httpbin.org/ip");
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(this, httpresponse_selector(MultiGameScene::onHttpRequestCompleted));

	// write the post data
	const char* postData = "visitor=cocos2d&TestSuite=Extensions Test/NetworkTest";
	request->setRequestData(postData, strlen(postData));

	request->setTag("POST test1");
	HttpClient::getInstance()->send(request);
	request->release();
}
void MultiGameScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)//�ص����� 
{
	if (!response)
	{
		return;
	}

	// You can get original request type from: response->request->reqType
	if (0 != strlen(response->getHttpRequest()->getTag())) //��ȡ����ı�ǩ 
	{
		log("%s completed", response->getHttpRequest()->getTag());
	}
	
	int statusCode = response->getResponseCode();//��ȡ�����״̬�롣״̬��Ϊ200��ʾ����ɹ�
	char statusString[64] = {};
	sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
	//_labelStatusCode->setString(statusString);
	log("response code: %d", statusCode);
	
	if (!response->isSucceed())
	{
		log("response failed");
		log("error buffer: %s", response->getErrorBuffer());
		return;
	}

	// dump data
	std::vector<char> *buffer = response->getResponseData();
	/*std::string buf(buffer->begin(),buffer->end());
	log("Http Test, dump data:%s",buf.c_str());
	char* responseString = new char[buffer->size() + 1];
	std::copy(buffer->begin(), buffer->end(), responseString);
	log("Http Test, dump data:%s",responseString);*/
	std::stringstream oss;
	for (unsigned int i = 0; i < buffer->size(); i++) {
		oss << (*buffer)[i];
	}
	std::string temp = oss.str();
	const char * buf = temp.c_str();
	log("Http Test, dump data:%s", buf);
}