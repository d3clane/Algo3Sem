#include <vector>
#include <iostream>
#include <cassert>
#include <cstdint>

struct Input
{
  std::vector<std::vector<int> > friends;
  size_t numberOfRobots;
};

Input readInput()
{
  size_t numberOfRobots = 0;
  std::cin >> numberOfRobots;

  std::vector<std::vector<int> > friends(numberOfRobots, std::vector<int>(numberOfRobots));

  for (size_t i = 0; i < numberOfRobots; ++i)
  {
    for (size_t j = 0; j < numberOfRobots; ++j)
    {
      char ch = 0;
      std::cin >> ch;

      int val = ch - '0';
      assert(val == 0 || val == 1);

      friends[i][j] = val;
    }
    friends[i][i] = 1;
  }

  return {friends, numberOfRobots};
}

std::vector<uint32_t> findFirstToSecondGraphEdges(
  const std::vector<std::vector<int> >& friends,
  size_t firstGraphSize,
  size_t secondGraphSize
)
{
  assert(secondGraphSize < sizeof(uint32_t) * __CHAR_BIT__);

  std::vector<uint32_t> firstToSecondGraphEdges(firstGraphSize);

  for (size_t i = 0; i < firstGraphSize; ++i)
  {
    for (size_t j = 0; j < secondGraphSize; ++j)
    {
      if (friends[i][j + firstGraphSize])
        firstToSecondGraphEdges[i] |= (1 << j);
    }
  }

  return firstToSecondGraphEdges;
}

std::vector<uint32_t> findPossibleVertexesInSecondGraph(
  const std::vector<std::vector<int> >& friends,
  size_t firstGraphSize,
  size_t secondGraphSize
)
{
  std::vector<uint32_t> possibleVertexesInSecondGraph(1 << firstGraphSize, 0);

  std::vector<uint32_t> firstToSecondGraphEdges = findFirstToSecondGraphEdges(
    friends, firstGraphSize, secondGraphSize
  );
  assert(firstToSecondGraphEdges.size() == firstGraphSize);

  size_t maxBitPos = 0;
  possibleVertexesInSecondGraph[0] = (1 << secondGraphSize) - 1;
  for (size_t mask = 1; mask < possibleVertexesInSecondGraph.size(); ++mask)
  {
    if ((mask & (1 << (maxBitPos + 1))) != 0)
    {
      ++maxBitPos;
    }

    possibleVertexesInSecondGraph[mask] = 
      possibleVertexesInSecondGraph[mask & ~(1 << maxBitPos)] & firstToSecondGraphEdges[maxBitPos];
  }

  return possibleVertexesInSecondGraph;
}

std::pair<std::vector<std::vector<int> >, std::vector<std::vector<int> > > splitOnGraphs(
  std::vector<std::vector<int> >& friends,
  size_t firstGraphSize,
  size_t secondGraphSize
)
{
  std::vector<std::vector<int> > firstGraph(firstGraphSize, std::vector<int>(firstGraphSize));
  std::vector<std::vector<int> > secondGraph(secondGraphSize, std::vector<int>(secondGraphSize));

  for (size_t i = 0; i < firstGraphSize; ++i)
  {
    for (size_t j = 0; j < firstGraphSize; ++j)
    {
      firstGraph[i][j] = friends[i][j];
    }
  }

  for (size_t i = 0; i < secondGraphSize; ++i)
  {
    for (size_t j = 0; j < secondGraphSize; ++j)
    {
      secondGraph[i][j] = friends[i + firstGraphSize][j + firstGraphSize];
    }
  }

  return {firstGraph, secondGraph};
}

std::vector<uint32_t> findEdgesInMaskForm(const std::vector<std::vector<int> >& graph)
{
  size_t size = graph.size();
  std::vector<uint32_t> edgesInMaskForm(size, 0);

  for (size_t i = 0; i < size; ++i)
  {
    for (size_t j = 0; j < size; ++j)
    {
      if (graph[i][j])
      {
        edgesInMaskForm[i] |= (1 << j);
      }
    }
  }

  return edgesInMaskForm;
}

std::vector<bool> findIsCliqueArray(const std::vector<std::vector<int> >& graph)
{
  size_t size = graph.size();

  std::vector<uint32_t> edgesInMaskForm = findEdgesInMaskForm(graph);

  std::vector<bool> isClique((1 << size), false);
  isClique[0] = true;

  size_t maxBitPos = 0;
  for (size_t mask = 1; mask < isClique.size(); ++mask)
  {
    if ((mask & (1 << (maxBitPos + 1))) != 0)
    {
      ++maxBitPos;
    }

    if ((mask & edgesInMaskForm[maxBitPos]) == mask)
    {
      isClique[mask] = isClique[mask & ~(1 << maxBitPos)];
    }

  }

  return isClique;
}

std::vector<uint64_t> findKCliquesArray(const std::vector<std::vector<int> >& graph)
{
  size_t size = graph.size();

  std::vector<uint32_t> edgesInMaskForm = findEdgesInMaskForm(graph);

  std::vector<uint64_t> kCliques((1 << size), 0);
  kCliques[0] = 1;

  size_t maxBitPos = 0;

  for (size_t mask = 1; mask < kCliques.size(); ++mask)
  {
    if ((mask & (1 << (maxBitPos + 1))) != 0)
    {
      ++maxBitPos;
    }

    kCliques[mask] = kCliques[mask & ~(1 << maxBitPos)] + 
                     kCliques[mask & edgesInMaskForm[maxBitPos] & ~(1 << maxBitPos)];
  }

  return kCliques;
}

uint64_t findEndDay(const Input& input)
{
  auto [friends, numberOfRobots] = input;

  if (numberOfRobots == 1)
  {
    return 2;
  }

  size_t firstGraphSize  = numberOfRobots / 2;
  size_t secondGraphSize = numberOfRobots - firstGraphSize;

  std::vector<uint32_t> possibleVertexesInSecondGraph = findPossibleVertexesInSecondGraph(
    friends, firstGraphSize, secondGraphSize
  );

  assert(possibleVertexesInSecondGraph.size() == (1 << firstGraphSize));

  auto [firstGraph, secondGraph] = splitOnGraphs(friends, firstGraphSize, secondGraphSize);

  std::vector<bool> isClique = findIsCliqueArray(firstGraph);

  std::vector<uint64_t> kCliques = findKCliquesArray(secondGraph);

  assert(isClique.size() == possibleVertexesInSecondGraph.size());
  assert(kCliques.size() == (1 << secondGraphSize));

  uint64_t endDay = 0;
  for (size_t mask = 0; mask < possibleVertexesInSecondGraph.size(); ++mask)
  {
    if (!isClique[mask])
      continue;

    endDay += kCliques[possibleVertexesInSecondGraph[mask]];
  }

  return endDay;
}

int main()
{
  auto input = readInput();

  uint64_t endDay = findEndDay(input);

  std::cout << endDay << "\n";
}
