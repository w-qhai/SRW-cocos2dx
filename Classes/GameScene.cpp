#include "GameScene.h"

static int current = 0;
GameScene::GameScene() {

}

GameScene::~GameScene() {

}

bool GameScene::init() {
    
    if (!Scene::init()) {
        return false;
    }
    _weapon_choose_layer = WeaponChooseLayer::create();
    _weapon_choose_layer->setVisible(false);
    this->addChild(_weapon_choose_layer);

    _game_status = GameStatus::NORMAL;
    _robot = nullptr;
    load_map(1);
    //初始化可移动区域
    movable_area = std::vector<std::vector<std::pair<int, Vec2>>>(_height, std::vector<std::pair<int, Vec2>>(_width));
    
    // 初始化小地图
    _mini_map = std::vector<std::vector<int>>(_height, std::vector<int>(_width));
    std::fill(_mini_map.begin(), _mini_map.end(), std::vector<int>(_width, 0));

    for (int i = 0; i < _game_map_layer->player_robots.size(); i++) {
        RobotSprite* r = _game_map_layer->player_robots[i];
        _mini_map[r->pos.y][r->pos.x] = constants::PLAYER;
    }

    for (int i = 0; i < _game_map_layer->enemy_robots.size(); i++) {
        RobotSprite* r = _game_map_layer->enemy_robots[i];
        _mini_map[r->pos.y][r->pos.x] = constants::ENEMY;
    }

    focuse_on(_game_map_layer->player_robots[0]);
    init_menu();
    install_robot_command_listener();
    install_scene_listener();

    _weapon_choose_layer->setVisible(true);
    _weapon_choose_layer->setZOrder(_game_map_layer->getZOrder() + 1);

    return true;
}

void GameScene::init_menu() {

    _menu_game = GameMenu::create();
    _menu_game->setVisible(false);
    _menu_game->set_menu_width(100);
    _menu_game->add_item(GameMenu::ButtonType::MOVE);
    _menu_game->add_item(GameMenu::ButtonType::STAUTS);
    _menu_game->add_item(GameMenu::ButtonType::ATTACK);
    this->addChild(_menu_game, 0);
     
    _menu_sys = GameMenu::create();
    _menu_sys->setVisible(false);
    _menu_sys->set_menu_width(150);
    _menu_sys->add_item(GameMenu::ButtonType::NEW_ROUND);
    _menu_sys->add_item(GameMenu::ButtonType::TEAM);
    this->addChild(_menu_sys, 0);

    // 点击机器人
    EventListenerCustom* event_click_robot = EventListenerCustom::create("click_robot",
        [&](EventCustom* event) {
            this->click_robot(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_click_robot, this);

    // 点击空地
    EventListenerCustom* event_click_empty = EventListenerCustom::create("click_empty",
        [&](EventCustom* event) {
            this->click_empty(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_click_empty, this);

    // 鼠标移动 拖动屏幕，隐藏所有菜单
    EventListenerCustom* event_drag_screen = EventListenerCustom::create("drag_screen",
        [&](EventCustom* event) {
            this->drag_screen(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_drag_screen, this);
}

void GameScene::install_robot_command_listener() {
    // 移动
    EventListenerCustom* event_robot_move = EventListenerCustom::create("robot_move",
        [&](EventCustom* event) {
            this->robot_move(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_robot_move, this);

    EventListenerCustom* event_attack = EventListenerCustom::create("attack",
        [&](EventCustom* event) {
            this->attack(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_attack, this);

    // 待命
    EventListenerCustom* event_stand_by = EventListenerCustom::create("stand_by",
        [&](EventCustom* event) {
            this->stand_by(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_stand_by, this);

    // 回合结束
    EventListenerCustom* event_new_round = EventListenerCustom::create("new_round",
        [=](EventCustom* event) {
            this->new_round(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_new_round, this);
}

void GameScene::install_scene_listener() {
    // 回合结束，敌方机体开始移动
    EventListenerCustom* event_after_enemy_robot_moved = EventListenerCustom::create("after_enemy_robot_moved",
        [=](EventCustom* event) {
            this->after_enemy_robot_moved(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event_after_enemy_robot_moved, this);
}

// 寻找可以移动区域
std::vector<std::vector<std::pair<int, Vec2>>> GameScene::BFS(RobotSprite* robot, bool AI) {

    /*
    * 根据当前选中的机器人的位置，类型，机动，寻找可移动区域
    */

    //左 下 右 上
    static int dir[4][2] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };

    short mov = AI ? 998 : robot->_mov;
    Vec2 pos = robot->pos;
    std::string type = "land";

    std::queue<Vec2> path;
    path.push(pos);

    std::fill(movable_area.begin(), movable_area.end(),
        std::vector<std::pair<int, Vec2>>(_width, std::make_pair(-1000, Vec2(0, 0))));
    movable_area[pos.y][pos.x] = { mov, pos };


    while (!path.empty()) {
        Vec2 from = path.front();
        path.pop();

        for (int i = 0; i < 4; i++) {
            Vec2 to;
            to.x = from.x + dir[i][0];
            to.y = from.y + dir[i][1];

            // 目标点首先在地图内
            if (to.y > 0 && to.y < _height && to.x > 0 && to.x < _width) {
                const int gid = _layer->getTileGIDAt(Vec2(to.x, to.y));
                Value value = _tiled_map->getPropertiesForGID(gid);
                int cost = value.asValueMap().at(type).asInt();

                if (int(robot->status()) & int(RobotSprite::Status::PLAYER)) {
                    if (_mini_map[to.y][to.x] == constants::ENEMY) {
                        cost = 999;
                    }
                }
                else if (int(robot->status()) & int(RobotSprite::Status::ENEMY)) {
                    if (_mini_map[to.y][to.x]) {
                        cost = 999;
                        if (movable_area[from.y][from.x].first - cost > movable_area[to.y][to.x].first) {
                            movable_area[to.y][to.x] = { movable_area[from.y][from.x].first - cost, from };
                        }
                    }
                }

                if (movable_area[from.y][from.x].first - cost > movable_area[to.y][to.x].first) {
                    path.push(to);
                    movable_area[to.y][to.x] = { movable_area[from.y][from.x].first - cost, from };
                }
            }
        }
    }
    return movable_area;
}

// 机体移动动画
bool GameScene::moveto_by_tile(int x, int y, bool AI) {
    Vec2 now(x, y);
    std::stack<Vec2> path;

    // 去掉到达不到的点
    while (AI && movable_area[now.y][now.x].second != now) {
        if (movable_area[now.y][now.x].first < constants::mov_max - _robot->_mov) {
            now = movable_area[now.y][now.x].second;
            continue;
        }
        break;
    }

    // 获取路径
    while (movable_area[now.y][now.x].second != now) {
        path.push(now);
        now = movable_area[now.y][now.x].second;
    }

    const double length = path.size();
    // 如果不动返回false
    if (path.size() < 1) {
        return false;
    }

    // 移动动作
    Vector<FiniteTimeAction*> actions;

    actions.pushBack(MoveBy::create(0.5, Vec2(0, constants::block_size * constants::scale / 2))); // 启动动作
    while (!path.empty()) {
        MoveTo* move_seq = MoveTo::create(path.size() / length / 5 + 0.03, // 移动速度逐渐加快
            _game_map_layer->convert_to_tiled_map(Vec2(path.top().x, path.top().y - 0.5)) * constants::block_size * constants::scale);
        path.pop();
        actions.pushBack(move_seq);
    }
    actions.pushBack(MoveBy::create(0.5, Vec2(0, -constants::block_size * constants::scale / 2))); // 降落动作
    // 结束时 变灰色
    actions.pushBack(CallFunc::create([=]() 
        {
            if (AI) {       
                stand_by(nullptr);
                _mini_map[_robot->pos.y][_robot->pos.x] = constants::ENEMY;
                current++;
                EventCustom event("after_enemy_robot_moved");
                _eventDispatcher->dispatchEvent(&event);
            }
            else {
                _menu_game->clear_items();
                _menu_game->setVisible(true);
                _menu_game->add_item(GameMenu::ButtonType::STAND_BY);
                _game_status = GameStatus::MOVED;
            }
            // 聚焦机器人
            focuse_on(_robot);
            _menu_game->setPosition(_robot->getPosition() + Vec2(constants::block_size * constants::scale, 0) - (this->getPosition() - _game_map_layer->getPosition()));
        }));

    Sequence* seq = Sequence::create(actions);
    _robot->runAction(seq);
    return true;
}

// 聚焦某个机器人
void GameScene::focuse_on(RobotSprite* robot) {
    _game_map_layer->setPosition(
        -((robot->getPosition() - this->getPosition()) - this->getContentSize() / 2)
    );
}

// 加载地图
void GameScene::load_map(int chapter) {
    _game_map_layer = GameMapLayer::create();
    _tiled_map = this->_game_map_layer->get_tiled_map();
    _layer  = _tiled_map->getLayer("layer");
    _width  = _layer->getLayerSize().width;
    _height = _layer->getLayerSize().height;
    this->addChild(_game_map_layer);
}

void GameScene::after_enemy_robot_moved(EventCustom* event) {
    // 循环遍历敌方机体进行移动
    if (current < _game_map_layer->enemy_robots.size()) {
        _robot = _game_map_layer->enemy_robots[current];
        focuse_on(_robot);
        movable_area = BFS(_robot, true);

        // 循环遍历目标
        for (int i = 0; i < _game_map_layer->player_robots.size(); i++) {
            RobotSprite* to = _game_map_layer->player_robots[i];
            if (moveto_by_tile(movable_area[int(to->pos.y)][int(to->pos.x)].second.x, movable_area[int(to->pos.y)][int(to->pos.x)].second.y, true)) {
                break;
            }
        }
    }
    else { // 敌方机体移动完
        for (int i = 0; i < _game_map_layer->player_robots.size(); i++) {
            _game_map_layer->player_robots[i]->unset_status(RobotSprite::Status::MOVED);
        }

        for (int i = 0; i < _game_map_layer->enemy_robots.size(); i++) {
            _game_map_layer->enemy_robots[i]->unset_status(RobotSprite::Status::MOVED);
        }
    }
}

void GameScene::new_round(EventCustom* event) {
    _menu_sys->setVisible(false);
    current = 0;
    EventCustom e("after_enemy_robot_moved");
    _eventDispatcher->dispatchEvent(&e);
}

void GameScene::stand_by(EventCustom* event) {
    _mini_map[_robot->pos.y][_robot->pos.x] = 0;
    _robot->pos = _game_map_layer->convert_to_tiled_map(Vec2(
        int(_robot->getPosition().x) / constants::block_size / int(constants::scale),
        int(_robot->getPosition().y) / constants::block_size / int(constants::scale)));
    _mini_map[_robot->pos.y][_robot->pos.x] = constants::PLAYER;

    _robot->set_status(RobotSprite::Status::MOVED);
    _menu_game->setVisible(false);
    _game_status = GameStatus::NORMAL;
}

void GameScene::robot_move(EventCustom* event) {
    _game_status = GameStatus::PREMOVE;

    _menu_sys->setVisible(false);
    _menu_game->setVisible(false);

    movable_area = BFS(_robot);

    for (int h = 0; h < _height; h++) {
        for (int w = 0; w < _width; w++) {
            Sprite* sprite = _layer->getTileAt(Vec2(w, h));
            if (movable_area[h][w].first < 0) {
                sprite->setVisible(false);
            }
        }
    }
}

void GameScene::drag_screen(EventCustom* event) {
    _menu_sys->setVisible(false);
    _menu_game->setVisible(false);
    if (_game_status == GameStatus::MOVED) {
        _robot->setPosition(_game_map_layer->convert_to_tiled_map(_robot->pos) * constants::block_size * constants::scale);
        _game_status = GameStatus::NORMAL;
    }
}

void GameScene::click_empty(EventCustom* event) {
    std::tuple<RobotSprite*, Vec2> param = *reinterpret_cast<std::tuple<RobotSprite*, Vec2>*>(event->getUserData());

    // 把点击坐标转化成tile坐标
    Vec2 tpos = std::get<1>(param) - _game_map_layer->getPosition();
    Vec2 will_to;
    switch (_game_status)
    {
    case GameScene::GameStatus::NORMAL:
        _menu_sys->setVisible(!_menu_sys->isVisible());
        _menu_game->setVisible(false);
        _menu_sys->setPosition(std::get<1>(param));
        break;
    case GameScene::GameStatus::PREMOVE:
        tpos.x = int(tpos.x) / constants::block_size / int(constants::scale);
        tpos.y = int(tpos.y) / constants::block_size / int(constants::scale) + 1;
        will_to = _game_map_layer->convert_to_tiled_map(tpos);

        if (movable_area[will_to.y][will_to.x].first >= 0) {
            moveto_by_tile(will_to.x, will_to.y);
        }
        break;
    case GameScene::GameStatus::MOVED:
        // 未点待命 返回原地
        _menu_game->setVisible(false);
        _robot->setPosition(_game_map_layer->convert_to_tiled_map(_robot->pos) * constants::block_size * constants::scale);
        break;
    default:
        break;
    }

    for (int h = 0; h < _height; h++) {
        for (int w = 0; w < _width; w++) {
            Sprite* sprite = _layer->getTileAt(Vec2(w, h));
            sprite->setVisible(true);
        }
    }
    _game_status = GameStatus::NORMAL;
}

void GameScene::click_robot(EventCustom* event) {
    std::tuple<RobotSprite*, Vec2> param = *reinterpret_cast<std::tuple<RobotSprite*, Vec2>*>(event->getUserData());
    RobotSprite* robot = std::get<0>(param);
    Vec2 pos = std::get<1>(param);
    /*
        判断机器人状态
    */
    _menu_game->clear_items();
    _menu_game->add_item(GameMenu::ButtonType::MOVE);
    _menu_game->add_item(GameMenu::ButtonType::STAUTS);
    _menu_game->add_item(GameMenu::ButtonType::ATTACK);
    switch (_game_status)
    {
    case GameScene::GameStatus::NORMAL:
        // 玩家机器人
        if (robot->status() == RobotSprite::Status::PLAYER) {
            _robot = robot;
            _menu_game->setVisible(!_menu_game->isVisible());
            _menu_sys->setVisible(false);
            _menu_game->set_robot(_robot);
            _menu_game->setPosition(pos.x + constants::scale * constants::block_size, pos.y + constants::scale * constants::block_size);
        }
        break;
    case GameScene::GameStatus::PREMOVE:
        break;
    case GameScene::GameStatus::MOVED:
        // attack
        break;
    default:
        break;
    }
}

void GameScene::attack(EventCustom* event) {
    _weapon_choose_layer->setVisible(true);
    _weapon_choose_layer->setZOrder(_game_map_layer->getZOrder() + 1);

}
