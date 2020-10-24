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

void RobotSprite::set_name(const String& name) {
    _name = name;
}

String RobotSprite::name() {
    return _name;
}

