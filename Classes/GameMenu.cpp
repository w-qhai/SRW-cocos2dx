#include "GameMenu.h"

GameMenu::GameMenu() {
    
}

bool GameMenu::init() {

    if (!Sprite::init()) {
        return false;
    }

    this->setTexture("/img/box.png");
    this->setOpacity(255);
    this->setAnchorPoint(Vec2(0, 1));
    
    _menu_items.pushBack(MenuItemFont::create("移动",
        [=](Ref* sender) {
            EventCustom event("robot_move");
            _eventDispatcher->dispatchEvent(&event);
        }));

    _menu_items.pushBack(MenuItemFont::create("状态",
        [&](Ref* sender) {
            
        }));

    _menu_items.pushBack(MenuItemFont::create("攻击",
        [&](Ref* sender) {

        }));

    _menu_items.pushBack(MenuItemFont::create("待命",
        [&](Ref* sender) {
            EventCustom event("stand_by");
            _eventDispatcher->dispatchEvent(&event);
        }));

    _menu_items.pushBack(MenuItemFont::create("回合结束",
        [&](Ref* sender) {
            EventCustom event("new_round");
            _eventDispatcher->dispatchEvent(&event);
        }));
    _menu_items.pushBack(MenuItemFont::create("部队表",
        [&](Ref* sender) {

        }));

    _menu = Menu::create();
    _menu->setPosition(this->_position);
    //_menu->setSwallowsTouches(false);
    this->addChild(_menu);

    return true;
}

GameMenu::~GameMenu() {

}

void GameMenu::clear_items() {
    _menu->removeAllChildren();
}

void GameMenu::add_item(ButtonType type) {
    _menu->addChild(_menu_items.at(static_cast<int>(type)));
    this->setContentSize(Size(_menu_width, _menu->getChildrenCount() * 50));
    _menu_items.at(static_cast<int>(type))->setPosition(_menu_width / 2, (_menu->getChildrenCount() * 50 - 25));
}

void GameMenu::remove_item(ButtonType type) {
    _menu->removeChild(_menu_items.at(static_cast<int>(type)), false);
}

void GameMenu::set_menu_width(int width) {
    _menu_width = width;
}

void GameMenu::set_robot(RobotSprite* robot) {
    _robot = robot;
}
