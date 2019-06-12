#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "SingleGameScene.h"
#include "Land.h"
#include "MultiGameScene.h"
#include "HelpScene.h"
#include "StoreScene.h"
USING_NS_CC;
//��������
Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// ��ӡ�쳣
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool HelloWorld::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

   //��ӹرղ˵�
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }
	//������ֹ��ܲ˵�
	auto SingleGameItem = MenuItemFont::create("Start SingleGame", CC_CALLBACK_1(HelloWorld::menuCallback, this));//������Ϸ
	auto MultiGameItem = MenuItemFont::create("Start MultiGame", CC_CALLBACK_1(HelloWorld::menuCallback, this));//������Ϸ
	auto HelpItem = MenuItemFont::create("Help", CC_CALLBACK_1(HelloWorld::menuCallback, this));//����
	auto StoreItem = MenuItemFont::create("Store", CC_CALLBACK_1(HelloWorld::menuCallback, this));//�̵�
	SingleGameItem->setPosition(Point(origin.x + visibleSize.width / 2 - closeItem->getContentSize().width / 2, 200));//Ϊ�˵�����λ��
	MultiGameItem->setPosition(Point(origin.x + visibleSize.width / 2 - closeItem->getContentSize().width / 2, 150));
	HelpItem->setPosition(Point(origin.x + visibleSize.width / 2 - closeItem->getContentSize().width / 2, 100));
	StoreItem->setPosition(Point(origin.x + visibleSize.width / 2 - closeItem->getContentSize().width / 2, 50));
	SingleGameItem->setTag(SingleGameTag);//Ϊ�˵����ñ�ǩ
	MultiGameItem->setTag(MultiGameTag);
	HelpItem->setTag(HelpTag);
	StoreItem->setTag(StoreTag);
	// ����menu
	auto menu = Menu::create(closeItem, SingleGameItem, MultiGameItem, HelpItem, StoreItem,NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

	//����label ��ӭ���� �������ִ���ս��
    auto label = Label::createWithTTF("Welcome To Arrow War", "fonts/Marker Felt.ttf", 24);
	label->setColor(Color3B::RED);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

   //������ʼ�����ı���ͼƬ
    auto sprite = Sprite::create("background.png");
    if (sprite == nullptr)
    {
        problemLoading("'HelloWorld.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}

void HelloWorld::menuCallback(Ref* pSender)
{
	auto NowItem = static_cast<MenuItem*>(pSender);
	int tag = NowItem->getTag();
	switch (tag)
	{
	case SingleGameTag:
		Director::getInstance()->replaceScene(SingleGameScene::CreateScene());
		break;
	case MultiGameTag:
		Director::getInstance()->replaceScene(Land::CreateScene());
		break;
	case HelpTag:
		Director::getInstance()->replaceScene(HelpScene::CreateScene());
		break;
	case StoreTag:
		Director::getInstance()->replaceScene(StoreScene::CreateScene());
		break;
	}
}
