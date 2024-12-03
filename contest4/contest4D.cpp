#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

class Dsu {
 public:
  Dsu(size_t numberOfElements);

  size_t findSet(size_t elementId);
  void unionSets(size_t firstElementId, size_t secondElementId);

 private:
  std::vector<size_t> parent_;
};

Dsu::Dsu(size_t numberOfElements) : parent_(numberOfElements) {
  for (size_t i = 0; i < numberOfElements; ++i) {
    parent_[i] = i;
  }
}

size_t Dsu::findSet(size_t elementId) {
  if (parent_[elementId] == elementId) {
    return elementId;
  }

  return parent_[elementId] = findSet(parent_[elementId]);
}

void Dsu::unionSets(size_t firstElementId, size_t secondElementId) {
  firstElementId = findSet(firstElementId);
  secondElementId = findSet(secondElementId);

  if (firstElementId == secondElementId) {
    return;
  }

  parent_[secondElementId] = firstElementId;
}

struct Edge {
  size_t u, v;
  unsigned weight;
};

struct Graph {
  std::vector<Edge> edges;
  size_t numberOfVertexes;
};

struct Input {
  Graph graph;
  size_t numberOfQueries;
};

Input readGraphAndNumberOfQueries() {
  Graph graph;
  std::cin >> graph.numberOfVertexes;

  size_t m, k;
  std::cin >> m >> k;

  graph.edges.resize(m);
  for (size_t i = 0; i < m; ++i) {
    std::cin >> graph.edges[i].u >> graph.edges[i].v >> graph.edges[i].weight;
    --graph.edges[i].u;
    --graph.edges[i].v;
  }

  return {graph, k};
}

class Tree {
 public:
  Tree(size_t numberOfVertexes, const std::vector<Edge>& edges);

  unsigned findMinValueOnWay(size_t vertexFrom, size_t vertexTo) const;

 private:
  void initLcaJumps(size_t vertex, size_t parent, unsigned& timer);
  bool checkIsAncestor(size_t ancestor, size_t vertex) const;

 private:
  struct LcaJumpsInfo {
    size_t vertexTo;
    unsigned minValOnWay;
  };

  struct TreeEdge {
    size_t vertexTo;
    unsigned valueOnEdge;
  };

  std::vector<std::vector<TreeEdge>> tree_;

  std::vector<std::vector<LcaJumpsInfo>> lcaInfo_;

  std::vector<unsigned> timeIn_;
  std::vector<unsigned> timeOut_;

  static const size_t maxLogValue = 30;
};

Tree::Tree(size_t numberOfVertexes, const std::vector<Edge>& edges)
    : tree_(numberOfVertexes),
      lcaInfo_(numberOfVertexes, std::vector<LcaJumpsInfo>(maxLogValue)),
      timeIn_(numberOfVertexes, 0),
      timeOut_(numberOfVertexes, 0) {
  for (size_t i = 0; i < edges.size(); ++i) {
    tree_[edges[i].u].push_back(TreeEdge{edges[i].v, edges[i].weight});
    tree_[edges[i].v].push_back(TreeEdge{edges[i].u, edges[i].weight});
  }

  lcaInfo_[0][0].vertexTo = 0;
  lcaInfo_[0][0].minValOnWay = 0;

  unsigned timer = 0;
  initLcaJumps(0, numberOfVertexes, timer);
}

unsigned Tree::findMinValueOnWay(size_t vertexFrom, size_t vertexTo) const {
  unsigned minVal = 1e9;

  for (ssize_t jumpLen = maxLogValue - 1; jumpLen >= 0; --jumpLen) {
    if (!checkIsAncestor(lcaInfo_[vertexFrom][jumpLen].vertexTo, vertexTo)) {
      minVal = std::min(minVal, lcaInfo_[vertexFrom][jumpLen].minValOnWay);
      vertexFrom = lcaInfo_[vertexFrom][jumpLen].vertexTo;
    }
  }

  for (ssize_t jumpLen = maxLogValue - 1; jumpLen >= 0; --jumpLen) {
    if (!checkIsAncestor(lcaInfo_[vertexTo][jumpLen].vertexTo, vertexFrom)) {
      minVal = std::min(minVal, lcaInfo_[vertexTo][jumpLen].minValOnWay);
      vertexTo = lcaInfo_[vertexTo][jumpLen].vertexTo;
    }
  }

  if (!checkIsAncestor(vertexFrom, vertexTo)) {
    minVal = std::min(minVal, lcaInfo_[vertexFrom][0].minValOnWay);
  }

  if (!checkIsAncestor(vertexTo, vertexFrom)) {
    minVal = std::min(minVal, lcaInfo_[vertexTo][0].minValOnWay);
  }

  return minVal;
}

bool Tree::checkIsAncestor(size_t ancestor, size_t vertex) const {
  return timeIn_[ancestor] <= timeIn_[vertex] &&
         timeOut_[ancestor] >= timeOut_[vertex];
}

void Tree::initLcaJumps(size_t vertex, size_t parent, unsigned& timer) {
  for (size_t jumpLen = 1; jumpLen < maxLogValue; ++jumpLen) {
    size_t prevJumpVertex = lcaInfo_[vertex][jumpLen - 1].vertexTo;
    lcaInfo_[vertex][jumpLen].vertexTo =
        lcaInfo_[prevJumpVertex][jumpLen - 1].vertexTo;

    lcaInfo_[vertex][jumpLen].minValOnWay =
        std::min(lcaInfo_[vertex][jumpLen - 1].minValOnWay,
                 lcaInfo_[prevJumpVertex][jumpLen - 1].minValOnWay);
  }

  timeIn_[vertex] = timer++;

  for (auto& [to, valueOnEdge] : tree_[vertex]) {
    if (to == parent) {
      continue;
    }

    lcaInfo_[to][0].vertexTo = vertex;
    lcaInfo_[to][0].minValOnWay = valueOnEdge;

    initLcaJumps(to, vertex, timer);
  }

  timeOut_[vertex] = timer;
}

Tree buildMaxTree(Graph graph) {
  std::sort(graph.edges.begin(), graph.edges.end(),
            [](const Edge& a, const Edge& b) { return a.weight > b.weight; });

  Dsu dsu(graph.numberOfVertexes);

  std::vector<Edge> treeEdges;

  for (size_t i = 0; i < graph.edges.size(); ++i) {
    size_t firstSet = dsu.findSet(graph.edges[i].u);
    size_t secondSet = dsu.findSet(graph.edges[i].v);

    if (firstSet == secondSet) {
      continue;
    }

    dsu.unionSets(firstSet, secondSet);
    treeEdges.push_back(graph.edges[i]);
  }

  return Tree{graph.numberOfVertexes, treeEdges};
}

void proceedQueries(const Graph& graph, size_t numberOfQueries) {
  Tree tree = buildMaxTree(graph);

  std::vector<unsigned> answers(numberOfQueries);

  for (size_t query = 0; query < numberOfQueries; ++query) {
    size_t u = 0, v = 0;

    std::cin >> u >> v;
    --u, --v;

    answers[query] = tree.findMinValueOnWay(u, v);
  }

  std::copy(answers.begin(), answers.end(),
            std::ostream_iterator<unsigned>(std::cout, "\n"));
}

int main() {
  auto [graph, numberOfQueries] = readGraphAndNumberOfQueries();

  proceedQueries(graph, numberOfQueries);
}