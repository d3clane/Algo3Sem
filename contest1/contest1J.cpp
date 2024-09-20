#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const int InvalidIndex = -10;

struct LcsInfo
{
  int lcs;
  std::vector< std::pair<size_t, size_t> > lcsIndexes;
};

struct IthLcsInfo
{
  int lcs;
  std::pair<int, int> prevIndex;

  IthLcsInfo() { lcs = 0; prevIndex = { InvalidIndex, InvalidIndex }; }
};

std::vector<std::pair<size_t, size_t> > getLCSIndexes(
    const std::vector< std::vector<IthLcsInfo> >& lcs,
    int firstWordBeginPos, int secondWordBeginPos) {
  std::vector<std::pair<size_t, size_t> > indexes;
  
  std::pair<int, int> indexesNow = {firstWordBeginPos, secondWordBeginPos};
  while (indexesNow.first >= 0 && indexesNow.second >= 0)
  {
    std::pair<int, int> prevIndexes = lcs[indexesNow.first][indexesNow.second].prevIndex;
    
    if (prevIndexes.first  == indexesNow.first - 1 && 
        prevIndexes.second == indexesNow.second - 1)
        indexes.push_back({indexesNow.first, indexesNow.second});

    indexesNow = prevIndexes;
  }

  std::reverse(indexes.begin(), indexes.end());

  return indexes;
}

LcsInfo getLcs(const std::string& firstWord, const std::string& secondWord) {
  size_t firstWordSize = firstWord.size();
  size_t secondWordSize = secondWord.size();

  std::vector< std::vector<IthLcsInfo> > lcs(firstWordSize, std::vector<IthLcsInfo>(secondWordSize));

  for (size_t i = 0; i < firstWordSize; ++i) {
    if (firstWord[i] == secondWord[0]) {
      lcs[i][0].lcs = 1;
      lcs[i][0].prevIndex = {i - 1, -1};
    }
  }
  for (size_t i = 0; i < secondWordSize; ++i) {
    if (firstWord[0] == secondWord[i]) {
      lcs[0][i].lcs = 1;
      lcs[0][i].prevIndex = {-1, i - 1};
    }
  }

  for (size_t i = 1; i < firstWordSize; ++i) {
    for (size_t j = 1; j < secondWordSize; ++j) {
      if (firstWord[i] == secondWord[j]) {
        lcs[i][j].lcs = lcs[i - 1][j - 1].lcs + 1;
        lcs[i][j].prevIndex = {i - 1, j - 1};
      } else {
        if (lcs[i - 1][j].lcs > lcs[i][j - 1].lcs) {
          lcs[i][j] = lcs[i - 1][j];
        } else {
          lcs[i][j] = lcs[i][j - 1];
        }
      }
    }
  }

  LcsInfo result;
  result.lcs = lcs[firstWordSize - 1][secondWordSize - 1].lcs;
  result.lcsIndexes = getLCSIndexes(lcs, firstWordSize - 1, secondWordSize - 1);

  return result;
}

void printAnswer(const LcsInfo& lcs)
{
  std::cout << lcs.lcs << "\n";

  for (size_t i = 0; i < lcs.lcsIndexes.size(); ++i)
    std::cout << lcs.lcsIndexes[i].first + 1 << " ";

  std::cout << "\n";

  for (size_t i = 0; i < lcs.lcsIndexes.size(); ++i)
    std::cout << lcs.lcsIndexes[i].second + 1 << " ";
}

int main() {
  std::string firstWord, secondWord;

  std::cin >> firstWord >> secondWord;

  LcsInfo lcs = getLcs(firstWord, secondWord);

  printAnswer(lcs);
}
