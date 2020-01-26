#pragma once
#include <functional>
#include <cmath>
#include "mtao/types.hpp"


//Line search for armijo/backtrack + weak/strong wolfe conditions
//
//check out the make_wolfe_line_search type things at the bottom / the LineSearchInterface. They allow for users to pair up a stepsize tool and an energy relatively aesily
//these "stepsize tools" have to implement CRTP passthroughs for objective/gradient/descent_direction and implement their own versions of stepsize
namespace mtao::optimization {
    template <typename Derived_, typename Scalar_, typename Vector_>
        struct LineSearchBase {
            using Derived = Derived_;
            using Scalar = Scalar_;
            using Vector = Vector_;//vector is state and gradient type and has a dot prodcut operator


            Derived& derived() { return *static_cast<Derived*>(this); }
            const Derived& derived() const { return *static_cast<const Derived*>(this); }

            //interface to be filled out
            Scalar objective(const Vector& pos) const { return derived().objective(pos); }
            Vector gradient(const Vector& pos) const { return derived().gradient(pos); }
            Vector descent_direction(const Vector& pos) const { return derived().descent_direction(pos); }
            Scalar stepsize(int iteration = Scalar(0), Scalar alpha = Scalar(1)) const { return derived().stepsize(iteration,alpha); }

            Scalar objective() const { return derived().objective(m_position); }
            //Vector gradient() const { return derived().gradient(m_position); }
            Vector descent_direction() const { return derived().descent_direction(m_position); }

            //run takes a function that guesses alpha from an iteration.
            //if gradient and whatnot need to be captured it's the caller's responsibility to capture it in the alpha function
            int run(const std::function<Scalar(int)>& alpha = [](int) -> Scalar { return Scalar(1.0); });

            void set_epsilon(Scalar v) { m_epsilon = v; }
            void set_position(Vector v) { 
                m_position = std::move(v); 
                update_gradient();
                initialized = true;
            }
            void update_gradient() { 
                m_gradient = gradient(m_position); 
            }


            const Vector& position() const { return m_position; }
            const Vector& gradient() const { 
                return m_gradient; 
            }
            Scalar epsilon() const { return m_epsilon; }

            bool converged() const { return m_gradient.norm() < m_epsilon; }
            //bool converged() const { return m_gradient.template lpNorm<Eigen::Infinity>() < m_epsilon; }
            private:
            bool initialized = false;
            Vector m_position;
            Vector m_gradient;
            Scalar m_epsilon = Scalar(1e-4);


        };
    template <typename Derived_, typename Scalar_, typename Vector_>
        struct BacktrackingLineSearchBase: public LineSearchBase<BacktrackingLineSearchBase<Derived_, Scalar_, Vector_>, Scalar_, Vector_> {
            using Derived = Derived_;
            using Scalar = Scalar_;
            using Vector = Vector_;//vector is state and gradient type and has a dot prodcut operator
            using Base = LineSearchBase<BacktrackingLineSearchBase<Derived_, Scalar_, Vector_>, Scalar_, Vector_>;
            using Base::gradient;
            using Base::position;
            using Base::objective;
            using Base::descent_direction;
            Derived& derived() { return *static_cast<Derived*>(this); }
            const Derived& derived() const { return *static_cast<const Derived*>(this); }

            Scalar objective(const Vector& pos) const { return derived().objective(pos); }
            Vector gradient(const Vector& pos) const { return derived().gradient(pos); }
            Vector descent_direction(const Vector& pos) const { return derived().descent_direction(pos); }
            Scalar stepsize(int iteration = Scalar(0), Scalar alpha = Scalar(1)) const;
            private:
            Scalar m_tau = Scalar(.5);
            Scalar m_c = Scalar(.5);
        };

    template <typename Derived_, typename Scalar_, typename Vector_>
        struct WolfeLineSearchBase: public LineSearchBase<
                                    WolfeLineSearchBase<Derived_
                                    , Scalar_
                                    , Vector_>
                                    , Scalar_
                                    , Vector_> {
                                        using Derived = Derived_;
                                        using Scalar = Scalar_;
                                        using Vector = Vector_;//vector is state and gradient type and has a dot prodcut operator
                                        using Base = LineSearchBase<WolfeLineSearchBase<Derived_, Scalar_, Vector_>, Scalar_, Vector_>;
                                        using Base::gradient;
                                        using Base::position;
                                        using Base::objective;
                                        using Base::descent_direction;

                                        Derived& derived() { return *static_cast<Derived*>(this); }
                                        const Derived& derived() const { return *static_cast<const Derived*>(this); }

                                        Scalar objective(const Vector& pos) const { return derived().objective(pos); }
                                        Vector descent_direction(const Vector& pos) const { return derived().descent_direction(pos); }
                                        Vector gradient(const Vector& pos) const { return derived().gradient(pos); }
                                        Scalar stepsize(int iteration = Scalar(0), Scalar alpha = Scalar(1)) const;

                                        void set_strong() { strong = true; }
                                        void set_weak() { strong = false; }
                                        void set_strong(bool b) { strong = b; }
                                        private:
                                        Scalar m_c1 = Scalar(1e-4);
                                        Scalar m_c2 = Scalar(.9);
                                        Scalar m_tau = Scalar(.5);
                                        bool strong = false;
                                    };

    template <typename Derived, typename Scalar, typename Vector>
        int LineSearchBase<Derived, Scalar, Vector>::run(const std::function<Scalar(int)>& alpha_func) {
            if(!initialized) {
                //TODO: what warning do  iwant here?
                return -1;
            }
            int k = 0;
            do {
                Scalar alpha_guess = alpha_func(k);
                Scalar alpha = stepsize(k,alpha_guess);
                set_position(position() + alpha * descent_direction());
                k++;
                //std::cout << k << ")" << objective() << "|" << gradient().template lpNorm<-1>() << std::endl;
            } while( !converged() );
            return k;

        }
    template <typename Derived, typename Scalar, typename Vector>
        auto BacktrackingLineSearchBase<Derived, Scalar, Vector>::stepsize(int iteration, Scalar alpha) const -> Scalar {
            Vector cur_dd = descent_direction();

            Scalar m = gradient().dot(cur_dd);
            Scalar t = -m_c * m;
            Scalar cur_obj = objective();

            int j = 0;

            //set in loop before end of first iter
            bool condition;
            do {
                Scalar alpha_j = alpha * std::pow<Scalar>(m_tau,j++);
                Vector newpos = position() + alpha_j * cur_dd;
                Scalar new_obj = objective(newpos);

                condition =  cur_obj - new_obj >= alpha_j * t;
            } while( !(condition) );

            return alpha * std::pow<Scalar>(m_tau,j-1);

        }
    template <typename Derived, typename Scalar, typename Vector>
        auto WolfeLineSearchBase<Derived, Scalar, Vector>::stepsize(int iteration, Scalar alpha) const -> Scalar {
            assert(0 < m_c1);
            assert(m_c1 < m_c2);
            assert(m_c2 < 1);

            int j = 0;

            Vector cur_dd = descent_direction();

            Scalar m = gradient().dot(cur_dd);
            Scalar cur_obj = objective();
            Scalar t = -m_c1 * m;
            Scalar curv = -m_c2 * m;

            bool condition;
            do {
                Scalar alpha_j = alpha * std::pow<Scalar>(m_tau,j++);
                Vector new_pos = position() + alpha_j * descent_direction();
                Scalar new_obj = objective(new_pos);

                bool armijo = new_obj - cur_obj <= alpha_j * t;
                bool curvature;
                if(strong) {
                    curvature = std::abs(cur_dd.dot(gradient(new_pos))) <= std::abs(m_c2 * m);
                } else {
                    curvature = -cur_dd.dot(gradient(new_pos)) <= curv;
                }
                condition = armijo && curvature;
            } while( !(condition ) );

            return alpha * std::pow<Scalar>(m_tau,j-1);

        }

    template <template<typename, typename, typename> typename LSType, typename Interface>
        struct LineSearchInterface: public LSType<LineSearchInterface<LSType, Interface>, typename Interface::Scalar, typename Interface::Vector> {
            using Scalar = typename Interface::Scalar;
            using Vector = typename Interface::Vector;
            using Base = LSType<LineSearchInterface<LSType, Interface>, Scalar,Vector>;
            using Base::gradient;
            using Base::position;
            using Base::objective;
            using Base::descent_direction;
            LineSearchInterface(const Interface& i): interface(i) {}
            const Interface& interface;
            Scalar objective(const Vector& pos) const { return interface.objective(pos); }
            Vector gradient(const Vector& pos) const { return interface.gradient(pos); }
            Vector descent_direction(const Vector& pos) const { return interface.descent_direction(pos); }
        };
    template <typename Func>
        auto make_backtracking_line_search(Func&& f) {
            static_assert(std::is_lvalue_reference_v<Func>);//make sure that the container isnt set to disappear
            using F  = std::decay_t<Func>;
            return LineSearchInterface<
                BacktrackingLineSearchBase
                ,F
                // , typename F::Scalar, typename F::Vector
                >(f);
        }
    template <typename Func>
        auto make_wolfe_line_search(Func&& f) {
            static_assert(std::is_lvalue_reference_v<Func>);//make sure that the container isnt set to disappear
            using F  = std::decay_t<Func>;
            return LineSearchInterface<
                WolfeLineSearchBase
                ,F
                // , typename F::Scalar, typename F::Vector
                >(f);
        }
}
