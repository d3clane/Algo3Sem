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

struct AreasInfo {
  MaxAreaInfo maxArea;
  std::vector<int> prevAreas;

  AreasInfo(MaxAreaInfo maxAreaInit, const std::vector<int>& prevs)
      : maxArea(maxAreaInit), prevAreas(prevs) {}

  AreasInfo(MaxAreaInfo maxAreaInit, const std::vector<int>&& prevs)
      : maxArea(maxAreaInit), prevAreas(prevs) {}
};

struct IthAreaInfo {
  int areasGot;
  int prevArea;

  IthAreaInfo() : areasGot(1), prevArea(-1) {}
};

inline std::vector<int> convertDpToPrevs(
    const std::vector<IthAreaInfo>& areas) {
  size_t areasSize = areas.size();
  std::vector<int> prevs(areasSize);
  for (size_t i = 0; i < areasSize; ++i) {
    prevs[i] = areas[i].prevArea;
  }

  return prevs;
}

MaxAreaInfo findMaxArea(const std::vector<IthAreaInfo>& areas) {
  MaxAreaInfo maxArea;
  for (size_t i = 0; i < areas.size(); ++i) {
    if (maxArea.maxNumberOfAreas < areas[i].areasGot) {
      maxArea.maxNumberOfAreas = areas[i].areasGot;
      maxArea.maxNumberOfAreasIndex = i;
    }
  }

  return maxArea;
}

AreasInfo findAreasPrevs(const std::vector<int>& a,
                         bool sequenceCondition(int)) {
  std::vector<IthAreaInfo> areas(a.size());

  for (size_t i = 1; i < a.size(); ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (areas[i].areasGot < areas[j].areasGot + 1) {
        if (sequenceCondition(areas[j].areasGot) && a[j] > a[i]) {
          areas[i].areasGot = areas[j].areasGot + 1;
          areas[i].prevArea = j;
        } else if (!sequenceCondition(areas[j].areasGot) && a[j] < a[i]) {
          areas[i].areasGot = areas[j].areasGot + 1;
          areas[i].prevArea = j;
        }
      }
    }
  }

  return AreasInfo(findMaxArea(areas), convertDpToPrevs(areas));
}

std::vector<int> findBestAreasIndexes(const std::vector<int>& a,
                                      const AreasInfo& areasInfo) {
  int i = areasInfo.maxArea.maxNumberOfAreasIndex;
  std::vector<int> ans;
  while (i != -1) {
    ans.push_back(a[i]);
    i = areasInfo.prevAreas[i];
  }

  std::reverse(ans.begin(), ans.end());

  return ans;
}

bool isEven(int x) { return x % 2 == 0; }
bool isOdd(int x) { return x % 2 == 1; }

AreasInfo findBestAreas(const std::vector<int>& a) {
  AreasInfo areaInfo1 = findAreasPrevs(a, isEven);
  AreasInfo areaInfo2 = findAreasPrevs(a, isOdd);

  if (areaInfo1.maxArea.maxNumberOfAreas < areaInfo2.maxArea.maxNumberOfAreas) {
    return areaInfo2;
  }

  return areaInfo1;
}

std::vector<int> findBestAreasIndexes(const std::vector<int>& a) {
  return findBestAreasIndexes(a, findBestAreas(a));
}

std::vector<int> readInput() {
  size_t n = 0;
  std::cin >> n;

  std::vector<int> a(n);
  std::copy_n(std::istream_iterator<int>(std::cin), n, a.begin());
  return a;
}

void printAns(const std::vector<int>& ans) {
  std::cout << ans.size() << "\n";
  std::copy(ans.begin(), ans.end(), std::ostream_iterator<int>(std::cout, " "));
}

int main() {
  std::vector<int> a = readInput();

  std::vector<int> ans = findBestAreasIndexes(a);

  printAns(ans);
}
