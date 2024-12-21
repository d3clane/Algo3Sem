#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <set>
#include <vector>

struct Edge {
  size_t u, v;
};

class CondenseTree {
 public:
  CondenseTree(size_t numberOfVertexes, const std::vector<Edge>& edges,
               const std::vector<size_t>& vertexMapping);

  void buildLca();

  unsigned countSameEdgesOnWay(size_t vertex1, size_t vertex2,
                               size_t vertexTo) const;

 private:
  void buildLca(size_t vertex, size_t parent, unsigned& timer);
  bool isAncestor(size_t ancestor, size_t vertex) const;
  unsigned findDistance(size_t vertexFrom, size_t vertexTo) const;
  size_t findLca(size_t vertex1, size_t vertex2) const;

 private:
  std::vector<std::vector<size_t>> tree_;

  std::vector<std::vector<size_t>> lcaJumpVertexTo_;

  std::vector<unsigned> timeIn_;
  std::vector<unsigned> timeOut_;

  static const size_t maxLogValue_ = 30;

  std::vector<size_t> vertexMapping_;
};

CondenseTree::CondenseTree(size_t numberOfVertexes,
                           const std::vector<Edge>& edges,
                           const std::vector<size_t>& vertexMapping)
    : tree_(numberOfVertexes),
      lcaJumpVertexTo_(numberOfVertexes, std::vector<size_t>(maxLogValue_)),
      timeIn_(numberOfVertexes, 0),
      timeOut_(numberOfVertexes, 0),
      vertexMapping_(vertexMapping) {
  assert(numberOfVertexes == edges.size() + 1);

  for (size_t i = 0; i < edges.size(); ++i) {
    tree_[edges[i].u].push_back(edges[i].v);
    tree_[edges[i].v].push_back(edges[i].u);
  }
}

void CondenseTree::buildLca() { 
  lcaJumpVertexTo_[0][0] = 0;

  unsigned timer = 0;
  buildLca(0, 0, timer); 
}

unsigned CondenseTree::countSameEdgesOnWay(size_t vertex1, size_t vertex2,
                                           size_t vertexTo) const {
  vertex1 = vertexMapping_[vertex1];
  vertex2 = vertexMapping_[vertex2];
  vertexTo = vertexMapping_[vertexTo];

  unsigned numberOfSameEdges = 0;

  size_t lca = findLca(vertex1, vertex2);

  if (!isAncestor(lca, vertexTo)) return findDistance(lca, vertexTo);

  assert(isAncestor(lca, vertexTo));

  int lca1 = findLca(vertex1, vertexTo);
  int lca2 = findLca(vertex2, vertexTo);

  assert(lca1 == lca || lca2 == lca);

  return std::min(findDistance(vertexTo, lca1), findDistance(vertexTo, lca2));
}

unsigned CondenseTree::findDistance(size_t vertexFrom, size_t vertexTo) const {
  unsigned distance = 0;

  for (ssize_t jumpLen = maxLogValue_ - 1; jumpLen >= 0; --jumpLen) {
    if (!isAncestor(lcaJumpVertexTo_[vertexFrom][jumpLen], vertexTo)) {
      distance += (1 << jumpLen);
      vertexFrom = lcaJumpVertexTo_[vertexFrom][jumpLen];
    }
  }

  for (ssize_t jumpLen = maxLogValue_ - 1; jumpLen >= 0; --jumpLen) {
    if (!isAncestor(lcaJumpVertexTo_[vertexTo][jumpLen], vertexFrom)) {
      distance += (1 << jumpLen);
      vertexTo = lcaJumpVertexTo_[vertexTo][jumpLen];
    }
  }

  if (!isAncestor(vertexFrom, vertexTo)) {
    ++distance;
  }

  if (!isAncestor(vertexTo, vertexFrom)) {
    ++distance;
  }

  return distance;
}

size_t CondenseTree::findLca(size_t vertex1, size_t vertex2) const {
  if (isAncestor(vertex1, vertex2)) return vertex1;
  if (isAncestor(vertex2, vertex1)) return vertex2;

  for (ssize_t jumpLen = maxLogValue_ - 1; jumpLen >= 0; --jumpLen) {
    if (!isAncestor(lcaJumpVertexTo_[vertex1][jumpLen], vertex2)) {
      vertex1 = lcaJumpVertexTo_[vertex1][jumpLen];
    }
  }

  return lcaJumpVertexTo_[vertex1][0];
}

bool CondenseTree::isAncestor(size_t ancestor, size_t vertex) const {
  return timeIn_[ancestor] <= timeIn_[vertex] &&
         timeOut_[ancestor] >= timeOut_[vertex];
}

void CondenseTree::buildLca(size_t vertex, size_t parent, unsigned& timer) {
  for (size_t jumpLen = 1; jumpLen < maxLogValue_; ++jumpLen) {
    size_t prevJumpVertex = lcaJumpVertexTo_[vertex][jumpLen - 1];
    lcaJumpVertexTo_[vertex][jumpLen] =
        lcaJumpVertexTo_[prevJumpVertex][jumpLen - 1];
  }

  timeIn_[vertex] = timer++;

  for (size_t to : tree_[vertex]) {
    if (to == parent) {
      continue;
    }

    lcaJumpVertexTo_[to][0] = vertex;
    buildLca(to, vertex, timer);
  }

  timeOut_[vertex] = timer;
}

class Graph {
 public:
  Graph(const std::vector<std::vector<size_t>> graph);

  const std::vector<std::vector<size_t>>& getGraph() const;
  CondenseTree condense() const;

 private:
  std::vector<std::vector<size_t>> graph_;
};

class BridgeFinder {
 public:
  std::set<std::pair<size_t, size_t>> findBridges(const Graph* graph);

 private:
  void findBridges(size_t vertex, size_t parent,
                   std::set<std::pair<size_t, size_t>>& bridges,
                   unsigned& timer);

 private:
  const Graph* graph_;
  std::vector<bool> used_;
  std::vector<unsigned> timeIn_;
  std::vector<unsigned> timeOut_;
  std::vector<unsigned> minTimeUp_;
};

class GraphCondenser {
 public:
  void condense(const Graph* graph);

  std::vector<size_t> getVertexMapping();
  std::vector<Edge> getEdges();
  size_t getCondGraphSize();

 private:
  void condense(size_t vertex);

 private:
  const Graph* graph_;

  std::set<std::pair<size_t, size_t>> bridges_;
  std::vector<Edge> edges_;
  std::vector<size_t> vertexMapping_;
  std::vector<bool> used_;
  size_t condGraphSize_ = 0;
};

void GraphCondenser::condense(const Graph* graph) {
  graph_ = graph;
  bridges_ = BridgeFinder().findBridges(graph);

  auto& vectorGraph = graph_->getGraph();
  size_t graphSize = vectorGraph.size();
  vertexMapping_.resize(graphSize);
  used_.resize(graphSize, false);

  for (size_t vertex = 0; vertex < vectorGraph.size(); ++vertex) {
    if (!used_[vertex]) {
      condense(vertex);
      condGraphSize_++;
    }
  }

  for (auto& edge : bridges_) {
    if (edge.first < edge.second) {
      edges_.push_back(
          Edge{vertexMapping_[edge.first], vertexMapping_[edge.second]});
    }
  }
}

void GraphCondenser::condense(size_t vertex) {
  used_[vertex] = true;
  vertexMapping_[vertex] = condGraphSize_;

  auto& graph = graph_->getGraph();
  for (size_t to : graph[vertex]) {
    if (bridges_.find({vertex, to}) != bridges_.end()) {
      continue;
    }

    if (!used_[to]) {
      condense(to);
    }
  }
}

std::vector<size_t> GraphCondenser::getVertexMapping() { return vertexMapping_; }
std::vector<Edge> GraphCondenser::getEdges() { return edges_; }
size_t GraphCondenser::getCondGraphSize() { return condGraphSize_; }

std::set<std::pair<size_t, size_t>> BridgeFinder::findBridges(
    const Graph* graph) {
  graph_ = graph;
  size_t graphSize = graph->getGraph().size();

  used_.resize(graphSize, false);
  timeIn_.resize(graphSize, 0);
  timeOut_.resize(graphSize, 0);
  minTimeUp_.resize(graphSize, 0);
  std::set<std::pair<size_t, size_t>> bridges;
  unsigned timer = 0;

  findBridges(0, 0, bridges, timer);
  return bridges;
}

void BridgeFinder::findBridges(size_t vertex, size_t parent,
                               std::set<std::pair<size_t, size_t>>& bridges,
                               unsigned& timer) {
  used_[vertex] = true;
  minTimeUp_[vertex] = timeIn_[vertex] = timer++;

  auto& graph = graph_->getGraph();
  for (size_t to : graph[vertex]) {
    if (to == parent) continue;

    if (used_[to])
      minTimeUp_[vertex] = std::min(minTimeUp_[vertex], timeIn_[to]);
    else {
      findBridges(to, vertex, bridges, timer);

      minTimeUp_[vertex] = std::min(minTimeUp_[to], minTimeUp_[vertex]);

      if (minTimeUp_[to] > timeIn_[vertex]) {
        bridges.insert({vertex, to});
        bridges.insert({to, vertex});
      }
    }
  }
}

Graph::Graph(const std::vector<std::vector<size_t>> graph) : graph_(graph) {}

const std::vector<std::vector<size_t>>& Graph::getGraph() const {
  return graph_;
}

CondenseTree Graph::condense() const {
  GraphCondenser condenser = GraphCondenser();
  condenser.condense(this);

  return CondenseTree{condenser.getCondGraphSize(), condenser.getEdges(),
                      condenser.getVertexMapping()};
}

struct Input {
  Graph graph;
  size_t finalVertex;
};

Input readInput() {
  size_t n = 0, m = 0;
  std::cin >> n >> m;

  size_t f = 0;
  std::cin >> f;
  --f;

  std::vector<std::vector<size_t>> graph(n);
  for (size_t i = 0; i < m; ++i) {
    int u, v;
    std::cin >> u >> v;

    --u, --v;
    graph[u].push_back(v);
    graph[v].push_back(u);
  }

  return Input{Graph(graph), f};
}

void proceedQueries(const Graph& graph, size_t finalVertex,
                    size_t numberOfQueries) {
  CondenseTree condenseTree = graph.condense();
  
  condenseTree.buildLca();
  for (size_t query = 0; query < numberOfQueries; ++query) {
    int u, v;
    std::cin >> u >> v;
    --u, --v;

    std::cout << condenseTree.countSameEdgesOnWay(u, v, finalVertex) << "\n";
  }
}

int main() {
  auto [graph, finalVertex] = readInput();

  size_t numberOfQueries = 0;
  std::cin >> numberOfQueries;

  proceedQueries(graph, finalVertex, numberOfQueries);
}