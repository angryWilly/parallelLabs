#include <iostream>
#include <cstdlib>
#include <chrono>
#include <omp.h>

double** InitializeMatrix(const int n, const int m)
{
    auto M = new double* [n];

    for (int i = 0; i < n; i++)
    {
        M[i] = new double[m];

        for (int j = 0; j < m; j++)
        {
            M[i][j] = rand() % 30 - 15;
        }
    }

    return M;
}

void DestroyMatrix(double** M, const int n)
{
    for (int i = 0; i < n; i++)
    {
        delete[] M[i];
    }

    delete[] M;
}

void ParallelMatrixMultiply(double** A, const int n1, const int m1, double** B, const int m2, double** C, const int threadsCount)
{
    omp_set_num_threads(threadsCount);
#pragma omp parallel for
    for (int i = 0; i < n1; i++)
    {
        for (int j = 0; j < m2; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < m1; k++)
            {
                C[i][j] += (A[i][k] * B[k][j]);
            }
        }
    }
}

void SequentialMatrixMultiply(double** A, const int n1, const int m1, double** B, const int m2, double** C)
{
    for (int i = 0; i < n1; i++)
    {
        for (int j = 0; j < m2; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < m1; k++)
            {
                C[i][j] += (A[i][k] * B[k][j]);
            }
        }
    }
}

bool IsSolutionsEqual(double** Cseq, double** Cpar, int n, int m)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (Cseq[i][j] != Cpar[i][j])
            {
                return false;
            }
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    using namespace std::chrono;
    srand(time(nullptr));
    constexpr int n1 = 1500, m1 = 550, n2 = 550, m2 = 1300;
    constexpr int threads = 6;

    auto A = InitializeMatrix(n1, m1);
    auto B = InitializeMatrix(n2, m2);
    auto Cseq = InitializeMatrix(n1, m2);
    auto Cpar = InitializeMatrix(n1, m2);

    auto start = system_clock::now();
    SequentialMatrixMultiply(A, n1, m1, B, m2, Cseq);
    auto end = system_clock::now();
    const auto seqElapsed = duration_cast<milliseconds>(end - start).count();

    start = system_clock::now();
    ParallelMatrixMultiply(A, n1, m1, B, m2, Cpar, threads);
    end = system_clock::now();
    const auto parElapsed = duration_cast<milliseconds>(end - start).count();

    auto correct = IsSolutionsEqual(Cseq, Cpar, n1, m2);

    std::cout << "Parallel solution correct : " << std::format("{}", correct) << "\n";
    std::cout << "Sequential elapsed : " << seqElapsed << "ms\n";
    std::cout << "Parallel elapsed : " << parElapsed << "ms\n";
    std::cout << "Speed up : " << static_cast<float>(seqElapsed) / parElapsed;

    DestroyMatrix(A, n1);
    DestroyMatrix(B, n2);
    DestroyMatrix(Cseq, n1);
    DestroyMatrix(Cpar, n1);

    return 0;
}
