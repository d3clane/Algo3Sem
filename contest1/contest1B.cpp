#include <cassert>
#include <cstdint>
#include <iostream>

template <uint32_t mod>
struct ModuloRingUint64 {
  uint64_t value;

  ModuloRingUint64(uint64_t value = 0) : value(value) {}

  ModuloRingUint64 operator*(const ModuloRingUint64& other) const {
    return (value * other.value) % mod;
  }

  ModuloRingUint64 operator+(const ModuloRingUint64& other) const {
    return (value + other.value) % mod;
  }

  ModuloRingUint64& operator+=(const ModuloRingUint64& other) {
    return *this = (*this + other) % mod;
  }

  ModuloRingUint64 operator-(const ModuloRingUint64& other) const {
    return (value - other.value) % mod;
  }

  operator uint64_t() { return value; }
};

template <typename T, size_t size>
struct Matrix {
  T matrix[size][size];

  Matrix() {
    for (size_t i = 0; i < size; ++i) {
      for (size_t j = 0; j < size; ++j) {
        matrix[i][j] = 0;
      }

      matrix[i][i] = 1;
    }
  }

  Matrix(uint64_t matrix[size][size]) {
    for (size_t i = 0; i < size; ++i) {
      for (size_t j = 0; j < size; ++j) {
        this->matrix[i][j] = matrix[i][j];
      }
    }
  }
};

template <typename T, size_t size>
Matrix<T, size> operator*(const Matrix<T, size>& m1,
                          const Matrix<T, size>& m2) {
  Matrix<T, size> res;

  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; ++j) {
      res.matrix[i][j] = 0;
      for (size_t k = 0; k < size; ++k) {
        res.matrix[i][j] += m1.matrix[i][k] * m2.matrix[k][j];
      }
    }
  }

  return res;
}

template <typename T>
T pow(const T value, const uint64_t n) {
  T result{};

  uint64_t power = n;
  T val = value;

  while (power != 0) {
    if (power & 1) {
      result = result * val;
    }

    val = val * val;
    power >>= 1;
  }

  return result;
}

uint64_t getAnswer(uint64_t n) {
  if (n == 1) return 1;

  const uint32_t mod = 1000003;
  const size_t matrixSize = 5;

  using ringType = ModuloRingUint64<mod>;

  Matrix<ringType, matrixSize> matrix(
      (uint64_t[matrixSize][matrixSize]){{1, 1, 1, 1, 1},
                                         {1, 0, 0, 0, 0},
                                         {0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0}});

  Matrix<ringType, matrixSize> resultMatrix = pow(matrix, n - 1);

  return resultMatrix.matrix[0][0];
}

int main() {
  uint64_t n = 0;
  std::cin >> n;

  uint64_t ans = getAnswer(n);

  std::cout << ans << "\n";
}
