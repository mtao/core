#ifndef COMPAT_H
#define COMPAT_H

#define USE_BOOST
#ifdef USE_BOOST
//#include <boost/array.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tr1/array.hpp>
#include <boost/type_traits.hpp>
#include <boost/bind.hpp>
namespace mtao {namespace compat {
    template <typename T, int D>
        using array = boost::array<T,D>;
    template <typename T>
        using tuple_size = std::tr1::tuple_size<T>;

    template <bool B, class T, class F>
        struct conditional {
            typedef T type;
        };
        template <class T, class F>
        struct conditional<false,T,F> {
            typedef F type;
        };
    template <class F, class... Args>
        auto bind(F f, Args... args){
        return boost::bind(f,args...);
        }
    /*
    namespace placeholders {
        using _1 = ::_1;
        using _2 = ::_2;
        using _3 = ::_3;
    }
    */
    /*
    template <int D, typename T, size_t N>
        using get = std::tr1::get<D,T,N>;
        */
}
}
#else

#include <array>
#include <tuple>
#include <type_traits>
namespace mtao {namespace compat {
    template <typename T, int D>
        using array = std::array<T,D>;
    template <typename T>
        using tuple_size = std::tuple_size<T>;
    template <bool B, class T, class F>
    using conditional = std::conditional<B,T,F>;
    
    template <class F, class... Args>
        auto bind(F f, Args... args){
        return std::bind(f,args...);
        }
    namespace placeholders {
        using _1 = std::placeholders::_1;
        using _2 = std::placeholders::_2;
        using _3 = std::placeholders::_3;
    }
    /*
    template <int D, typename T>
        using get = std::get<D,T>;
        */
}
}
#endif

#endif//COMPAT_H
