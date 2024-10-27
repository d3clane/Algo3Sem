#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>

class ModuloRingUint64 {
 public:
  ModuloRingUint64(uint64_t initVal, uint32_t mod)
      : value_(initVal % mod), mod_(mod) {}

  void value(uint64_t initVal) { value_ = initVal % mod_; }
  uint64_t value() const { return value_; }

  operator uint64_t() const { return value_; }

  ModuloRingUint64& operator*=(const ModuloRingUint64& other);
  ModuloRingUint64& operator+=(const ModuloRingUint64& other);

 private:
  uint64_t value_;
  uint32_t mod_;
};

struct DivisionByTwoResult;
class DecimalBigInt {
  std::vector<uint8_t> digits_;

 public:
  DecimalBigInt() = default;
  DecimalBigInt(const std::string& value);

  DecimalBigInt& operator--();
  DivisionByTwoResult divideByTwo() const;

  std::string convertToBinary() const;

  bool isZero() const;
};

struct DivisionByTwoResult {
  DecimalBigInt result;
  int reminder;
};

DecimalBigInt::DecimalBigInt(const std::string& value) {
  digits_.resize(value.size());

  for (size_t i = 0; i < value.size(); ++i) {
    digits_[i] = static_cast<uint8_t>(value[i]) - static_cast<uint8_t>('0');
  }

  std::reverse(digits_.begin(), digits_.end());
}

bool DecimalBigInt::isZero() const {
  for (size_t i = 0; i < digits_.size(); ++i) {
    if (digits_[i] != 0) {
      return false;
    }
  }

  return true;
}

DivisionByTwoResult DecimalBigInt::divideByTwo() const {
  DecimalBigInt result;
  int carry = 0;
  int len = digits_.size();
  for (ssize_t i = len - 1; i >= 0; --i) {
    int value = (carry * 10) + digits_[i];
    result.digits_.push_back(value / 2);
    carry = value % 2;
  }

  std::reverse(result.digits_.begin(), result.digits_.end());
  return {result, carry};
}

DecimalBigInt& DecimalBigInt::operator--() {
  for (size_t i = 0; i < digits_.size(); ++i) {
    assert(digits_[i] <= 9);

    if (digits_[i] != 0) {
      digits_[i]--;
      break;
    }

    digits_[i] = 9;
  }

  return *this;
}

std::string DecimalBigInt::convertToBinary() const {
  std::string binary;

  DecimalBigInt decimal = *this;

  while (!decimal.isZero()) {
    auto [afterDivision, reminder] = decimal.divideByTwo();
    binary += std::to_string(reminder);
    decimal = afterDivision;
  }

  return binary;
}

ModuloRingUint64 operator*(const ModuloRingUint64& first,
                           const ModuloRingUint64& second) {
  ModuloRingUint64 result = first;
  return result *= second;
}

ModuloRingUint64& ModuloRingUint64::operator*=(const ModuloRingUint64& other) {
  value_ = (value_ * other.value_) % mod_;
  return *this;
}

ModuloRingUint64 operator+(const ModuloRingUint64& a,
                           const ModuloRingUint64& b) {
  ModuloRingUint64 result = a;
  return result += b;
}

ModuloRingUint64& ModuloRingUint64::operator+=(const ModuloRingUint64& other) {
  value_ = (value_ + other.value_) % mod_;
  return *this;
}

template <typename T>
class Matrix {
 private:
  struct MatrixLine {
    std::vector<T>& matrixLine;

    T& operator[](size_t pos) { return matrixLine[pos]; }
  };

 public:
  Matrix(size_t size = 0, T initVal = T{});

  Matrix<T>& operator*=(const Matrix<T>& other);

  MatrixLine operator[](size_t pos) { return MatrixLine{matrix_[pos]}; }

  size_t size() const { return matrix_.size(); }

  template <typename U>
  friend Matrix<U> operator*(const Matrix<U>& first, const Matrix<U>& second);

 private:
  std::vector<std::vector<T> > matrix_;
};

template <typename T>
Matrix<T>::Matrix(size_t size, T initVal) {
  matrix_.resize(size, std::vector(size, initVal));
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& first, const Matrix<T>& second) {
  Matrix<T> res = first;

  size_t size = res.size();
  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; ++j) {
      res.matrix_[i][j] = first.matrix_[i][0] * second.matrix_[0][j];
      for (size_t k = 1; k < size; ++k) {
        res.matrix_[i][j] += first.matrix_[i][k] * second.matrix_[k][j];
      }
    }
  }

  return res;
}

template <typename T>
Matrix<T>& Matrix<T>::operator*=(const Matrix<T>& other) {
  return *this = *this * other;
}

template <typename T>
T pow(const T& value, const T& neutralValue, const std::string& binaryPower) {
  T result = neutralValue;

  T val = value;

  for (size_t i = 0; i < binaryPower.size(); ++i) {
    assert(binaryPower[i] == '1' || binaryPower[i] == '0');

    if (binaryPower[i] == '1') {
      result *= val;
    }

    val *= val;
  }

  return result;
}

Matrix<ModuloRingUint64> FillRecurrentMatrix(const uint32_t width,
                                             const uint32_t mod) {
  uint32_t nProfiles = (1 << width);

  Matrix<ModuloRingUint64> profileStep(nProfiles, ModuloRingUint64{1, mod});

  for (uint32_t profile1 = 0; profile1 < nProfiles; profile1++) {
    for (uint32_t profile2 = 0; profile2 < nProfiles; profile2++) {
      for (uint32_t mask = 1; mask < nProfiles / 2; mask *= 2) {
        uint32_t squareTopLeft = (profile1 & mask) ? 1 : 0;
        uint32_t squareTopRight = (profile2 & mask) ? 1 : 0;
        uint32_t newMask = mask * 2;
        uint32_t squareBottomLeft = (profile1 & newMask) ? 1 : 0;
        uint32_t squareBottomRight = (profile2 & newMask) ? 1 : 0;

        if (squareTopLeft == squareTopRight &&
            squareTopRight == squareBottomLeft &&
            squareBottomLeft == squareBottomRight) {
          profileStep[profile1][profile2] = ModuloRingUint64{0, mod};
          break;
        }
      }
    }
  }

  return profileStep;
}

Matrix<ModuloRingUint64> createIdentityMatrix(const size_t size,
                                              const uint32_t mod) {
  Matrix<ModuloRingUint64> res(size, ModuloRingUint64{0, mod});
  for (size_t i = 0; i < size; ++i) {
    res[i][i] = {1, mod};
  }

  return res;
}

uint64_t getInfluencingOptionsAmount(const DecimalBigInt& height,
                                     const uint32_t width, const uint32_t mod) {
  using RingType = ModuloRingUint64;

  Matrix<RingType> recurrentMatrix = FillRecurrentMatrix(width, mod);

  DecimalBigInt newHeight = height;
  --newHeight;

  if (newHeight.isZero()) {
    return (1 << width) % mod;
  }

  auto identityMatrix = createIdentityMatrix(recurrentMatrix.size(), mod);

  Matrix<RingType> resultMatrix =
      pow(recurrentMatrix, identityMatrix, newHeight.convertToBinary());

  size_t matrixSize = resultMatrix.size();

  RingType answer{0, mod};

  for (size_t i = 0; i < matrixSize; ++i) {
    for (size_t j = 0; j < matrixSize; ++j) {
      answer += resultMatrix[i][j];
    }
  }

  return answer.value();
}

int main() {
  std::string n;
  uint32_t m = 0, mod = 0;
  std::cin >> n;
  std::cin >> m >> mod;

  uint64_t influencingOptionsAmount = getInfluencingOptionsAmount(n, m, mod);

  std::cout << influencingOptionsAmount << "\n";

  return 0;
}
