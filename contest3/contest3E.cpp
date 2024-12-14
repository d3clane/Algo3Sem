#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <vector>

std::vector<std::vector<size_t> > readGraph() {
  size_t nVertexes = 0, nEdges = 0;
  std::cin >> nVertexes >> nEdges;

  std::vector<std::vector<size_t> > graph(nVertexes);
  for (size_t i = 0; i < nEdges; ++i) {
    size_t from = 0, to = 0;
    std::cin >> from >> to;
    --from, --to;
    graph[from].push_back(to);
  }

  return graph;
}

void findTopologicalSort(size_t vertex,
                         const std::vector<std::vector<size_t> >& graph,
                         std::vector<bool>& used,
                         std::vector<size_t>& topSort) {
  used[vertex] = true;

  for (auto& to : graph[vertex]) {
    if (used[to]) continue;

    findTopologicalSort(to, graph, used, topSort);
  }

  topSort.push_back(vertex);
}

std::vector<size_t> findTopologicalSort(
    const std::vector<std::vector<size_t> >& graph) {
  std::vector<bool> used(graph.size(), false);
  std::vector<size_t> topSort;

  for (size_t i = 0; i < graph.size(); ++i) {
    if (!used[i]) {
      findTopologicalSort(i, graph, used, topSort);
    }
  }

  std::reverse(topSort.begin(), topSort.end());
  return topSort;
}

std::vector<std::vector<size_t> > reverseGraph(
    const std::vector<std::vector<size_t> >& graph) {
  std::vector<std::vector<size_t> > reverseGraph(graph.size());
  for (size_t v = 0; v < static_cast<size_t>(graph.size()); ++v) {
    for (auto& to : graph[v]) {
      reverseGraph[to].push_back(v);
    }
  }

  return reverseGraph;
}

void findComponent(size_t vertex,
                   const std::vector<std::vector<size_t> >& graph,
                   std::vector<bool>& used, std::vector<size_t>& component) {
  used[vertex] = true;

  for (auto& to : graph[vertex]) {
    if (used[to]) continue;

    findComponent(to, graph, used, component);
  }

  component.push_back(vertex);
}

std::vector<std::vector<size_t> > findComponents(
    const std::vector<std::vector<size_t> >& graph) {
  std::vector<bool> used(graph.size(), false);
  std::vector<std::vector<size_t> > components;
  std::vector<std::vector<size_t> > reversedGraph = reverseGraph(graph);

  std::vector<size_t> topSort = findTopologicalSort(graph);

  for (size_t i = 0; i < topSort.size(); ++i) {
    if (used[topSort[i]]) continue;

    std::vector<size_t> component;
    findComponent(topSort[i], reversedGraph, used, component);
    components.push_back(component);
  }

  return components;
}

void printComponents(const std::vector<std::vector<size_t> >& graph,
                     const std::vector<std::vector<size_t> >& components) {
  std::vector<size_t> vertexesComponentsIds(graph.size(), -1);

  for (size_t i = 0; i < components.size(); ++i) {
    for (auto& vertex : components[i]) {
      vertexesComponentsIds[vertex] = i;
    }
  }

  std::cout << components.size() << "\n";
  for (size_t i = 0; i < graph.size(); ++i) {
    std::cout << vertexesComponentsIds[i] + 1 << " ";
  }
  std::cout << "\n";
}

int main() {
  std::vector<std::vector<size_t> > graph = readGraph();

  std::vector<std::vector<size_t> > components = findComponents(graph);

  printComponents(graph, components);
}