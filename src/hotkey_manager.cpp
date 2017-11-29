#include "hotkey_manager.hpp"
#include <bitset>
#include "logging/logger.hpp"
using namespace mtao::logging;
namespace mtao {
    HotkeyManager::Entry::Entry(const std::function<void(void)>& f, const std::string& d,int k, int m, int a)
        : key(k), modifiers(m), action(a), func(f), description(d) {}

    std::optional<int> HotkeyManager::Entry::active(int k, int m, int a) const {
        //m & modifiers is == modifiers only if m has at least the modifier bits active

        if(k == key && a == action && ((modifiers & m)==modifiers) ) {
            std::bitset<8*sizeof(int)> b(modifiers);
            return b.count();
        } else {
            return std::optional<int>();
        }
    }

    void HotkeyManager::press(int k, int m, int a) const {
        if(auto it = m_key_map.find(k); it != m_key_map.end()) {
            const std::function<void(void)>* fptr = nullptr;
            int count = -1;

            for(auto&& e: it->second) {
                if(auto o = e.active(k,m,a); o) {
                    if(*o > count) {
                        fptr = &e.func;
                        count = *o;
                    }
                }
            }
            if(fptr) {
                (*fptr)();
            }
        }
    }

    std::map<int,std::string> make_key_map() {
        //http://www.glfw.org/docs/latest/group__keys.html
        std::map<int,std::string> m;
        m[-1] = "Unknown";
        for(int i = 33; i < 127; ++i) {
            m[i] = char(i);
        }
        m[32] = "Space";
        m[256] = "Esc";
        static const char* special_keys[] = {
            "Enter",   //257
            "Tab",   
            "Bksp",
            "Ins",
            "Del",
            "Right",   
            "Left",   
            "Down",   
            "Up",   
            "PG_UP",   
            "PG_DOWN",   
            "Home",   
            "End",   //269
        };
        for(int i = 0; i < 269-257+1; ++i) {
            m[i+257] = special_keys[i];
        }
        static const char* special_keys2[] = {
            "Capslock",   //280
            "Scrolllock",   
            "Numlock",   
            "Print_screen",   
            "Pause",   };//Pause 284
        for(int i = 0; i < 284-280+1; ++i) {
            m[i+280] = special_keys2[i];
        }
        for(int i = 0; i < 25; ++i) {
            std::stringstream ss;
            ss << "F" << (i+1);
            m[290+i] = ss.str();
        }
        for(int i = 0; i < 10; ++i) {
            std::stringstream ss;
            ss << "KP" << (i);
            m[320+i] = ss.str();
        }
        //TODO: add keypad_decimal and up

        return m;
    }

    HotkeyManager::Entry::operator std::string() const {
        std::stringstream ss;
        const static std::map<int,std::string> key_map = make_key_map();

        if(auto it = key_map.find(key); it != key_map.end()) {
            ss << it->second;
        } else {
            ss << "Unknown[" << key << "]";
        }
        if(modifiers) {
            ss << "{mods:";
            if(modifiers & GLFW_MOD_ALT) {
                ss << "[alt]";
            }
            if(modifiers & GLFW_MOD_CONTROL) {
                ss << "[ctrl]";
            }
            if(modifiers & GLFW_MOD_SHIFT) {
                ss << "[shift]";
            }
            if(modifiers & GLFW_MOD_SUPER) {
                ss << "[super]";
            }
            ss << "}";
        }
        if(action == GLFW_PRESS) {
            ss << "{press}";
        } else if(action == GLFW_REPEAT) {
            ss << "{repeat}";
        } else if(action == GLFW_RELEASE) {
            ss << "{release}";
        }
        ss << ": " << description;

        return ss.str();
    }

    void HotkeyManager::add(const std::function<void(void)>& f, const std::string& d, int key, int modifiers, int action) {
        auto& km = m_key_map[key];
        km.emplace(f,d,key,modifiers,action);
    }

    std::string HotkeyManager::info() const {
        std::stringstream ss;
        for(auto&& pairs: m_key_map) {
            for(auto&& e: pairs.second) {
                ss << std::string(e) << std::endl;
            }
        }
        return ss.str();
    }
}
