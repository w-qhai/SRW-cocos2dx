#pragma once
namespace constants {
    extern double scale;
    extern int block_size;
}

namespace param_types {
    struct ClickRobot {
        void* who;
        float pos_x;
        float pos_y;
    };
};
