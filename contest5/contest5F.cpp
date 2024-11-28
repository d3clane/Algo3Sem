#include <algorithm>
#include <iostream>
#include <vector>

std::vector<std::vector<bool>> readGraphAdjMatrix() {
  size_t n;
  std::cin >> n;

  std::vector<std::vector<bool>> graphAdjMatrix(n, std::vector<bool>(n, false));

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      int value = 0;
      std::cin >> value;
      if (value == 0) {
        graphAdjMatrix[i][j] = 0;
      } else {
        graphAdjMatrix[i][j] = 1;
      }
    }
  }

  return graphAdjMatrix;
}

std::vector<std::vector<bool>> findTransitiveClosure(
    const std::vector<std::vector<bool>>& graphAdjMatrix) {
  size_t n = graphAdjMatrix.size();

  std::vector<std::vector<bool>> closure = graphAdjMatrix;

  for (size_t k = 0; k < n; k++) {
    for (size_t i = 0; i < n; i++) {
      for (size_t j = 0; j < n; j++) {
        closure[i][j] = closure[i][j] | (closure[i][k] & closure[k][j]);
      }
    }
  }

  return closure;
}

void printGraphAdjMatrix(const std::vector<std::vector<bool>>& graphAdjMatrix) {
  for (size_t i = 0; i < graphAdjMatrix.size(); i++) {
    for (size_t j = 0; j < graphAdjMatrix.size(); j++) {
      std::cout << (int)graphAdjMatrix[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

int main() {
  auto graphAdjMatrix = readGraphAdjMatrix();
  auto closure = findTransitiveClosure(graphAdjMatrix);
  printGraphAdjMatrix(closure);
}