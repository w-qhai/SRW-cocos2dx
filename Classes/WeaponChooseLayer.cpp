#include "WeaponChooseLayer.h"

WeaponChooseLayer::WeaponChooseLayer() {

}

WeaponChooseLayer::~WeaponChooseLayer() {
   
}

bool WeaponChooseLayer::init() {

    if (!Layer::init()) {
        return false;
    }
    
    Layout* wapper = Layout::create();
    wapper->setLayoutType(Layout::Type::HORIZONTAL);
    wapper->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    wapper->setBackGroundColor(Color3B::GRAY);
    wapper->setContentSize(this->getContentSize());

    Layout* left = Layout::create();
    left->setLayoutType(Layout::Type::VERTICAL);
    left->setContentSize(this->getContentSize() / 2);
    wapper->addChild(left);

    Layout* right = Layout::create();
    right->setLayoutType(Layout::Type::VERTICAL);
    right->setContentSize(this->getContentSize() / 2);
    wapper->addChild(right);
    
    LinearLayoutParameter* center_horizonal = LinearLayoutParameter::create();
    center_horizonal->setGravity(LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL);
    center_horizonal->setMargin(Margin(10, 10, 10, 10));
    ImageView* image = ImageView::create("img/robot/ZGMF-X20A.png");
    image->setLayoutParameter(center_horizonal);
    left->addChild(image);
    {
        LinearLayoutParameter* param = LinearLayoutParameter::create();
        param->setMargin(Margin(10, 10, 10, 10));

        ui::Text* text = Text::create();
        text->setString("");
        text->setFontSize(30);
        text->setLayoutParameter(param);
        right->addChild(text);
    }

    {
        LinearLayoutParameter* param = LinearLayoutParameter::create();
        param->setMargin(Margin(10, 5, 10, 5));

        ui::Text* text = Text::create();
        text->setString(String::createWithFormat(
            "\
强袭自由高达\n\
LV %2d    类型 %s\n\
移动 %2d\n\
精神    %3d / %3d\
", 46, "空", 6, 255, 255)->_string);
        text->setFontSize(45);
        text->setLayoutParameter(param);
        right->addChild(text);
    }
    this->addChild(wapper);

    return true;
}
