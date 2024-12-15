#include <algorithm>
#include <iostream>
#include <vector>

class Graph {
 public:
  Graph() = default;
  explicit Graph(std::vector<std::vector<bool>> graphAdjMatrix)
      : adjacentMatrix_(graphAdjMatrix) {}

  Graph findTransitiveClosure();

  void printGraphAdjacentMatrix();

  static Graph readGraphFromAdjacentMatrix();

 private:
  std::vector<std::vector<bool>> adjacentMatrix_;
};

Graph Graph::readGraphFromAdjacentMatrix() {
  size_t n;
  std::cin >> n;

  std::vector<std::vector<bool>> graphAdjacentMatrix(
      n, std::vector<bool>(n, false));

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      int value = 0;
      std::cin >> value;
      if (value == 0) {
        graphAdjacentMatrix[i][j] = 0;
      } else {
        graphAdjacentMatrix[i][j] = 1;
      }
    }
  }

  return Graph{graphAdjacentMatrix};
}

Graph Graph::findTransitiveClosure() {
  size_t n = adjacentMatrix_.size();

  std::vector<std::vector<bool>> closure = adjacentMatrix_;

  for (size_t k = 0; k < n; k++) {
    for (size_t i = 0; i < n; i++) {
      for (size_t j = 0; j < n; j++) {
        closure[i][j] = closure[i][j] | (closure[i][k] & closure[k][j]);
      }
    }
  }

  return Graph{closure};
}

void Graph::printGraphAdjacentMatrix() {
  for (size_t i = 0; i < adjacentMatrix_.size(); i++) {
    for (size_t j = 0; j < adjacentMatrix_[i].size(); j++) {
      std::cout << static_cast<int>(adjacentMatrix_[i][j]) << " ";
    }
    std::cout << std::endl;
  }
}

int main() {
  Graph graph = Graph::readGraphFromAdjacentMatrix();
  Graph closure = graph.findTransitiveClosure();

  closure.printGraphAdjacentMatrix();
}