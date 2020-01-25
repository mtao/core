#pragma once
#include <functional>
#include <cmath>
#include "mtao/types.hpp"


//Line search for armijo/backtrack + weak/strong wolfe conditions
namespace mtao::optimization {
    template <typename Obj>
        struct traits {
            using Scalar = double;
            using Vector = Eigen::VectorXd;
        };
    template <typename Derived_>
        struct LineSearchBase {
            using Derived = Derived_;
            using Traits = traits<Derived>;
            using Scalar = typename Traits::Scalar;
            using Vector = typename Traits::Vector;//vector is state and gradient type and has a dot prodcut operator


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
            void run(const std::function<Scalar(int)>& alpha = [](int) -> Scalar { return Scalar(1.0); });

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
            private:
            bool initialized = false;
            Vector m_position;
            Vector m_gradient;
            Scalar m_epsilon = Scalar(1e-4);


        };
    template <typename Derived_>
        struct BacktrackingLineSearchBase: public LineSearchBase<BacktrackingLineSearchBase<Derived_>> {
            using Derived = Derived_;
            using Traits = traits<Derived>;
            using Scalar = typename Traits::Scalar;
            using Vector = typename Traits::Vector;//vector is state and gradient type and has a dot prodcut operator
            using Base = LineSearchBase<BacktrackingLineSearchBase<Derived_>>;
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

    template <typename Derived_>
        struct WolfeLineSearchBase: public LineSearchBase<WolfeLineSearchBase<Derived_>> {
            using Derived = Derived_;
            using Traits = traits<Derived>;
            using Scalar = typename Traits::Scalar;
            using Vector = typename Traits::Vector;//vector is state and gradient type and has a dot prodcut operator
            using Base = LineSearchBase<WolfeLineSearchBase<Derived_>>;
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
            Scalar c1 = Scalar(1e-4);
            Scalar c2 = Scalar(.9);
            Scalar m_tau = Scalar(.5);
            bool strong = false;
        };

    template <typename Derived>
            void LineSearchBase<Derived>::run(const std::function<Scalar(int)>& alpha_func) {
                if(!initialized) {
                    //TODO: what warning do  iwant here?
                    return;
                }
                int k = 0;
                do {
                    Scalar alpha_guess = alpha_func(k);
                    Scalar alpha = stepsize(k,alpha_guess);
                    set_position(position() + alpha * descent_direction());
                    k++;
                } while( !converged() );

            }
    template <typename Derived>
        auto BacktrackingLineSearchBase<Derived>::stepsize(int iteration, Scalar alpha) const -> Scalar {
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
            } while( !condition );

            return alpha * std::pow<Scalar>(m_tau,j-1);

        }
        template <typename Derived>
        auto WolfeLineSearchBase<Derived>::stepsize(int iteration, Scalar alpha) const -> Scalar {
            assert(0 < c1);
            assert(c1 < c2);
            assert(c2 < 1);

            int j = 0;

            Vector cur_dd = descent_direction();

            Scalar m = gradient().dot(cur_dd);
            Scalar cur_obj = objective();
            Scalar t = -c1 * m;

            bool condition;
            do {
                Scalar alpha_j = alpha * std::pow<Scalar>(m_tau,j++);
                Vector new_pos = position() + alpha_j * descent_direction();
                Scalar new_obj = objective(new_pos);

                bool armijo = new_obj - cur_obj <= alpha_j * t;
                bool curvature;
                if(strong) {
                    curvature = std::abs(cur_dd.dot(gradient(new_pos))) <= std::abs(c2 * m);
                } else {
                    curvature = -cur_dd.dot(gradient(new_pos)) <= -c2 * m;
                }
                condition = armijo && curvature;
                j++;
            } while( !(condition ));

            return alpha * std::pow<Scalar>(m_tau,j-1);

        }

}
