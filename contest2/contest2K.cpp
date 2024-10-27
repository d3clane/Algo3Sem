#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

struct Point {
  int x, y;

  explicit Point(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Line {
  int slopeFactor, yOffset;

  explicit Line(int slopeFactor = 0, int yOffset = 0)
      : slopeFactor(slopeFactor), yOffset(yOffset) {}

  int getY(int x) const { return slopeFactor * x + yOffset; }
};

class LowerEnvelope {
  std::vector<Line> lines_;
  std::vector<int> linesIntersectionsX_;

 public:
  LowerEnvelope();

  void addLine(const Line& line);

  int getY(int x) const;
};

int findCrossX(const Line& line1, const Line& line2) {
  assert(line1.slopeFactor != line2.slopeFactor);

  int x =
      (line2.yOffset - line1.yOffset) / (line1.slopeFactor - line2.slopeFactor);
  if (line2.yOffset - line1.yOffset < 0) --x;

  return x;
}

LowerEnvelope::LowerEnvelope() {
  static const int inf = 1e9;

  lines_.push_back(Line(0, inf));
  linesIntersectionsX_.push_back(-inf);
}

void LowerEnvelope::addLine(const Line& line) {
  int intersectionX = linesIntersectionsX_.back();

  while (lines_.size() != 0 &&
         lines_.back().getY(intersectionX) > line.getY(intersectionX)) {
    lines_.pop_back();
    linesIntersectionsX_.pop_back();

    intersectionX = linesIntersectionsX_.back();
  }

  static const int infinitelyRemotePointX = -1e9;

  if (lines_.empty()) {
    linesIntersectionsX_.push_back(infinitelyRemotePointX);
  } else {
    linesIntersectionsX_.push_back(findCrossX(lines_.back(), line));
  }

  lines_.push_back(line);
}

int LowerEnvelope::getY(int x) const {
  int lineIndex = std::lower_bound(linesIntersectionsX_.begin(),
                                   linesIntersectionsX_.end(), x) -
                  linesIntersectionsX_.begin() - 1;

  return lines_[lineIndex].getY(x);
}

int findMinSquareLens(const size_t nPoints, const size_t nSegments) {
  if (nSegments == 0) {
    return 0;
  }

  static const int infDpVal = 1e9;

  std::vector<std::vector<int> > dp(nPoints + 1,
                                    std::vector<int>(nSegments + 1, infDpVal));

  LowerEnvelope envelope;

  for (int i = 0; i <= nSegments; ++i) {
    dp[0][i] = 0;
  }

  static const Line zeroPointsLine{-2 * 1, 0 + 1 * 1};

  envelope.addLine(zeroPointsLine);

  for (size_t nUsedSegments = 1; nUsedSegments <= nSegments; ++nUsedSegments) {
    LowerEnvelope newEnvelope;
    newEnvelope.addLine(zeroPointsLine);

    for (size_t pointId = 1; pointId <= nPoints; ++pointId) {
      dp[pointId][nUsedSegments] = envelope.getY(pointId) + pointId * pointId;
      newEnvelope.addLine(
          Line{-2 * (pointId + 1),
               dp[pointId][nUsedSegments] + (pointId + 1) * (pointId + 1)});
    }

    envelope = newEnvelope;
  }

  return dp[nPoints][nSegments];
}

int main() {
  size_t nPoints = 0, nSegments = 0;
  std::cin >> nPoints >> nSegments;

  int minSquareLens = findMinSquareLens(nPoints, nSegments);

  std::cout << minSquareLens << "\n";

  return 0;
}
