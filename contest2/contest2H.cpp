#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

class Graph {
  std::vector<uint32_t> edgesMasks_;

 public:
  explicit Graph(const std::vector<std::vector<int> >& graph);

  uint32_t getEdgeMask(size_t vertex) const;

  size_t size() const;
};

Graph::Graph(const std::vector<std::vector<int> >& graph) {
  size_t size = graph.size();
  edgesMasks_.resize(size);

  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; ++j) {
      if (graph[i][j]) {
        edgesMasks_[i] |= (1 << j);
      }
    }
  }
}

size_t Graph::size() const { return edgesMasks_.size(); }

uint32_t Graph::getEdgeMask(size_t vertex) const { return edgesMasks_[vertex]; }

std::vector<std::vector<int> > readFriendsGraph() {
  size_t numberOfRobots = 0;
  std::cin >> numberOfRobots;

  std::vector<std::vector<int> > friends(numberOfRobots,
                                         std::vector<int>(numberOfRobots));

  for (size_t i = 0; i < numberOfRobots; ++i) {
    for (size_t j = 0; j < numberOfRobots; ++j) {
      char ch = 0;
      std::cin >> ch;

      int val = ch - '0';
      assert(val == 0 || val == 1);

      friends[i][j] = val;
    }
    friends[i][i] = 1;
  }

  return friends;
}

std::vector<uint32_t> findFirstToSecondGraphEdges(
    const std::vector<std::vector<int> >& friends, size_t firstGraphSize,
    size_t secondGraphSize) {
  assert(secondGraphSize < sizeof(uint32_t) * __CHAR_BIT__);

  std::vector<uint32_t> firstToSecondGraphEdges(firstGraphSize);

  for (size_t i = 0; i < firstGraphSize; ++i) {
    for (size_t j = 0; j < secondGraphSize; ++j) {
      if (friends[i][j + firstGraphSize])
        firstToSecondGraphEdges[i] |= (1 << j);
    }
  }

  return firstToSecondGraphEdges;
}

std::vector<uint32_t> findPossibleVertexesInSecondGraph(
    const std::vector<std::vector<int> >& friends, size_t firstGraphSize,
    size_t secondGraphSize) {
  std::vector<uint32_t> possibleVertexesInSecondGraph(1 << firstGraphSize, 0);

  std::vector<uint32_t> firstToSecondGraphEdges =
      findFirstToSecondGraphEdges(friends, firstGraphSize, secondGraphSize);
  assert(firstToSecondGraphEdges.size() == firstGraphSize);

  size_t maxBitPos = 0;
  possibleVertexesInSecondGraph[0] = (1 << secondGraphSize) - 1;
  for (size_t mask = 1; mask < possibleVertexesInSecondGraph.size(); ++mask) {
    if ((mask & (1 << (maxBitPos + 1))) != 0) {
      ++maxBitPos;
    }

    // vertexes that are achieved from every vertex from mask except [maxBitPos]
    // vertex BIT_AND with all vertexes I can achieve from the [maxBitPos]
    // vertex. Results in all vertexes I can achieve from each vertex from mask
    possibleVertexesInSecondGraph[mask] =
        possibleVertexesInSecondGraph[mask & ~(1 << maxBitPos)] &
        firstToSecondGraphEdges[maxBitPos];
  }

  return possibleVertexesInSecondGraph;
}

std::pair<Graph, Graph> splitOnGraphs(
    const std::vector<std::vector<int> >& friends, size_t firstGraphSize,
    size_t secondGraphSize) {
  std::vector<std::vector<int> > firstGraph(firstGraphSize,
                                            std::vector<int>(firstGraphSize));
  std::vector<std::vector<int> > secondGraph(secondGraphSize,
                                             std::vector<int>(secondGraphSize));

  for (size_t i = 0; i < firstGraphSize; ++i) {
    for (size_t j = 0; j < firstGraphSize; ++j) {
      firstGraph[i][j] = friends[i][j];
    }
  }

  for (size_t i = 0; i < secondGraphSize; ++i) {
    for (size_t j = 0; j < secondGraphSize; ++j) {
      secondGraph[i][j] = friends[i + firstGraphSize][j + firstGraphSize];
    }
  }

  return {Graph(firstGraph), Graph(secondGraph)};
}

std::vector<bool> findIsCliqueArray(const Graph& graph) {
  size_t size = graph.size();

  std::vector<bool> isClique((1 << size), false);
  isClique[0] = true;

  size_t maxBitPos = 0;
  for (size_t mask = 1; mask < isClique.size(); ++mask) {
    if ((mask & (1 << (maxBitPos + 1))) != 0) {
      ++maxBitPos;
    }

    // checking if [maxBitPos] vertex is connected with all other vertexes in
    // mask. If yes - it is clique only when mask without [maskBitPos] vertex is
    // clique
    if ((mask & graph.getEdgeMask(maxBitPos)) == mask) {
      isClique[mask] = isClique[mask & ~(1 << maxBitPos)];
    }
  }

  return isClique;
}

std::vector<uint64_t> findNCliquesArray(const Graph& graph) {
  size_t size = graph.size();

  std::vector<uint64_t> nCliques((1 << size), 0);
  nCliques[0] = 1;

  size_t maxBitPos = 0;

  for (size_t mask = 1; mask < nCliques.size(); ++mask) {
    if ((mask & (1 << (maxBitPos + 1))) != 0) {
      ++maxBitPos;
    }

    // nCliques[mask & graph.getEdgeMask(maxBitPos) & ~(1 << maxBitPos)]:
    // Get all vertexes that can be achieved from [maxBitPos] vertex, find
    // nCliques on that vertexes, now connecting all this vertexes to
    // [maxBitPos] vertex - it still a clique because all of that vertexes are
    // achieved from [maxBitPos]

    // nCliques[mask & ~(1 << maxBitPos)] - simply all cliques without
    // [maxBitPos] vertex
    nCliques[mask] =
        nCliques[mask & ~(1 << maxBitPos)] +
        nCliques[mask & graph.getEdgeMask(maxBitPos) & ~(1 << maxBitPos)];
  }

  return nCliques;
}

uint64_t findEndDay(const std::vector<std::vector<int> >& friends) {
  size_t numberOfRobots = friends.size();

  if (numberOfRobots == 1) {
    return 2;
  }

  size_t firstGraphSize = numberOfRobots / 2;
  size_t secondGraphSize = numberOfRobots - firstGraphSize;

  // for each mask1 from graph1 finding such mask2 vertexes from second graph so
  // that from EACH vertex from mask1 I can achieve EACH vertex from mask2
  std::vector<uint32_t> possibleVertexesInSecondGraph =
      findPossibleVertexesInSecondGraph(friends, firstGraphSize,
                                        secondGraphSize);

  assert(possibleVertexesInSecondGraph.size() == (1 << firstGraphSize));

  auto [firstGraph, secondGraph] =
      splitOnGraphs(friends, firstGraphSize, secondGraphSize);

  std::vector<bool> isClique = findIsCliqueArray(firstGraph);

  std::vector<uint64_t> nCliques = findNCliquesArray(secondGraph);

  assert(isClique.size() == possibleVertexesInSecondGraph.size());
  assert(nCliques.size() == (1 << secondGraphSize));

  uint64_t endDay = 0;
  for (size_t mask = 0; mask < possibleVertexesInSecondGraph.size(); ++mask) {
    if (!isClique[mask]) continue;

    // for each clique from graph1 find number of cliques from second graph
    // so that their connection is still a clique.
    endDay += nCliques[possibleVertexesInSecondGraph[mask]];
  }

  return endDay;
}

int main() {
  auto friendsGraph = readFriendsGraph();

  uint64_t endDay = findEndDay(friendsGraph);

  std::cout << endDay << "\n";
}
