#include "mtao/opengl/Monitor.h"
Monitor::Monitor(GLFWmonitor *m) : monitor(m) {}
Monitor Monitor::getPrimary() {
    return Monitor{ glfwGetPrimaryMonitor() };
}
std::vector<Monitor> Monitor::getAll() {
    int count = 0;
    GLFWmonitor **ms = glfwGetMonitors(&count);

    std::vector<Monitor> ret(count);
    std::copy(ms, ms + count, ret.begin());
    return ret;
}


const GLFWvidmode &Monitor::getVideoMode() {
    return *glfwGetVideoMode(monitor);
}
std::vector<GLFWvidmode> Monitor::getVideoModes() {
    int count = 0;
    const GLFWvidmode *ms = glfwGetVideoModes(monitor, &count);

    std::vector<GLFWvidmode> ret(count);
    std::copy(ms, ms + count, ret.begin());
    return ret;
}

std::array<int, 2> Monitor::getPos() const {
    std::array<int, 2> p;
    glfwGetMonitorPos(monitor, &p[0], &p[1]);
    return p;
}
std::array<int, 2> Monitor::getPhysicalSize() const {
    std::array<int, 2> p;
    glfwGetMonitorPhysicalSize(monitor, &p[0], &p[1]);
    return p;
}
std::string Monitor::getName() const {
    return glfwGetMonitorName(monitor);
}
