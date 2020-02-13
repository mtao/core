#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <array>
#include <string>

class Monitor {
    public:
        Monitor(GLFWmonitor* m = nullptr);
        static Monitor getPrimary();
        static std::vector<Monitor> getAll();

        bool operator==(const Monitor& o) const { return monitor == o.monitor; }
        bool operator!=(const Monitor& o) const { return monitor != o.monitor; }

        const GLFWvidmode& getVideoMode();
        std::vector<GLFWvidmode> getVideoModes();

        std::array<int,2> getPos() const;
        std::array<int,2> getPhysicalSize() const;
        std::string getName() const;


        GLFWmonitor* glfwMonitor() const { return monitor; }



    private:
        GLFWmonitor* monitor;
};
