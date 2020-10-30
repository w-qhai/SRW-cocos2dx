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


    RobotSprite* destiny_gundam1 = create_robot("ZGMF-X42S", Vec2(4, 18), RobotSprite::Status::ENEMY);
    RobotSprite* destiny_gundam2 = create_robot("ZGMF-X42S", Vec2(5, 19), RobotSprite::Status::ENEMY);
    RobotSprite* destiny_gundam3 = create_robot("ZGMF-X42S", Vec2(5, 18), RobotSprite::Status::ENEMY);
    
    RobotSprite* freedom_gundam1 = create_robot("ZGMF-X20A", Vec2(6, 15), RobotSprite::Status::PLAYER);
    RobotSprite* freedom_gundam2 = create_robot("ZGMF-X20A", Vec2(7, 15), RobotSprite::Status::PLAYER);
    
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
            EventCustom event("drag_screen");
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

RobotSprite* GameMapLayer::create_robot(std::string number, Vec2 pos, RobotSprite::Status status) {
    RobotSprite* robot = RobotSprite::create();
    robot->setAnchorPoint(Vec2(0, 1));
    robot->pos = pos;
    robot->setScale(constants::scale);
    robot->set_number(number);
    robot->setPosition(convert_to_tiled_map(robot->pos) * constants::block_size * constants::scale);
    robot->set_status(status);
    switch (status)
    {
    case RobotSprite::Status::NONE:
        break;
    case RobotSprite::Status::MOVED:
        break;
    case RobotSprite::Status::ENEMY:
        enemy_robots.push_back(robot);
        break;
    case RobotSprite::Status::PLAYER:
        player_robots.push_back(robot);
        break;
    default:
        break;
    }

    // get robot data from config
    robot->_mov = 7;
    this->addChild(robot, 1);
    return robot;
}