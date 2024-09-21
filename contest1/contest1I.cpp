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

struct AreasInfo {
  MaxAreaInfo maxArea;
  std::vector<IthAreaInfo> areas;

  AreasInfo(MaxAreaInfo maxAreaInit, std::vector<IthAreaInfo> areasInit)
      : maxArea(maxAreaInit), areas(areasInit) {}
};

MaxAreaInfo getMaxNumberOfAreasAndPos(const std::vector<IthAreaInfo>& areas) {
  MaxAreaInfo maxArea;
  for (size_t i = 0; i < areas.size(); ++i) {
    if (maxArea.maxNumberOfAreas < areas[i].areasGot) {
      maxArea.maxNumberOfAreas = areas[i].areasGot;
      maxArea.maxNumberOfAreasIndex = i;
    }
  }

  return maxArea;
}

AreasInfo getAreasInfo(const std::vector<int>& a, bool sequenceCondition(int)) {
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

  return AreasInfo(getMaxNumberOfAreasAndPos(areas), areas);
}

bool isEven(int x) { return x % 2 == 0; }
bool isOdd(int x) { return x % 2 == 1; }

std::vector<int> getBestAreasIndexes(const std::vector<int>& a) {
  AreasInfo areaInfo1 = getAreasInfo(a, isEven);
  AreasInfo areaInfo2 = getAreasInfo(a, isOdd);

  if (areaInfo1.maxArea.maxNumberOfAreas < areaInfo2.maxArea.maxNumberOfAreas) {
    std::swap(areaInfo1, areaInfo2);
  }

  int i = areaInfo1.maxArea.maxNumberOfAreasIndex;
  std::vector<int> ans;
  while (i != -1) {
    ans.push_back(a[i]);
    i = areaInfo1.areas[i].prevArea;
  }

  std::reverse(ans.begin(), ans.end());

  return ans;
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

  std::vector<int> ans = getBestAreasIndexes(a);

  printAns(ans);
}
