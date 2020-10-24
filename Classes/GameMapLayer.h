#pragma once
#include "cocos2d.h"
#include "constants.h"
#include "RobotSprite.h"
using namespace cocos2d;

class GameMapLayer : public Layer
{
public:
    friend class GameScene;
    GameMapLayer(int chapter = 1);
    ~GameMapLayer();

    CREATE_FUNC(GameMapLayer);
    bool init() override;
    TMXTiledMap* get_tiled_map();
    Vec2 convert_to_tiled_map(const Vec2& pos);
    RobotSprite* create_robot(std::string number);
    std::vector<RobotSprite*> player_robots;
    std::vector<RobotSprite*> enemy_robots;

private:
    void map_install_touch_listener();

private:
    int             _chapter;           // which chapter
    TMXTiledMap*    _tmx_tiled_map;     // load map
    bool            _drag;
    Vec2            _last_point;        // record last click point position
};

