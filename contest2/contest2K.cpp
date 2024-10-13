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

int findCrossX(const Line& l1, const Line& l2) {
  assert(l1.slopeFactor != l2.slopeFactor);

  int x = (l2.yOffset - l1.yOffset) / (l1.slopeFactor - l2.slopeFactor);
  if (l2.yOffset - l1.yOffset < 0) --x;

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

  if (lines_.empty())
    linesIntersectionsX_.push_back(infinitelyRemotePointX);
  else
    linesIntersectionsX_.push_back(findCrossX(lines_.back(), line));

  lines_.push_back(line);
}

int LowerEnvelope::getY(int x) const {
  int lineIndex = std::lower_bound(linesIntersectionsX_.begin(),
                                   linesIntersectionsX_.end(), x) -
                  linesIntersectionsX_.begin() - 1;

  return lines_[lineIndex].getY(x);
}

int findMinSquareLens(const int n, const int k) {
  if (k == 0) {
    return 0;
  }

  static const int infDpVal = 1e9;

  std::vector<std::vector<int> > dp(n + 1, std::vector<int>(k + 1, infDpVal));

  LowerEnvelope envelope;

  for (int i = 0; i <= k; ++i) {
    dp[0][i] = 0;
  }

  static const Line zeroPointsLine{-2 * 1, 0 + 1 * 1};

  envelope.addLine(zeroPointsLine);

  for (int numberOfSegments = 1; numberOfSegments <= k; ++numberOfSegments) {
    LowerEnvelope newEnvelope;
    newEnvelope.addLine(zeroPointsLine);

    for (int pointId = 1; pointId <= n; ++pointId) {
      dp[pointId][numberOfSegments] =
          envelope.getY(pointId) + pointId * pointId;
      newEnvelope.addLine(
          Line{-2 * (pointId + 1),
               dp[pointId][numberOfSegments] + (pointId + 1) * (pointId + 1)});
    }

    envelope = newEnvelope;
  }

  return dp[n][k];
}

int main() {
  int n = 0, k = 0;
  std::cin >> n >> k;

  int answer = findMinSquareLens(n, k);

  std::cout << answer << "\n";
}
