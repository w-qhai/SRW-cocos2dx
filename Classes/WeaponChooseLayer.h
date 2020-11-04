#pragma once

#include "cocos2d.h"
#include "RobotSprite.h"
#include "constants.h"
#include "ui/CocosGUI.h"
#include <queue>
#include <vector>
#include <stack>
using namespace cocos2d;
using namespace ui;
class WeaponChooseLayer : public Layer
{
public:
    WeaponChooseLayer();
    ~WeaponChooseLayer();

    CREATE_FUNC(WeaponChooseLayer);

    bool init() override;
};

