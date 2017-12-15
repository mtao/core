#include <iostream>
#include <GLFW/glfw3.h>
#include <map>
#include <set>
#include <functional>
#include <bitset>
#include <sstream>
#include "mtao/hotkey_manager.hpp"




int main() {
    mtao::HotkeyManager hotkeys;
    hotkeys.add([]() {
            std::cout << "K pressed!" << std::endl;
            },"tracks K presses" , GLFW_KEY_K);

    int count = 0;
    int shift_count = 0;

    hotkeys.add([&]() {
            std::cout << "Space pressed " << ++count << " times!" << std::endl;
            }, "Tracks spaces pressed", GLFW_KEY_SPACE);
    hotkeys.add([&]() {
            std::cout << "Shift Space pressed " << ++shift_count << " times!" << std::endl;
            }, "Tracks shift spaces press", GLFW_KEY_SPACE, GLFW_MOD_SHIFT);

    std::cout << hotkeys.info() << std::endl;
    hotkeys.press(GLFW_KEY_SPACE);
    hotkeys.press(GLFW_KEY_SPACE);
    hotkeys.press(GLFW_KEY_SPACE);
    hotkeys.press(GLFW_KEY_SPACE);
    hotkeys.press(GLFW_KEY_SPACE);
    hotkeys.press(GLFW_KEY_K);
    hotkeys.press(GLFW_KEY_K);
    hotkeys.press(GLFW_KEY_K);
    hotkeys.press(GLFW_KEY_K);
    hotkeys.press(GLFW_KEY_SPACE, GLFW_MOD_SHIFT);
    hotkeys.press(GLFW_KEY_SPACE, GLFW_MOD_SHIFT);
    hotkeys.press(GLFW_KEY_SPACE, GLFW_MOD_SHIFT);
    hotkeys.press(GLFW_KEY_SPACE, GLFW_MOD_SHIFT);
    hotkeys.press(GLFW_KEY_SPACE, GLFW_MOD_SHIFT);
    hotkeys.press(GLFW_KEY_SPACE, GLFW_MOD_SHIFT);
    hotkeys.press(GLFW_KEY_SPACE, GLFW_MOD_SHIFT);
    hotkeys.press(GLFW_KEY_K);
    hotkeys.press(GLFW_KEY_K);

    return 0;
}
