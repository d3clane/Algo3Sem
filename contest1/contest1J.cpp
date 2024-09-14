#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const int InvalidIndex = -10;

std::pair<std::vector<std::vector<std::pair<int, int> > > /* prevs */,
          int /* lcs */>
getLCS(const std::string& firstWord, const std::string& secondWord) {
  size_t firstWordSize = firstWord.size();
  size_t secondWordSize = secondWord.size();

  std::vector<std::vector<std::pair<int, int> > > prevState(
      firstWordSize, std::vector<std::pair<int, int> >(
                         secondWordSize, {InvalidIndex, InvalidIndex}));

  std::vector<std::vector<int> > lcs(firstWordSize,
                                     std::vector<int>(secondWordSize, 0));

  for (size_t i = 0; i < firstWordSize; ++i) {
    if (firstWord[i] == secondWord[0]) {
      lcs[i][0] = 1;
      prevState[i][0] = {i - 1, -1};
    }
  }
  for (size_t i = 0; i < secondWordSize; ++i) {
    if (firstWord[0] == secondWord[i]) {
      lcs[0][i] = 1;
      prevState[0][i] = {-1, i - 1};
    }
  }

  for (size_t i = 1; i < firstWordSize; ++i) {
    for (size_t j = 1; j < secondWordSize; ++j) {
      if (firstWord[i] == secondWord[j]) {
        lcs[i][j] = lcs[i - 1][j - 1] + 1;
        prevState[i][j] = {i - 1, j - 1};
      } else {
        if (lcs[i - 1][j] > lcs[i][j - 1]) {
          lcs[i][j] = lcs[i - 1][j];
          prevState[i][j] = {i - 1, j};
        } else {
          lcs[i][j] = lcs[i][j - 1];
          prevState[i][j] = {i, j - 1};
        }
      }
    }
  }

  return {prevState, lcs[firstWordSize - 1][secondWordSize - 1]};
}

void getLCSIndexes(
    const std::vector<std::vector<std::pair<int, int> > >& prevState,
    int firstWordPos, int secondWordPos,
    std::vector<std::pair<size_t, size_t> >& lcsIndexes) {
  if (firstWordPos < 0 || secondWordPos < 0) return;

  getLCSIndexes(prevState, prevState[firstWordPos][secondWordPos].first,
                prevState[firstWordPos][secondWordPos].second, lcsIndexes);

  if (prevState[firstWordPos][secondWordPos].first == firstWordPos - 1 &&
      prevState[firstWordPos][secondWordPos].second == secondWordPos - 1)
    lcsIndexes.push_back({firstWordPos, secondWordPos});
}

int main() {
  std::string firstWord, secondWord;

  std::cin >> firstWord >> secondWord;

  auto [prevState, lcs] = getLCS(firstWord, secondWord);

  std::vector<std::pair<size_t, size_t> > lcsIndexes;
  getLCSIndexes(prevState, (int)firstWord.size() - 1,
                (int)secondWord.size() - 1, lcsIndexes);

  std::cout << lcs << "\n";

  for (size_t i = 0; i < lcsIndexes.size(); ++i)
    std::cout << lcsIndexes[i].first + 1 << " ";

  std::cout << "\n";

  for (size_t i = 0; i < lcsIndexes.size(); ++i)
    std::cout << lcsIndexes[i].second + 1 << " ";
}
