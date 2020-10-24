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
    //Vec2 convert_from_tiled_map(const Vec2& pos);

private:
    void map_install_touch_listener();

private:
    int             _chapter;           // which chapter
    TMXTiledMap*    _tmx_tiled_map;     // load map
    bool            _drag;
    Vec2            _last_point;        // record last click point position
    //std::vector<PlayerRobotAvatar*> _player_robots;
};

