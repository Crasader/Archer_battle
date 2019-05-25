#include "MultiGameScene.h"
#include "HelloWorldScene.h"
#include"ODsocket.h"
#include<pthread.h>
using namespace std;
#pragma comment(lib, "pthreadVC2.lib")
USING_NS_CC;
static MultiGameScene* now;
bool MultiGameScene::init()
{
	if (!Layer::init())return false;
	//this->schedule(schedule_selector(MultiGameScene::Get, this));
	//this->schedule(schedule_selector(MultiGameSc                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    ene::Post, this));
	
	auto label = LabelTTF::create("HelloWorld", "Arial", 24);
	label->setTag(111);
	label->setPosition(Vec2(480, 240));
	addChild(label, 1);
	//���ӷ�����
	now = this;
	sock_client = new ODsocket();
	sock_client->Init();
	bool res = sock_client->Create(AF_INET, SOCK_STREAM, 0);
	res = sock_client->Connect("192.168.1.103", 8867);
	//�������������Ϣ
	this->postMessage();
	if (res)
	{
		pthread_t tid;
		bool ok = pthread_create(&tid, NULL, MultiGameScene::getMessage, NULL);//����������Ϣ���߳�
	}



	this->scheduleUpdate();
	return true;
}
Scene* MultiGameScene::CreateScene()
{
	auto scene = Scene::create();
	auto layer = MultiGameScene::create();
	scene->addChild(layer);
	return scene;
}
void MultiGameScene::postMessage()
{
	MessageToPost = "Test";
	sock_client->Send((char*)MessageToPost.c_str(), MessageToPost.length(), 0);
}
void* MultiGameScene::getMessage(void *)
{
	char buffer[1024];
	while (1)
	{	
		memset(buffer, 0, sizeof(buffer));
		now->sock_client->Recv(buffer, sizeof(buffer));
		now->strmsg = buffer;
	}
}
void MultiGameScene::update(float t)
{
	LabelTTF* label = (LabelTTF*)getChildByTag(111);
	label->setString(now->strmsg.c_str());
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