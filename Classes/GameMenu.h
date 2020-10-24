#pragma once

#include "cocos2d.h"
#include "constants.h"
#include "RobotSprite.h"
using namespace cocos2d;

class GameMenu : public Sprite
{
public:

    enum class BUTTON_TYPE {
        MOVE,
        STAUTS,
        ATTACK,

        NEW_ROUND,
        TEAM
    };

    GameMenu();
    ~GameMenu();

    CREATE_FUNC(GameMenu);
    bool init() override;
    void clear_items();
    void add_item(BUTTON_TYPE type);
    void remove_item(BUTTON_TYPE type);
    void set_menu_width(int width);
    void set_robot(RobotSprite* robot);
private:
    Menu* _menu;
    Vector<MenuItem*> _menu_items;
    RobotSprite* _robot;
    int _menu_width;
};
