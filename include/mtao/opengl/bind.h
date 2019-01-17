#ifndef OPENGL_BIND_H
#define OPENGL_BIND_H
#include <cstddef>
#include <utility>
#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#define __gl_h_
#else
#include <GL/gl.h>
#endif


namespace mtao { namespace opengl {

template <typename Obj>
struct bind_enabled {
    public:
        const Obj& derived() const {return *static_cast<const Obj*>(this);}
        Obj& derived() {return *static_cast<Obj*>(this);}

        template <typename Optional>
            static void bind(GLuint id, const Optional& optional) {
                if constexpr(!optional_exists()) {
                    Obj::bind(id);
                } else {
                    Obj::bind(id,optional);
                }
            }
        void bind() const {
            bind(id(),optional());
        }
        template <typename Optional>
            static void release(GLuint id, const Optional& optional) {
                if constexpr(!optional_exists()) {
                    Obj::release(id);
                } else {
                    Obj::release(id,optional);
                }
            }
        void release() const {
            release(id(),optional());
        }

        GLuint id() const { return derived().id(); }
        auto optional() const {
            if constexpr(optional_exists()) {
                return derived().optional();
            } else {
                return nullptr;
            }
        }
    private:
        template <typename U=Obj> constexpr static bool optional_exists() {return optional_exists_<U>(0);}
        //&U::optional has a reference in order to make it a ptr type so passing 0 works
        template <typename U> constexpr static bool optional_exists_(decltype(&U::optional)) {return true;}
        //C style varargs to let anything passed in works
        template <typename U> constexpr static bool optional_exists_(...) {return false;}

};


template <typename Bindable>
struct Binder {
    public:
        using optional_data_type = decltype(std::declval<Bindable>().optional());
        Binder(const Bindable& obj): m_id(obj.id()), m_optional(obj.optional()) {
            Bindable::bind(m_id, m_optional);
        }
        ~Binder() {
            Bindable::release(m_id,m_optional);
        }
    private:
        GLuint m_id;
        optional_data_type m_optional;
};


template <typename Obj>
auto make_binder(const Obj& o) { return Binder<bind_enabled<Obj>>(o); }

}}


#endif//OPENGL_BIND_H
