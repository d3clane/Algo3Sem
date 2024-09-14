#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

std::pair<std::vector<int>, std::vector<int> > getMaxAreasGot(
    const std::vector<int>& a, size_t n, bool sequenceCondition(int)) {
  std::vector<int> numberOfAreasGot(n,
                                    1);  // numberOfAreasGot[i] - max number of
                                         // areas with i-th area in sequence
  std::vector<int> prevArea(n, -1);

  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (numberOfAreasGot[i] < numberOfAreasGot[j] + 1) {
        if (sequenceCondition(numberOfAreasGot[j]) && a[j] > a[i]) {
          numberOfAreasGot[i] = numberOfAreasGot[j] + 1;
          prevArea[i] = j;
        } else if (!sequenceCondition(numberOfAreasGot[j]) && a[j] < a[i]) {
          numberOfAreasGot[i] = numberOfAreasGot[j] + 1;
          prevArea[i] = j;
        }
      }
    }
  }

  return {numberOfAreasGot, prevArea};
}

std::pair<int, int> getMaxNumberOfAreasAndPos(
    const std::vector<int>& numberOfAreasGot) {
  int maxNumberOfAreas = 0;
  int maxNumberOfAreasIndex = 0;
  for (size_t i = 0; i < numberOfAreasGot.size(); ++i) {
    if (maxNumberOfAreas < numberOfAreasGot[i]) {
      maxNumberOfAreas = numberOfAreasGot[i];
      maxNumberOfAreasIndex = i;
    }
  }

  return {maxNumberOfAreas, maxNumberOfAreasIndex};
}

bool isEven(int x) { return x % 2 == 0; }
bool isOdd(int x) { return x % 2 == 1; }

std::vector<int> getMaxAreasGot(const std::vector<int>& a, size_t n) {
  auto [numberOfAreasGot1, prevArea1] = getMaxAreasGot(a, n, isEven);
  auto [numberOfAreasGot2, prevArea2] = getMaxAreasGot(a, n, isOdd);

  auto [maxNumberOfAreas1, maxNumberOfAreasIndex1] =
      getMaxNumberOfAreasAndPos(numberOfAreasGot1);
  auto [maxNumberOfAreas2, maxNumberOfAreasIndex2] =
      getMaxNumberOfAreasAndPos(numberOfAreasGot2);

  if (maxNumberOfAreas1 < maxNumberOfAreas2) {
    std::swap(maxNumberOfAreas1, maxNumberOfAreas2);
    std::swap(maxNumberOfAreasIndex1, maxNumberOfAreasIndex2);
    std::swap(prevArea1, prevArea2);
    std::swap(numberOfAreasGot1, numberOfAreasGot2);
  }

  int i = maxNumberOfAreasIndex1;
  std::vector<int> ans;
  while (i != -1) {
    ans.push_back(a[i]);
    i = prevArea1[i];
  }

  std::reverse(ans.begin(), ans.end());

  return ans;
}

int main() {
  size_t n = 0;
  std::cin >> n;

  std::vector<int> a(n);

  for (size_t i = 0; i < n; ++i) std::cin >> a[i];

  std::vector<int> numberOfAreasGot(n,
                                    0);  // numberOfAreasGot[i] - max number of
                                         // areas with i-th area in sequence

  std::vector<int> ans = getMaxAreasGot(a, n);

  std::cout << ans.size() << "\n";
  for (size_t i = 0; i < ans.size(); ++i) std::cout << ans[i] << " ";
}
