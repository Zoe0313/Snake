//
//  CGameScene.h
//  Snake
//
//  Created by 刘 晓霞 on 13-7-7.
//
//

#ifndef __Snake__CGameScene__
#define __Snake__CGameScene__

#include "cocos2d.h"
#include <vector>
using namespace cocos2d;

#define kMaxPosX 40
#define kMaxPosY 30

enum SnakeDirection {
    kSnakeDirection_invaild = 0,
    kSnakeDirection_Up = 1,
    kSnakeDirection_Right,
    kSnakeDirection_Down,
    kSnakeDirection_Left
};

class SnakeBodyPart
{
public:
    static bool isPosEnable(CCPoint pos)
    {
        if(pos.x < 0 || pos.y < 0 || pos.x >= kMaxPosX || pos.y >= kMaxPosY)
        {
            return false;
        }
        return true;
    }
    
    static SnakeBodyPart* NewPart(const std::string& partImgPath, const CCPoint pos)
    {
        SnakeBodyPart* temp = new SnakeBodyPart();
        temp->m_partSprite = CCSprite::create(partImgPath.c_str());
        temp->m_partSprite->retain();
        temp->m_pos = pos;
        
        if(temp->m_partSprite == NULL || !isPosEnable(pos))
        {
            delete temp;
            temp = NULL;
            return temp;
        }
        return temp;
    }
    
    SnakeBodyPart()
    :m_partSprite(NULL),
    m_pos(CCPointZero)
    {
    }
    
    ~SnakeBodyPart()
    {
        if(m_partSprite)
        {
            m_partSprite->release();
            m_partSprite = NULL;
        }
    }
    
    void updatePos(CCPoint pos)
    {
        if(isPosEnable(pos))
        {
            m_pos = pos;
            m_partSprite->setPosition(ccp(m_pos.x*10,m_pos.y*10));
        }
        else
        {
            CCLOG("Warning: pos(%2f, %2f) isn't enable", pos.x, pos.y);
        }
    }
    
    CCSprite* m_partSprite;
    CCPoint m_pos;
};


class CGameScene: public CCLayer
{
public:
    static CCScene* scene()
    {
        CCScene *scene = CCScene::create();
        CGameScene *pLayer = CGameScene::create();
        scene->addChild(pLayer);
        
        return scene;
    }
    
    CREATE_FUNC(CGameScene);
    
    CGameScene()
    :m_bg(NULL),
    m_currentFood(NULL),
    m_snakeDirection(kSnakeDirection_invaild),
    m_iCurrentLevel(7),
    m_bisGameover(false)
    {
    }
    
    // 创建游戏中用到的元素，并将其添加到Layer上
    virtual bool init();
    
    // 游戏的主要逻辑
    virtual void update(float delta);
    
    // 开启update
    virtual void onEnterTransitionDidFinish();
    virtual void onExit();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
private:
    void initSnake(int length=3);
    void addSnakePartToSnake(SnakeBodyPart* part);

    bool isFoodPosEnable(CCPoint pos);
    void addFoodToScene(SnakeBodyPart *food);
    
    SnakeBodyPart* createFood();
    void deleteFood();
  
    // 点击屏幕，根据蛇当前的前进方向确定蛇的转向（向上运动，点击蛇身左侧，蛇的转向为左）
    void changeSnakeDirection(SnakeDirection direction)
    {
        m_snakeDirection = direction;
    }
    
private:
    // 背景Sprite的引用，方便代码中获得背景Sprite
    CCSprite* m_bg;
    
    // 一条蛇
    std::vector<SnakeBodyPart*> m_snake;
    
    // 食物
    SnakeBodyPart* m_currentFood;
    
    // 蛇的方向
    SnakeDirection m_snakeDirection;
    
    // 游戏等级（决定蛇的前进速度，越高越快）
    int m_iCurrentLevel;
    
    // 蛇撞自己身上，游戏结束
    bool m_bisGameover;
};


#endif /* defined(__Snake__CGameScene__) */
