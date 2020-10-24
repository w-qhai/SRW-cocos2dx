#pragma once

#include "cocos2d.h"
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
    std::vector<std::vector<std::pair<int, Vec2>>> BFS();
    void init_menu();
    void install_robot_command_listener();
    void moveto_by_tile(int x, int y);
private:
    enum class GAME_STATUS {
        NORMAL,
        MOVING
    } _game_status;
    GameMapLayer*   _game_map_layer;
    GameMenu*       _menu_game;
    GameMenu*       _menu_sys;
    RobotSprite*    _robot;
    std::vector<std::vector<std::pair<int, Vec2>>> movable_area;
    
};

