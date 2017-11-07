#ifndef TYPES_H
#define TYPES_H

#include <Eigen/Dense>
#include <vector>
namespace mtao{ 
    template <typename T, int A, int B>
        using Matrix = Eigen::Matrix<T,A,B>;

    template <typename T, int A> using SquareMatrix = Matrix<T,A,A>;
    template <typename T, int D> using Vector = Matrix<T,D,1>;
    template <typename T, int D> using RowVector = Matrix<T,1,D>;

    template <typename T> using VectorX = Vector<T,Eigen::Dynamic>;
    template <typename T> using RowVectorX = RowVector<T,Eigen::Dynamic>;

    template <typename T> using Vector2 = Vector<T,2>;
    template <typename T> using RowVector2 = RowVector<T,2>;

    template <typename T> using Vector3 = Vector<T,3>;
    template <typename T> using RowVector3 = RowVector<T,3>;

    template <typename T, int D> using ColVectors = Matrix<T,D, Eigen::Dynamic>;
    template <typename T, int D> using RowVectors = Matrix<T,Eigen::Dynamic, D>;


    using Mat3f = Matrix3<float> ;
    using Mat2f = Matrix2<float> ;
    using Vec3f = Vector3<float> ;
    using Vec2f = Vector2<float> ;
    using MatXf = MatrixX<float> ;
    using VecXf = VectorX<float> ;

    using Mat3d = Matrix3<double> ;
    using Mat2d = Matrix2<double> ;
    using Vec3d = Vector3<double> ;
    using Vec2d = Vector2<double> ;
    using MatXd = MatrixX<double> ;
    using VecXd = VectorX<double> ;

    using Vec2i = Vector2<int>;
    using Vec3i = Vector3<int>;

    //types packaged for a given embedded dimensions
    template <typename T, int D>
        struct embedded_types {
            using Vector = Vector<T,D>;
            using SquareMatrix = SquareMatrix<T,D>;
            using ColVectors = ColVectors<T,D>;
        };

    template <typename T>
        struct scalar_types {
            typedef Eigen::Matrix<T,2,1> Vec2;
            typedef Eigen::Matrix<T,3,1> Vec3;
            typedef Eigen::Matrix<T,Eigen::Dynamic,1> VecX;
            typedef Eigen::Matrix<T,2,2> Mat2;
            typedef Eigen::Matrix<T,3,3> Mat3;
            typedef Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> MatX;
        };
    template <int Dim>
        struct dim_types {
            typedef Eigen::Matrix<double,Dim,1> Vecf;
            typedef Eigen::Matrix<float,Dim,1> Vecd;
            typedef Eigen::Matrix<int,Dim,1> Veci;
            typedef Eigen::Matrix<double,Dim,Dim> Matf;
            typedef Eigen::Matrix<float,Dim,Dim> Matd;
            typedef mtao::coord<Dim> Coord;
        };
    template <typename T, int Dim>
        struct numerical_types {
            typedef Eigen::Matrix<T,Dim,1> Vec;
            typedef Eigen::Matrix<T,Dim,Dim> Mat;
        };

    namespace internal {
        template <typename T>
            struct allocator_selector {
                using type = std::allocator<T>;
            };

        template <typename T, int R, int C>
            struct allocator_selector<Eigen::Matrix<T,R,C>> {
                using type = Eigen::aligned_allocator<Eigen::Matrix<T,R,C>>;
            };
    }

    template <typename T>
        using allocator = typename internal::allocator_selector<T>::type;

    template <typename T, typename Allocator = mtao::allocator<T>>
        using vector = std::vector<T,Allocator>;

    template <typename T>
        struct scalar_type {
            using type = T;
        };

    template <typename T, int R, int C>
        struct scalar_type<Eigen::Matrix<T,R,C>> {
            using type = T;
        };

    template <typename T>
    using scalar_type_t = typename scalar_type<T>::type;



}
#endif
