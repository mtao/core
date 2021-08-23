#pragma once

#include <mtao/types.h>
#include <numbers>

template<typename T>
mtao::ColVectors<T, 3> enright_velocities(const mtao::ColVectors<T, 3> &P) {
    mtao::ColVectors<T, 3> V(P.rows(), P.cols());
    for (int i = 0; i < P.cols(); ++i) {
        auto v = V.col(i);
        auto p = P.col(i);
        const auto &x = p.x();
        const auto &y = p.y();
        const auto &z = p.z();
        T s2x = std::sin(2 * std::numbers::pi_v<T> * x);
        T s2y = std::sin(2 * std::numbers::pi_v<T> * y);
        T s2z = std::sin(2 * std::numbers::pi_v<T> * z);
        v(0) = 2 * s2y * s2z * std::pow<T>(std::sin(std::numbers::pi_v<T> * x), 2);
        v(1) = -s2x * s2z * std::pow<T>(std::sin(std::numbers::pi_v<T> * y), 2);
        v(2) = -s2x * s2y * std::pow<T>(std::sin(std::numbers::pi_v<T> * z), 2);
    }
    return V;
}
template<typename T>
mtao::ColVectors<T, 3> enright_velocities(const mtao::ColVectors<T, 3> &P, T time) {
    if (time < .5) {
        return enright_velocities(P);
    } else {
        return -enright_velocities(P);
    }
}
