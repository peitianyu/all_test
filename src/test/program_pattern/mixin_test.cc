#include "core/tt_test.h"
#include <iostream>

// 定义一个可见性Mixin
class VisibilityMixin {
public:
    void show() {
        visible = true;
    }
    
    void hide() {
        visible = false;
    }

protected:
    bool visible = true;
};

// 定义一个禁用状态Mixin
class DisabledMixin {
public:
    void enable() {
        disabled = false;
    }
    
    void disable() {
        disabled = true;
    }

protected:
    bool disabled = false;
};

// 定义一个按钮类，并使用Mixin来添加功能
class Button : public VisibilityMixin, public DisabledMixin {
public:
    void click() {
        if (visible && !disabled) {
            std::cout << "Button clicked!\n";
        }
    }
};


// JUST_RUN_TEST(mixin, test)
TEST(mixin, test)
{
    Button button;
    button.click();
    button.show();
    button.click();
    button.disable();
    button.click();
}