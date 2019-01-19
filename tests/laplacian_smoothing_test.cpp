#include <mtao/types.hpp>
#include <iostream>
#include<Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
#include <mtao/logging/timer.hpp>
#include <mtao/eigen/mic0_preconditioner.hpp>
using Mat = mtao::MatrixX<float>;

int N=200;
float permeability = 100.0;
float timestep = 1000.0;

Eigen::SparseMatrix<float> grid_boundary(int ni, int nj, bool dirichlet_boundary) {
    int system_size = ni*nj;
    int usize = (ni+1) * nj;
    int vsize = (nj+1) * ni;
    int offset = usize;

    auto u_ind = [ni,nj](int i, int j) {
        return i + j*(ni + 1);
    };

    auto v_ind = [ni,nj,offset](int i, int j) {
        return i + j*ni + offset;
    };

    std::vector< Eigen::Triplet<float> > triplets;

    Eigen::SparseMatrix<float> D(usize + vsize, system_size);
    for (int j = 0; j < nj ; ++j) {
        for (int i = 0; i < ni ; ++i) {
            int index = i + ni*j;
            if(!(dirichlet_boundary && i == 0)) {
                triplets.push_back(Eigen::Triplet<float>( u_ind(i,j), index, 1.0));
            }
            if(!(dirichlet_boundary && i == ni-1)) {
            triplets.push_back(Eigen::Triplet<float>( u_ind(i+1,j), index, -1.0));
            }


            if(!(dirichlet_boundary && j == 0)) {
            triplets.push_back(Eigen::Triplet<float>(  v_ind(i,j), index, 1.0));
            }
            if(!(dirichlet_boundary && j == ni-1)) {
                triplets.push_back(Eigen::Triplet<float>(  v_ind(i,j+1), index, -1.0));
            }

        }
    }
    D.setFromTriplets(triplets.begin(), triplets.end());
    D /= N;
    return D;
}




template <typename SolverType>
void run_test(SolverType& solver, const std::string& name, bool print = false) {
    Mat data = Mat::Random(N,N);
    Eigen::Map<Eigen::VectorXf> theta(data.data(),data.size());
    int size = N*N;
    auto B = grid_boundary(N,N,true);
    auto dirichlet_energy = [&]() -> double {
        return (B * theta).squaredNorm();
    };
    //size ~ dx * dy ~ dx^2
    Eigen::SparseMatrix<float> L = B.transpose() * B;
    Eigen::SparseMatrix<float> A(size,size);

    auto t = mtao::logging::timer(name + "-total");
    for(int i = 0; i < 1; ++i) {
        timestep = std::pow(10,i);
        A.setIdentity();
        A /= timestep;
        A += L;
        //auto t = mtao::logging::timer(name);
        solver.compute(A);
        for(int j = 0; j < 100; ++j) {
            data = Mat::Random(N,N);
            for(int i = 0; i < 100; ++i) {
                theta = solver.solve(theta);
                if(print) {
                    std::cout << dirichlet_energy() << std::endl;
                }
            }
        }
    }
}

int main() {
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> ldlt_solver;
    Eigen::ConjugateGradient<Eigen::SparseMatrix<float>, Eigen::Lower|Eigen::Upper> cg_solver;
    Eigen::ConjugateGradient<Eigen::SparseMatrix<float>, Eigen::Lower|Eigen::Upper, mtao::eigen::preconditioners::MIC0Preconditioner<float>> mic0pcg_solver;
    //run_test(ldlt_solver, "LDLT");
    run_test(cg_solver, "CG");
    run_test(mic0pcg_solver, "MIC0PCG");
    return 0;

    }
