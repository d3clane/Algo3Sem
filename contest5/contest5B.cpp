#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <vector>

struct Edge {
  size_t to;
  uint64_t weight;
};

struct Distance {
  size_t to;
  uint64_t distance;
};

bool operator<(const Distance& self, const Distance& other) {
  return self.distance < other.distance ||
         (self.distance == other.distance && self.to < other.to);
}

static const uint64_t kInfiniteDistance = 2e16;

uint64_t findMinDistance(const std::vector<std::vector<Edge>>& graph,
                         size_t start, size_t end,
                         const std::vector<uint64_t>& virusDistances) {
  std::vector<uint64_t> distance(graph.size(), kInfiniteDistance);
  std::set<Distance, std::less<Distance>> distancesTo;
  distancesTo.insert(Distance{start, 0});
  distance[start] = 0;

  while (!distancesTo.empty()) {
    size_t from = distancesTo.begin()->to;
    distancesTo.erase(distancesTo.begin());

    assert(distance[from] < virusDistances[from]);

    for (auto [to, weight] : graph[from]) {
      uint64_t newDistance = distance[from] + weight;

      if (distance[to] > newDistance && newDistance < virusDistances[to]) {
        distancesTo.erase(Distance{to, distance[to]});
        distance[to] = newDistance;
        distancesTo.insert(Distance{to, distance[to]});
      }
    }
  }

  return distance[end];
}

std::vector<uint64_t> findVirusDistances(
    const std::vector<std::vector<Edge>>& graph,
    const std::vector<size_t> starts) {
  std::vector<uint64_t> virusDistances(graph.size(), kInfiniteDistance);
  std::set<Distance> distancesTo;

  for (auto start : starts) {
    virusDistances[start] = 0;
    distancesTo.insert(Distance{start, 0});
  }

  while (!distancesTo.empty()) {
    size_t from = distancesTo.begin()->to;
    distancesTo.erase(distancesTo.begin());

    for (auto [to, weight] : graph[from]) {
      uint64_t newDistance = virusDistances[from] + weight;
      if (virusDistances[to] > newDistance) {
        distancesTo.erase(Distance{to, virusDistances[to]});
        virusDistances[to] = newDistance;
        distancesTo.insert(Distance{to, virusDistances[to]});
      }
    }
  }

  return virusDistances;
}

struct Spaceship {
  size_t start;
  size_t end;

  std::vector<std::vector<Edge>> graph;
  std::vector<size_t> virusStarts;
};

void readSpaceshipInfo(Spaceship& spaceship) {
  size_t nVertexes, nEdges, nVirusStarts;
  std::cin >> nVertexes >> nEdges >> nVirusStarts;

  Spaceship spaceship;
  spaceship.graph.resize(nVertexes);
  spaceship.virusStarts.resize(nVirusStarts);

  for (size_t i = 0; i < nVirusStarts; ++i) {
    std::cin >> spaceship.virusStarts[i];
    --spaceship.virusStarts[i];
  }

  for (size_t i = 0; i < nEdges; ++i) {
    size_t u, v;
    size_t weight;
    std::cin >> u >> v >> weight;
    --u, --v;
    spaceship.graph[u].push_back(Edge{v, weight});
    spaceship.graph[v].push_back(Edge{u, weight});
  }

  std::cin >> spaceship.start >> spaceship.end;
  --spaceship.start, --spaceship.end;
}

int64_t findMinDistance(const Spaceship& spaceship) {
  std::vector<uint64_t> virusDistances =
      findVirusDistances(spaceship.graph, spaceship.virusStarts);
  uint64_t distance = findMinDistance(spaceship.graph, spaceship.start,
                                      spaceship.end, virusDistances);

  return distance != kInfiniteDistance ? distance : -1;
}

int main() {
  Spaceship spaceship;
  readSpaceshipInfo(spaceship);

  std::cout << findMinDistance(spaceship) << std::endl;

  return 0;
}