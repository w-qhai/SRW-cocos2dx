#include "GameMapLayer.h"

GameMapLayer::GameMapLayer(int chapter) : _chapter(chapter) {

}

GameMapLayer::~GameMapLayer() {

}


bool GameMapLayer::init() {

    if (!Layer::init()) {
        return false;
    }

    _tmx_tiled_map = TMXTiledMap::create(__String::createWithFormat("TiledMap/Chapter_%d.tmx", _chapter)->_string);
    _tmx_tiled_map->setPosition(Vec2(0, 0));
    _tmx_tiled_map->setScale(constants::scale);
    _tmx_tiled_map->setAnchorPoint(Vec2(0, 0));
    this->addChild(_tmx_tiled_map);

    TMXLayer* tmx_layer = _tmx_tiled_map->getLayer("layer");

    RobotSprite* destiny_gundam = create_robot("ZGMF-X42S");
    destiny_gundam->pos = Vec2(10, 11);
    destiny_gundam->_mov = 6;
    destiny_gundam->setPosition(convert_to_tiled_map(destiny_gundam->pos) * constants::block_size * constants::scale);
    destiny_gundam->set_status(RobotSprite::Status::EMENY);
    enemy_robots.push_back(destiny_gundam);
    
    RobotSprite* freedom_gundam = create_robot("ZGMF-X20A");
    freedom_gundam->pos = Vec2(10, 13);
    freedom_gundam->_mov = 6;
    freedom_gundam->setPosition(convert_to_tiled_map(freedom_gundam->pos) * constants::block_size * constants::scale);
    freedom_gundam->set_status(RobotSprite::Status::PLAYER);
    player_robots.push_back(freedom_gundam);

    map_install_touch_listener();
    
    return true;
}

void GameMapLayer::map_install_touch_listener() {
    EventListenerTouchOneByOne* move_screen_touch_listener = EventListenerTouchOneByOne::create();
    move_screen_touch_listener->setSwallowTouches(false);

    move_screen_touch_listener->onTouchBegan = [this](Touch* touch, Event* event) {
        _last_point = touch->getLocation();
        _drag = false;
        return true;
    };

    move_screen_touch_listener->onTouchMoved = [this](Touch* touch, Event* event) {
        const Vec2 now_point = touch->getLocation();
        const float dx = _last_point.x - now_point.x;
        const float dy = _last_point.y - now_point.y;

        _last_point = now_point;
        this->setPosition(this->getPosition() - Vec2(dx, dy));
        _drag = true;
        if (_drag) {
            EventCustom event("mouse_move");
            _eventDispatcher->dispatchEvent(&event);
        }
    };

    move_screen_touch_listener->onTouchEnded = [this](Touch* touch, Event* event) {
        if (!_drag) {
            EventCustom event("click_empty");
            event.setUserData((void*)&(touch->getLocation()));
            _eventDispatcher->dispatchEvent(&event);
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(move_screen_touch_listener, this);
}

TMXTiledMap* GameMapLayer::get_tiled_map() {
    return _tmx_tiled_map;
}

Vec2 GameMapLayer::convert_to_tiled_map(const Vec2& pos) {
    return Vec2(pos.x, _tmx_tiled_map->getLayer("layer")->getLayerSize().height - pos.y);
}

RobotSprite* GameMapLayer::create_robot(std::string number) {
    RobotSprite* robot = RobotSprite::create();
    robot->setAnchorPoint(Vec2(0, 1));
    robot->setScale(constants::scale);
    robot->set_number(number);
    this->addChild(robot, 1);
    return robot;
}