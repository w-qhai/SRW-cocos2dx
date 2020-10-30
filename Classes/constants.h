#pragma once
namespace constants {
    extern double scale;
    extern int block_size;
    extern int mov_max;
    extern int PLAYER;
    extern int ENEMY;
}

namespace param_types {
    struct ClickRobot {
        void* who;
        float pos_x;
        float pos_y;
    };
};
