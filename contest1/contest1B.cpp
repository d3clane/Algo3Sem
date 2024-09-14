#include <cassert>
#include <iostream>

template <int size, unsigned long long MOD>
struct Matrix {
  unsigned long long matrix[size][size];

  Matrix() {
    for (size_t i = 0; i < size; ++i) {
      for (size_t j = 0; j < size; ++j) matrix[i][j] = 0;

      matrix[i][i] = 1;
    }
  }

  Matrix(unsigned long long matrix[size][size]) {
    for (size_t i = 0; i < size; ++i)
      for (size_t j = 0; j < size; ++j) this->matrix[i][j] = matrix[i][j];
  }
};

template <int size, unsigned long long MOD>
Matrix<size, MOD> operator*(const Matrix<size, MOD>& m1,
                            const Matrix<size, MOD>& m2) {
  Matrix<size, MOD> tmp;

  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; ++j) {
      tmp.matrix[i][j] = 0;
      for (size_t k = 0; k < size; ++k) {
        tmp.matrix[i][j] += m1.matrix[i][k] * m2.matrix[k][j];
        tmp.matrix[i][j] %= MOD;
      }
    }
  }

  return tmp;
}

template <typename T>
T binPow(T a, unsigned long long n) {
  if (n == 1) return a;

  if (n % 2 == 0) {
    T tmp = binPow(a, n / 2);
    return tmp * tmp;
  }

  return binPow(a, n - 1) * a;
}

unsigned long long getAnswer(unsigned long long n) {
  if (n == 1) return 1;

  const unsigned long long MOD = 1000003;
  const size_t matrixSize = 5;

  unsigned long long matrixArr[matrixSize][matrixSize] = {{1, 1, 1, 1, 1},
                                                          {1, 0, 0, 0, 0},
                                                          {0, 1, 0, 0, 0},
                                                          {0, 0, 1, 0, 0},
                                                          {0, 0, 0, 1, 0}};

  Matrix<matrixSize, MOD> matrix(matrixArr);

  Matrix<matrixSize, MOD> resultMatrix = binPow(matrix, n - 1);
  return resultMatrix.matrix[0][0];
}

int main() {
  unsigned long long n = 0;
  std::cin >> n;

  unsigned long long ans = getAnswer(n);

  std::cout << ans << "\n";
}
