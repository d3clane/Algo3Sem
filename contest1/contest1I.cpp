#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <vector>

struct MaxAreaInfo {
  int maxNumberOfAreas = 0;
  int maxNumberOfAreasIndex = 0;
};

struct IthAreaInfo {
  int areasGot;
  int prevArea;

  IthAreaInfo() : areasGot(1), prevArea(-1) {}
};

MaxAreaInfo findMaxArea(const std::vector<IthAreaInfo>& areasDP) {
  MaxAreaInfo maxArea;
  for (size_t i = 0; i < areasDP.size(); ++i) {
    if (maxArea.maxNumberOfAreas < areasDP[i].areasGot) {
      maxArea.maxNumberOfAreas = areasDP[i].areasGot;
      maxArea.maxNumberOfAreasIndex = i;
    }
  }

  return maxArea;
}

std::vector<int> restoreAreasToCaptureBasedOnAreasDP(
    const std::vector<int>& numberOfGangstersInAreas,
    const std::vector<IthAreaInfo>& areasDP) {
  MaxAreaInfo maxArea = findMaxArea(areasDP);

  int i = maxArea.maxNumberOfAreasIndex;
  std::vector<int> areasToCapture;
  while (i != -1) {
    areasToCapture.push_back(numberOfGangstersInAreas[i]);
    i = areasDP[i].prevArea;
  }

  std::reverse(areasToCapture.begin(), areasToCapture.end());

  return areasToCapture;
}

std::vector<IthAreaInfo> calculateAreasDP(
    const std::vector<int>& numberOfGangstersInAreas,
    bool sequenceCondition(int)) {
  std::vector<IthAreaInfo> areasDP(numberOfGangstersInAreas.size());

  for (size_t i = 1; i < numberOfGangstersInAreas.size(); ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (areasDP[i].areasGot < areasDP[j].areasGot + 1) {
        if (sequenceCondition(areasDP[j].areasGot) &&
            numberOfGangstersInAreas[j] > numberOfGangstersInAreas[i]) {
          areasDP[i].areasGot = areasDP[j].areasGot + 1;
          areasDP[i].prevArea = j;
        } else if (!sequenceCondition(areasDP[j].areasGot) &&
                   numberOfGangstersInAreas[j] < numberOfGangstersInAreas[i]) {
          areasDP[i].areasGot = areasDP[j].areasGot + 1;
          areasDP[i].prevArea = j;
        }
      }
    }
  }

  return areasDP;
}

std::vector<int> findAreasToCaptureWithSequenceConditionStrategy(
    const std::vector<int>& numberOfGangstersInAreas,
    bool sequenceCondition(int)) {
  return restoreAreasToCaptureBasedOnAreasDP(
      numberOfGangstersInAreas,
      calculateAreasDP(numberOfGangstersInAreas, sequenceCondition));
}

bool isEven(int x) { return x % 2 == 0; }
bool isOdd(int x) { return x % 2 == 1; }

std::vector<int> findAreasToCapture(
    const std::vector<int>& numberOfGangstersInAreas) {
  auto areasToCaptureStrategy1 =
      findAreasToCaptureWithSequenceConditionStrategy(numberOfGangstersInAreas,
                                                      isEven);
  auto areasToCaptureStrategy2 =
      findAreasToCaptureWithSequenceConditionStrategy(numberOfGangstersInAreas,
                                                      isOdd);

  if (areasToCaptureStrategy1.size() < areasToCaptureStrategy2.size()) {
    return areasToCaptureStrategy2;
  }

  return areasToCaptureStrategy1;
}

std::vector<int> readInput() {
  size_t nAreas = 0;
  std::cin >> nAreas;

  std::vector<int> numberOfGangstersInAreas(nAreas);
  std::copy_n(std::istream_iterator<int>(std::cin), nAreas,
              numberOfGangstersInAreas.begin());
  return numberOfGangstersInAreas;
}

void printAreasToCapture(const std::vector<int>& areasToCapture) {
  std::cout << areasToCapture.size() << "\n";

  std::copy(areasToCapture.begin(), areasToCapture.end(),
            std::ostream_iterator<int>(std::cout, " "));
}

int main() {
  std::vector<int> numberOfGangstersInAreas = readInput();

  std::vector<int> areasToCapture =
      findAreasToCapture(numberOfGangstersInAreas);

  printAreasToCapture(areasToCapture);
}
