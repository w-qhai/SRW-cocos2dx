#pragma once

#include "cocos2d.h"
#include "constants.h"
using namespace cocos2d;

class RobotSprite : public Sprite
{
public:
    RobotSprite();
    ~RobotSprite();

    CREATE_FUNC(RobotSprite);
    bool init() override;
    void set_name(const String& name);
    String name();

    Vec2 pos;
    int _mov;
private:
    void robot_install_touch_listener();

private:
    String _name;
    bool _is_moving;
};

