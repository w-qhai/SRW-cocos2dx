#include "RobotSprite.h"

RobotSprite::RobotSprite() {

}

bool RobotSprite::init() {

    if (!Sprite::init()) {
        return false;
    }
    robot_install_touch_listener();
    return true;
}

RobotSprite::~RobotSprite() {

}

void RobotSprite::robot_install_touch_listener() {
    EventListenerTouchOneByOne* click_robot_listener = EventListenerTouchOneByOne::create();
    click_robot_listener->setSwallowTouches(true);

    click_robot_listener->onTouchBegan = [this](Touch* touch, Event* event) {
        if (this->getBoundingBox().containsPoint(touch->getLocation() - this->getParent()->getPosition())) {
            EventCustom event("click_robot");
            param_types::ClickRobot param;

            param.who   = (void*)this;
            param.pos_x = touch->getLocation().x;
            param.pos_y = touch->getLocation().y;

            event.setUserData((void*)&param);
            _eventDispatcher->dispatchEvent(&event);
            return true;
        }
        else {
            return false;
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(click_robot_listener, this);
}

void RobotSprite::set_number(const std::string& number) {
    _number = number;
}

std::string RobotSprite::number() {
    return _number;
}

void RobotSprite::set_status(STATUS status) {
    switch (status)
    {
    case RobotSprite::STATUS::MOVED:
        this->setTexture("img/RobotAvatar/gray/" + this->number() + ".png");
        break;
    case RobotSprite::STATUS::EMENY:
        this->setTexture("img/RobotAvatar/red/" + this->number() + ".png");
        break;
    case RobotSprite::STATUS::PLAYER:
        this->setTexture("img/RobotAvatar/blue/" + this->number() + ".png");
        break;
    default:
        break;
    }
    _status = status;
}

RobotSprite::STATUS RobotSprite::status() {
    return _status;
}
