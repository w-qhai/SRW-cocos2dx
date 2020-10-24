#include "GameScene.h"

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

bool GameScene::init() {
    
    if (!Scene::init()) {
        return false;
    }
    _game_status = GAME_STATUS::NORMAL;
    _robot = nullptr;
    _game_map_layer = GameMapLayer::create();
    this->addChild(_game_map_layer);

    TMXTiledMap* tiled_map = this->_game_map_layer->get_tiled_map();
    TMXLayer* layer = tiled_map->getLayer("layer");
    const int width = layer->getLayerSize().width;
    const int height = layer->getLayerSize().height;
    movable_area = std::vector<std::vector<std::pair<int, Vec2>>>(height, std::vector<std::pair<int, Vec2>>(width));

    init_menu();
    install_robot_command_listener();
    return true;
}

void GameScene::init_menu() {

    _menu_game = GameMenu::create();
    _menu_game->setVisible(false);
    _menu_game->set_menu_width(100);
    _menu_game->add_item(GameMenu::BUTTON_TYPE::MOVE);
    _menu_game->add_item(GameMenu::BUTTON_TYPE::STAUTS);
    _menu_game->add_item(GameMenu::BUTTON_TYPE::ATTACK);
    this->addChild(_menu_game, 0);
     
    /*
    * NORMAL模式：
    *   关闭系统菜单
    *   根据点击位置显示游戏菜单位置
    * MOVING模式：
    *   ...
    */
    EventListenerCustom* event_click_robot = EventListenerCustom::create("click_robot",
        [&](EventCustom* event) {
            const param_types::ClickRobot* param = static_cast<param_types::ClickRobot*>(event->getUserData());
            RobotSprite* robot = static_cast<RobotSprite*>(param->who);

            switch (_game_status)
            {
            case GameScene::GAME_STATUS::NORMAL:
                _robot = robot;
                _menu_game->setVisible(!_menu_game->isVisible());
                _menu_sys->setVisible(false);
                _menu_game->set_robot(_robot);
                _menu_game->setPosition(param->pos_x + constants::scale * constants::block_size, param->pos_y + constants::scale * constants::block_size);
                break;
            case GameScene::GAME_STATUS::MOVING:
                break;
            default:
                break;
            }
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_click_robot, this);

    _menu_sys = GameMenu::create();
    _menu_sys->setVisible(false);
    _menu_sys->set_menu_width(150);
    _menu_sys->add_item(GameMenu::BUTTON_TYPE::NEW_ROUND);
    _menu_sys->add_item(GameMenu::BUTTON_TYPE::TEAM);
    this->addChild(_menu_sys, 0);

    /*
    * NORMAL模式：
    *   隐藏游戏菜单
    *   根据点击位置设置菜单位置
    * MOVING模式:
    *   移动robot到点击位置
    *   恢复地图精灵
    *   设置模式为NORMAL
    */
    EventListenerCustom* event_click_empty = EventListenerCustom::create("click_empty",
        [&](EventCustom* event) {
            const Vec2* pos = static_cast<Vec2*>(event->getUserData());
            Vec2 tpos = *pos - _game_map_layer->getPosition();
            Vec2 will_to;
            switch (_game_status)
            {
            case GameScene::GAME_STATUS::NORMAL:
                _menu_sys->setVisible(!_menu_sys->isVisible());
                _menu_game->setVisible(false);
                _menu_sys->setPosition(pos->x, pos->y);
                break;
            case GameScene::GAME_STATUS::MOVING:
                tpos.x = int(tpos.x) / constants::block_size / int(constants::scale);
                tpos.y = int(tpos.y) / constants::block_size / int(constants::scale) + 1;
                will_to = _game_map_layer->convert_to_tiled_map(tpos);

                if (movable_area[will_to.y][will_to.x].first >= 0) {
                    moveto_by_tile(will_to.x, will_to.y);
                    _robot->pos = will_to;
                    //_robot->setPosition(_game_map_layer->convert_to_tiled_map(_robot->pos) * constants::block_size * constants::scale);
                }
                break;
            default:
                break;
            }

            TMXTiledMap* tiled_map = this->_game_map_layer->get_tiled_map();
            TMXLayer* layer = tiled_map->getLayer("layer");
            const int width = layer->getLayerSize().width;
            const int height = layer->getLayerSize().height;
            for (int h = 0; h < height; h++) {
                for (int w = 0; w < width; w++) {
                    Sprite* sprite = layer->getTileAt(Vec2(w, h));
                    sprite->setVisible(true);
                }
            }
            _game_status = GAME_STATUS::NORMAL;
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_click_empty, this);

    /*
    * 鼠标移动 拖动屏幕，隐藏所有菜单
    */
    EventListenerCustom* event_mouse_move = EventListenerCustom::create("mouse_move",
        [&](EventCustom* event) {
            _menu_sys->setVisible(false);
            _menu_game->setVisible(false);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_mouse_move, this);
}

void GameScene::install_robot_command_listener() {
    /*
    * 移动命令:
    *   robot移动，将不可移动位置隐藏(变灰色)
    */
    EventListenerCustom* event_robot_move = EventListenerCustom::create("robot_move",
        [&](EventCustom* event) {
            _game_status = GAME_STATUS::MOVING;

            _menu_sys->setVisible(false);
            _menu_game->setVisible(false);
            
            TMXTiledMap* tiled_map = this->_game_map_layer->get_tiled_map();
            TMXLayer* layer = tiled_map->getLayer("layer");
            const int width = layer->getLayerSize().width;
            const int height = layer->getLayerSize().height;

            movable_area = BFS();
            
            for (int h = 0; h < height; h++) {
                for (int w = 0; w < width; w++) {
                    Sprite* sprite = layer->getTileAt(Vec2(w, h));
                    if (movable_area[h][w].first >= 0) {
                        /*int gid = layer->getTileGIDAt(Vec2(w, h));
                        Value value = tiled_map->getPropertiesForGID(gid);
                        Label* label = Label::create();
                        label->setString(String::createWithFormat("%d %.0f %.0f", movable_area[h][w].first, movable_area[h][w].second.x, movable_area[h][w].second.y)->_string);
                        label->setPosition(sprite->getPosition());
                        label->setTextColor(Color4B::RED);
                        label->setAnchorPoint(Vec2(0, 0));
                        label->setScale(0.5);
                        tiled_map->addChild(label, 10);*/
                    }
                    else {
                        sprite->setVisible(false);
                    }     
                }
            }
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_robot_move, this);

}


std::vector<std::vector<std::pair<int, Vec2>>> GameScene::BFS() {

    /*
    * 根据当前选中的机器人的位置，类型，机动，寻找可移动区域
    */

    static int dir[4][2] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };

    short mov = _robot->_mov;
    Vec2 pos = _robot->pos;
    std::string type = "land";

    std::queue<Vec2> path;
    path.push(pos);

    TMXTiledMap* tiled_map = this->_game_map_layer->get_tiled_map();
    TMXLayer* layer = tiled_map->getLayer("layer");
    const int width = layer->getLayerSize().width;
    const int height = layer->getLayerSize().height;
    std::fill(movable_area.begin(), movable_area.end(), 
        std::vector<std::pair<int, Vec2>>(width, std::make_pair(-1, Vec2(0, 0))));
    movable_area[pos.y][pos.x] = { mov, pos };

    
    while (!path.empty()) {
        Vec2 from = path.front();
        path.pop();

        for (int i = 0; i < 4; i++) {
            Vec2 to;
            to.x = from.x + dir[i][0];
            to.y = from.y + dir[i][1];

            // 目标点首先在地图内
            if (to.y > 0 && to.y < height && to.x > 0 && to.x < width) {
                const int gid = layer->getTileGIDAt(Vec2(to.x, to.y));
                Value value = tiled_map->getPropertiesForGID(gid);
                const int cost = value.asValueMap().at(type).asInt();
                if (movable_area[from.y][from.x].first - cost > movable_area[to.y][to.x].first) {
                    path.push(to);
                    movable_area[to.y][to.x] = { movable_area[from.y][from.x].first - cost, from };
                }
            }
        }
    }
    return movable_area;
}

void GameScene::moveto_by_tile(int x, int y) {
    std::stack<Vec2> path;
    Vec2 now(x, y);
    while (movable_area[now.y][now.x].second != now) {
        path.push(now);
        now = movable_area[now.y][now.x].second;
    }
    const double length = path.size();

    Vector<FiniteTimeAction*> actions;
    actions.pushBack(MoveBy::create(0.5, Vec2(0, constants::block_size * constants::scale / 2))); // 启动动作
    while (!path.empty()) {
        MoveTo* move_seq = MoveTo::create(path.size() / length / 5 + 0.03, // 移动速度逐渐加快
            _game_map_layer->convert_to_tiled_map(Vec2(path.top().x, path.top().y - 0.5)) * constants::block_size * constants::scale);
        path.pop();
        actions.pushBack(move_seq);
    }
    actions.pushBack(MoveBy::create(0.5, Vec2(0, -constants::block_size * constants::scale / 2))); // 降落动作
    Sequence* seq = Sequence::create(actions);
    _robot->runAction(seq);
}
