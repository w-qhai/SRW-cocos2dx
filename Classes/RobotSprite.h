#pragma once

#include "cocos2d.h"
#include "constants.h"
using namespace cocos2d;

class RobotSprite : public Sprite
{
public:
    RobotSprite();
    ~RobotSprite();

    enum class Status {
        NONE    = 0B00000000,
        MOVED   = 0B00000001,
        EMENY   = 0B00000010,
        PLAYER  = 0B00000100
    };

    CREATE_FUNC(RobotSprite);
    bool init() override;
    void set_number(const std::string& number);
    void set_status(Status status);
    void unset_status(Status status);
    Status status();
    std::string number();

    Vec2 pos;
    int _mov;
private:
    void robot_install_touch_listener();

private:
    Status _status;
    std::string _number;
    bool _is_moving;
};

