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

  unsigned countSameEdgesOnWay(size_t vertex1, size_t vertex2,
                               size_t vertexTo) const;

 private:
  void initLcaJumps(size_t vertex, size_t parent, unsigned& timer);
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

  lcaJumpVertexTo_[0][0] = 0;

  unsigned timer = 0;
  initLcaJumps(0, numberOfVertexes, timer);
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

void CondenseTree::initLcaJumps(size_t vertex, size_t parent, unsigned& timer) {
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
    initLcaJumps(to, vertex, timer);
  }

  timeOut_[vertex] = timer;
}

class Graph {
 public:
  Graph(const std::vector<std::vector<size_t>> graph);

  CondenseTree condense() const;

 private:
  void findBridges(size_t vertex, size_t parent, std::vector<bool>& used,
                   std::vector<unsigned>& timeIn, unsigned& timer,
                   std::vector<unsigned>& minTimeUp,
                   std::set<std::pair<size_t, size_t>>& bridges) const;

  void condense(size_t vertex, size_t condGraphVertexId,
                std::vector<size_t>& condGraphMapping, std::vector<bool>& used,
                const std::set<std::pair<size_t, size_t>>& bridges) const;

 private:
  std::vector<std::vector<size_t>> graph_;
};

Graph::Graph(const std::vector<std::vector<size_t>> graph) : graph_(graph) {}

void Graph::findBridges(size_t vertex, size_t parent, std::vector<bool>& used,
                        std::vector<unsigned>& timeIn, unsigned& timer,
                        std::vector<unsigned>& minTimeUp,
                        std::set<std::pair<size_t, size_t>>& bridges) const {
  used[vertex] = true;
  minTimeUp[vertex] = timeIn[vertex] = timer++;

  for (size_t to : graph_[vertex]) {
    if (to == parent) continue;

    if (used[to])
      minTimeUp[vertex] = std::min(minTimeUp[vertex], timeIn[to]);
    else {
      findBridges(to, vertex, used, timeIn, timer, minTimeUp, bridges);

      minTimeUp[vertex] = std::min(minTimeUp[to], minTimeUp[vertex]);

      if (minTimeUp[to] > timeIn[vertex]) {
        bridges.insert({vertex, to});
        bridges.insert({to, vertex});
      }
    }
  }
}

void Graph::condense(size_t vertex, size_t condGraphVertexId,
                     std::vector<size_t>& condGraphMapping,
                     std::vector<bool>& used,
                     const std::set<std::pair<size_t, size_t>>& bridges) const {
  used[vertex] = true;
  condGraphMapping[vertex] = condGraphVertexId;

  for (size_t to : graph_[vertex]) {
    if (bridges.find({vertex, to}) != bridges.end()) {
      continue;
    }

    if (!used[to]) {
      condense(to, condGraphVertexId, condGraphMapping, used, bridges);
    }
  }
}

CondenseTree Graph::condense() const {
  static const unsigned inf = 1e9;
  std::vector<bool> used(graph_.size(), false);
  std::vector<unsigned> timeIn(graph_.size(), 0);
  std::vector<unsigned> minTimeUp(graph_.size(), inf);

  std::set<std::pair<size_t, size_t>> bridges;

  unsigned timer = 0;

  findBridges(0, 0, used, timeIn, timer, minTimeUp, bridges);

  used.assign(graph_.size(), false);

  std::vector<size_t> condGraphMapping(graph_.size());

  size_t condGraphVertexId = 0;
  for (size_t vertex = 0; vertex < graph_.size(); ++vertex) {
    if (!used[vertex]) {
      condense(vertex, condGraphVertexId, condGraphMapping, used, bridges);
      condGraphVertexId++;
    }
  }

  std::vector<Edge> edges;

  for (auto& edge : bridges) {
    if (edge.first < edge.second) {
      edges.push_back(
          Edge{condGraphMapping[edge.first], condGraphMapping[edge.second]});
    }
  }

  return CondenseTree{condGraphVertexId, edges, condGraphMapping};
}

struct Input
{
    Graph graph;
    size_t finalVertex;
};

Input readInput()
{
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

  return Input{ Graph(graph), f };
}

void proceedQueries(const Graph& graph, size_t finalVertex, size_t numberOfQueries)
{
    CondenseTree condenseTree = graph.condense();

    for (size_t query = 0; query < numberOfQueries; ++query)
    {
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