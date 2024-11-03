#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <vector>

std::vector<std::vector<int> > readGraph() {
  int nVertexes = 0, nEdges = 0;
  std::cin >> nVertexes >> nEdges;

  std::vector<std::vector<int> > graph(nVertexes);
  for (size_t i = 0; i < nEdges; ++i) {
    int u = 0, v = 0;
    std::cin >> u >> v;
    --u, --v;
    graph[u].push_back(v);
  }

  return graph;
}

void findTopologicalSort(int vertex, const std::vector<std::vector<int> >& graph,
                        std::vector<bool>& used, std::vector<int>& topSort) {
  used[vertex] = true;

  for (auto& to : graph[vertex]) {
    if (used[to]) continue;

    findTopologicalSort(to, graph, used, topSort);
  }

  topSort.push_back(vertex);
}

std::vector<int> findTopologicalSort(
    const std::vector<std::vector<int> >& graph) {
  std::vector<bool> used(graph.size(), false);
  std::vector<int> topSort;

  for (size_t i = 0; i < graph.size(); ++i) {
    if (!used[i]) {
      findTopologicalSort(i, graph, used, topSort);
    }
  }

  std::reverse(topSort.begin(), topSort.end());
  return topSort;
}

std::vector<std::vector<int> > reverseGraph(
    const std::vector<std::vector<int> >& graph) {
  std::vector<std::vector<int> > reverseGraph(graph.size());
  for (int v = 0; v < graph.size(); ++v) {
    for (auto& to : graph[v]) {
      reverseGraph[to].push_back(v);
    }
  }

  return reverseGraph;
}

void findComponent(int vertex, const std::vector<std::vector<int> >& graph,
                  std::vector<bool>& used, std::vector<int>& component) {
  used[vertex] = true;

  for (auto& to : graph[vertex]) {
    if (used[to]) continue;

    findComponent(to, graph, used, component);
  }

  component.push_back(vertex);
}

std::vector<std::vector<int> > findComponents(
    const std::vector<std::vector<int> >& graph) {
  std::vector<bool> used(graph.size(), false);
  std::vector<std::vector<int> > components;
  std::vector<std::vector<int> > reversedGraph = reverseGraph(graph);

  std::vector<int> topSort = findTopologicalSort(graph);

  for (size_t i = 0; i < topSort.size(); ++i) {
    if (used[topSort[i]]) continue;

    std::vector<int> component;
    findComponent(topSort[i], reversedGraph, used, component);
    components.push_back(component);
  }

  return components;
}

void printComponents(const std::vector<std::vector<int> >& graph,
                     const std::vector<std::vector<int> >& components) {
  std::vector<int> vertexesComponentsIds(graph.size(), -1);

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
  std::vector<std::vector<int> > graph = readGraph();

  std::vector<std::vector<int> > components = findComponents(graph);

  printComponents(graph, components);
}