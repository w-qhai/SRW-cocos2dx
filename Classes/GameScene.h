#pragma once

#include "cocos2d.h"
#include "WeaponChooseLayer.h"
#include "GameMapLayer.h"
#include "RobotSprite.h"
#include "GameMenu.h"
#include "constants.h"
#include <queue>
#include <vector>
#include <stack>
using namespace cocos2d;

class GameScene : public Scene
{
public:
    GameScene();
    ~GameScene();

    CREATE_FUNC(GameScene);
    bool init() override;
private:
    std::vector<std::vector<std::pair<int, Vec2>>> BFS(RobotSprite* robot, bool AI = false);
    void init_menu();
    void install_robot_command_listener();
    void install_scene_listener();
    bool moveto_by_tile(int x, int y, bool AI = false);
    void focuse_on(RobotSprite* robot);
    void load_map(int chapter);

    //
    void click_robot(EventCustom* event);
    void click_empty(EventCustom* event);
    void drag_screen(EventCustom* event);
    void robot_move(EventCustom* event);

    // 命令
    void attack(EventCustom* event);
    void stand_by(EventCustom* event);
    void new_round(EventCustom* event);
    void after_enemy_robot_moved(EventCustom* event);
private:
    enum class GameStatus {
        NORMAL,
        PREMOVE,
        MOVED
    } _game_status;

    GameMapLayer*   _game_map_layer;
    WeaponChooseLayer* _weapon_choose_layer;

    GameMenu*       _menu_game;
    GameMenu*       _menu_sys;
    RobotSprite*    _robot;
    std::vector<std::vector<int>> _mini_map;
    std::vector<std::vector<std::pair<int, Vec2>>> movable_area;
    TMXTiledMap* _tiled_map;
    TMXLayer* _layer;
    int _width;
    int _height;
};

