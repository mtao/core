#pragma once
#include <memory>

namespace mtao {
    template <typename T, class Deleter=std::default_delete<T>>
    class copyable_unique_ptr: public std::unique_ptr<T,Deleter> {
        using Base = typename std::unique_ptr<T,Deleter>;
        using Base::Base;
        using Base::release;
        using Base::reset;
        using Base::swap;
        using Base::get;
        using Base::get_deleter;
        using Base::operator bool;
        using Base::operator*;
        using Base::operator->;
        copyable_unique_ptr(const copyable_unique_ptr& o): Base(new T(*o)) {} 
        copyable_unique_ptr& operator=(const copyable_unique_ptr& o) {
            this->reset(new T(*o));
        }
    };
    template <typename T, class Deleter=std::default_delete<T>>
        auto make_copyable_unique_ptr(Args&&... args) {
            return copyable_unique_ptr(new T(std::forward<Args>(args)...));
        }

}
