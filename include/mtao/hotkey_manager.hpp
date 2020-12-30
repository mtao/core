#pragma once
#include <GLFW/glfw3.h>
#include <map>
#include <set>
#include <functional>
#include <sstream>
#include <optional>

namespace mtao {
class HotkeyManager {
  public:
    struct Entry {
        int key = GLFW_KEY_UNKNOWN;
        int modifiers = 0;//GLFW_MOD_ALT, GLFW_MOD_CONTROL, GLFW_MOD_SHIFT, GLFW_MOD_SUPER
        int action = GLFW_PRESS;//GLFW_PRESS, GLFW_REPEAT, or GLFW_RELEASE
        //int scancode;
        std::function<void(void)> func;
        std::string description;


        std::optional<int> active(int k, int m, int a) const;//if active, returns the number of modifiers met
        Entry() = default;
        Entry(Entry &&) = default;
        Entry(const Entry &) = default;
        Entry(const std::function<void(void)> &f, const std::string &d, int k, int m = 0, int a = GLFW_PRESS);
        bool operator<(const Entry &e) const {
            return e.key < key
                   || (e.key == key && (e.modifiers < modifiers || (e.modifiers == modifiers && e.action < action)));
        }
        operator std::string() const;
    };
    void add(const std::function<void(void)> &f, const std::string &d, int key, int modifiers = 0, int action = GLFW_PRESS);
    void press(int key, int modifiers = 0, int action = GLFW_PRESS) const;

    std::string info() const;

  private:
    std::map<int, std::set<Entry>> m_key_map;
};
}// namespace mtao
