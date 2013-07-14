//
//  CGameScene.cpp
//  Snake
//
//  Created by 刘 晓霞 on 13-7-7.
//
//

#include "CGameScene.h"

const CCPoint middlePoint = ccp(240,160);
#define kEvenTypePartImagePath "white.png"
#define kOddTypePartImagePath "yellow.png"
#define kSnakeSartPos ccp(19,14)
#define kMaxRandomFoodPosTimes 3
#define kSnakeZOrder 10
#define kFoodZOrder 15

const char* g_foodImagePaths[] =
{
    "blue.png",
    "green.png",
    "purple.png"
};

bool CGameScene::init()
{
    if(!CCLayer::init())
    {        
        return false;
    }
    
    CCSprite* bg = CCSprite::create("bg.jpg");
    bg->setPosition(middlePoint);
    this->addChild(bg);
    
    m_bg = CCSprite::create("bg1.jpg");
    m_bg->setPosition(middlePoint);
    this->addChild(m_bg);
    
    initSnake(10);
    addFoodToScene(createFood());
    m_snakeDirection = kSnakeDirection_Left;
    
    this->setTouchEnabled(true);
    this->setTouchMode(kCCTouchesOneByOne);
    
    return true;
}

// 游戏的主要逻辑
void CGameScene::update(float delta)
{
    if(m_bisGameover)//游戏结束，弹出主循环
    {
        return;
    }
    
    // 控制蛇前进速度的计数器
    static float l_timer = 0.0f;
    l_timer += delta;
    if(l_timer < 0.8f/m_iCurrentLevel)
    {
        return;
    }
    else
    {
        l_timer = 0.0;
    }
    
    // 蛇坐标的改变
    int deltaX = 0;
    int deltaY = 0;
    
    switch (m_snakeDirection) {
        case kSnakeDirection_Up:
            deltaY++;
            break;
        case kSnakeDirection_Down:
            deltaY--;
            break;
        case kSnakeDirection_Left:
            deltaX--;
            break;
        case kSnakeDirection_Right:
            deltaX++;
            break;
        default:
            break;
    }
    
    SnakeBodyPart* header = m_snake.at(0);// 蛇头结点
    int iSnakeSize = m_snake.size();// 蛇长
    CCPoint tailPos = m_snake.at(iSnakeSize - 1)->m_pos;// 蛇尾位置
    
    // 移动蛇的身体（除头部以外的部分）
    SnakeBodyPart* tempPart = NULL;
    SnakeBodyPart* tempPrevious = NULL;
    
    // 更新每一节（除头）的坐标
    for (int i = iSnakeSize - 1; i>0; i--)
    {
        tempPart = m_snake[i];
        tempPrevious = m_snake[i-1];
        tempPart->updatePos(tempPrevious->m_pos);
    }
    // 即将更新头节点的坐标
    CCPoint nextPos = header->m_pos;
    nextPos.x += deltaX;
    nextPos.y += deltaY;
    
    // 判断蛇是否碰到自己身体，即游戏结束
    for (int i = 0; i < m_snake.size(); i++)
    {
        if(nextPos.x == m_snake.at(i)->m_pos.x && nextPos.y == m_snake.at(i)->m_pos.y)
        {
            m_bisGameover = true;
            break;
        }
    }
    
    // 当蛇碰到墙壁的坐标更新
    if(!SnakeBodyPart::isPosEnable(nextPos))
    {
        switch (m_snakeDirection) {
            case kSnakeDirection_Up:
                nextPos.y = 0;
                break;
            case kSnakeDirection_Down:
                nextPos.y = kMaxPosY-1;
                break;
            case kSnakeDirection_Left:
                nextPos.x = kMaxPosX-1;
                break;
            case kSnakeDirection_Right:
                nextPos.x = 0;
                break;
            default:
                break;
        }
    }
    
    // 更新蛇头坐标
    header->updatePos(nextPos);
    
    // 判断是否遇到食物
    CCPoint foodPos = m_currentFood->m_pos;
    if(nextPos.x == foodPos.x && nextPos.y == foodPos.y)
    {
        SnakeBodyPart* part=NULL;
        const char* partImgPath=NULL;
        
        if (iSnakeSize%2==0)
        {
            partImgPath=kEvenTypePartImagePath;
        }
        else
        {
            partImgPath=kOddTypePartImagePath;
        }
        
        part=SnakeBodyPart::NewPart(partImgPath,tailPos);
        addSnakePartToSnake(part);
        
        deleteFood();
        
        addFoodToScene(createFood());
    }
}

// 开启update
void CGameScene::onEnterTransitionDidFinish()
{
    CCLayer::onEnterTransitionDidFinish();
    scheduleUpdate();
}

void CGameScene::onExit()
{
    CCLayer::onExit();
    unscheduleUpdate();
}

void CGameScene::initSnake(int length)
{
    SnakeBodyPart* part = NULL;
    char* partImagePath = NULL;
    CCPoint pos = kSnakeSartPos;
    for(int i = 0; i < length; i++)
    {
        if(i%2 == 0)
        {
            partImagePath = kEvenTypePartImagePath;
        }
        else
        {
            partImagePath = kOddTypePartImagePath;
        }
        pos.x++;
        part = SnakeBodyPart::NewPart(partImagePath,pos);
        addSnakePartToSnake(part);
    }
}

void CGameScene::addSnakePartToSnake(SnakeBodyPart* part)
{
    m_snake.push_back(part);
    CCPoint posInScreen = ccp(part->m_pos.x * 10, part->m_pos.y * 10);
    part->m_partSprite->setPosition(posInScreen);
    m_bg->addChild(part->m_partSprite, kSnakeZOrder);
}

SnakeBodyPart* CGameScene::createFood()
{
    SnakeBodyPart* food = NULL;
    int foodImageIndex = random();
    foodImageIndex %= sizeof(g_foodImagePaths) / sizeof(const char*);
    
    const char* foodImagePath = g_foodImagePaths[foodImageIndex];
    
    int posX = 0;
    int posY = 0;
    for(int i = 0; i < kMaxRandomFoodPosTimes; i++)
    {
        posX = random() % kMaxPosX;
        posY = random() % kMaxPosY;
        
        if(isFoodPosEnable(ccp(posX,posY)))
        {
            break;
        }
    }
    
    food = SnakeBodyPart::NewPart(foodImagePath,ccp(posX,posY));
    return food;
}

void CGameScene::deleteFood()
{
    m_currentFood->m_partSprite->removeFromParentAndCleanup(true);
    delete m_currentFood;
    m_currentFood = NULL;
}

bool CGameScene::isFoodPosEnable(CCPoint pos)
{
    if(!SnakeBodyPart::isPosEnable(pos))
    {
        return false;
    }
    
    SnakeBodyPart* part = NULL;
    int snakeLength = m_snake.size();
    for(int i = 0; i < snakeLength; i++)
    {
        part = m_snake.at(i);
        if(part->m_pos.x == pos.x && part->m_pos.y == pos.y)
        {
            return false;
        }
    }
    return true;
}
               
void CGameScene::addFoodToScene(SnakeBodyPart *food)
{
    m_currentFood = food;
    CCPoint posInScreen = ccp(food->m_pos.x * 10, food->m_pos.y * 10);
    food->m_partSprite->setPosition(posInScreen);
    m_bg->addChild(food->m_partSprite, kFoodZOrder);
    
    CCBlink* action = CCBlink::create(1, 2);
    CCRepeatForever* repeat = CCRepeatForever::create(action);
    food->m_partSprite->runAction(repeat);
}


bool CGameScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint touchPos = pTouch->getLocation();
    SnakeBodyPart* header = m_snake.at(0);
    
    CCPoint snakeHeaderPos = header->m_pos;
    snakeHeaderPos.x *= 10.f;
    snakeHeaderPos.y *= 10.f;
    
    float delta = 0.f;
    switch (m_snakeDirection) {
        case kSnakeDirection_Up:
        case kSnakeDirection_Down:
        {
            delta = touchPos.x - snakeHeaderPos.x;
            if(delta > 0)
            {
                changeSnakeDirection(kSnakeDirection_Right);
            }
            else if(delta < 0)
            {
                changeSnakeDirection(kSnakeDirection_Left);
            }
            break;
        }
        case kSnakeDirection_Right:
        case kSnakeDirection_Left:
        {
            delta = touchPos.y - snakeHeaderPos.y;
            if(delta > 0)
            {
                changeSnakeDirection(kSnakeDirection_Up);
            }
            else if(delta < 0)
            {
                changeSnakeDirection(kSnakeDirection_Down);
            }
            break;
        }
            
        default:
            break;
    }
    return true;
}
