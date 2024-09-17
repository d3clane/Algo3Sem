#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iostream>

template <uint32_t mod>
class ModuloRingUint64 {
  uint64_t value_;

 public:
  ModuloRingUint64(uint64_t initVal = 0)
      : value_((initVal % mod + mod) % mod) {}

  void value(uint64_t initVal) { value_ = (initVal % mod + mod) % mod; }
  uint64_t value() const { return value_; }

  operator uint64_t() const { return value_; }

  template <uint32_t modNew>
  friend ModuloRingUint64<modNew> operator*(const ModuloRingUint64<modNew>& a,
                                            const ModuloRingUint64<modNew>& b);

  template <uint32_t modNew>
  friend ModuloRingUint64<modNew> operator+(const ModuloRingUint64<modNew>& a,
                                            const ModuloRingUint64<modNew>& b);

  template <uint32_t modNew>
  friend ModuloRingUint64<modNew>& operator+=(
      ModuloRingUint64<modNew>& a, const ModuloRingUint64<modNew>& b);

  template <uint32_t modNew>
  friend ModuloRingUint64<modNew> operator-(const ModuloRingUint64<modNew>& a,
                                            const ModuloRingUint64<modNew>& b);
};

template <uint32_t mod>
ModuloRingUint64<mod> operator*(const ModuloRingUint64<mod>& a,
                                const ModuloRingUint64<mod>& b) {
  return (a.value_ * b.value_) % mod;
}

template <uint32_t mod>
ModuloRingUint64<mod> operator+(const ModuloRingUint64<mod>& a,
                                const ModuloRingUint64<mod>& b) {
  ModuloRingUint64<mod> result = a;
  return result += b;
}

template <uint32_t mod>
ModuloRingUint64<mod>& operator+=(ModuloRingUint64<mod>& a,
                                  const ModuloRingUint64<mod>& b) {
  a.value_ = a.value_ + b.value_;
  a.value_ = a.value_ >= mod ? a.value_ - mod
                             : a.value_;  // probably it's faster than mod
  return a;
}

template <uint32_t mod>
ModuloRingUint64<mod> operator-(const ModuloRingUint64<mod>& a,
                                const ModuloRingUint64<mod>& b) {
  ModuloRingUint64<mod> result = a;
  result.value_ = result.value_ - b.value_;
  result.value_ = result.value_ < 0
                      ? result.value_ + mod
                      : result.value_;  // probably it's faster than mod

  return result;
}

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

  Matrix(std::initializer_list<std::initializer_list<T> > initMatrix) {
    assert(initMatrix.size() == size);

    size_t matrixRow = 0;
    for (auto& row : initMatrix) {
      assert(row.size() == size);

      std::copy(row.begin(), row.end(), matrix[matrixRow]);

      ++matrixRow;
    }
  }
};

template <typename T, size_t size>
Matrix<T, size> operator*(const Matrix<T, size>& a, const Matrix<T, size>& b) {
  Matrix<T, size> res;

  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; ++j) {
      res.matrix[i][j] = 0;
      for (size_t k = 0; k < size; ++k) {
        res.matrix[i][j] += a.matrix[i][k] * b.matrix[k][j];
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
    if (power % 2 == 1) {
      result = result * val;
    }

    val = val * val;
    power /= 2;
  }

  return result;
}

uint64_t getAnswer(uint64_t n) {
  if (n == 1) {
    return 1;
  }

  const uint32_t mod = 1000003;
  const size_t matrixSize = 5;

  using RingType = ModuloRingUint64<mod>;

  Matrix<RingType, matrixSize> matrix({{1, 1, 1, 1, 1},
                                       {1, 0, 0, 0, 0},
                                       {0, 1, 0, 0, 0},
                                       {0, 0, 1, 0, 0},
                                       {0, 0, 0, 1, 0}});

  Matrix<RingType, matrixSize> resultMatrix = pow(matrix, n - 1);

  return resultMatrix.matrix[0][0];
}

int main() {
  uint64_t n = 0;
  std::cin >> n;

  uint64_t ans = getAnswer(n);

  std::cout << ans << "\n";
}
