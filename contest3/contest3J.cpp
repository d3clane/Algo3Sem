#include <stddef.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <vector>

class GameState {
 public:
  static const size_t size_ = 3;

  struct Position {
    int row;
    int col;
  };

  using InitGameState = const int (&)[size_][size_];

  GameState(InitGameState gameState);

  void swapZeroElement(Position other);
  int64_t encode() const;

  Position zeroPos() const;

 private:
  int state_[size_][size_];

  Position zeroPos_;
};

void GameState::swapZeroElement(GameState::Position other) {
  if (other.row < 0 || other.row >= size_ || other.col < 0 ||
      other.col >= size_) {
    return;
  }

  std::swap(state_[zeroPos_.row][zeroPos_.col], state_[other.row][other.col]);
  zeroPos_ = other;
}

GameState::GameState(InitGameState gameState) {
  std::copy(&gameState[0][0], &gameState[0][0] + size_ * size_, &state_[0][0]);

  for (size_t i = 0; i < size_; ++i) {
    for (size_t j = 0; j < size_; ++j) {
      if (state_[i][j] == 0) {
        zeroPos_ = {(int)i, (int)j};
        break;
      }
    }
  }
}

GameState::Position GameState::zeroPos() const { return zeroPos_; }

int64_t GameState::encode() const {
  int64_t result = 0;

  const int* state = &state_[0][0];
  for (int i = 0; i < size_ * size_; ++i) {
    result = result * 10 + state[i];
  }

  return result;
}

struct PathToSolution {
  bool pathExist;
  std::string path;
};

struct Moves {
  int dx, dy;
};

struct PrevState {
  int64_t stateEncoding;
  char move;
};

struct VertexInfo {
  int distance = -1;

  PrevState prevState = {-1, -1};
};

char moveToChar(Moves move) {
  if (move.dx == 1) {
    return 'D';
  } else if (move.dx == -1) {
    return 'U';
  } else if (move.dy == 1) {
    return 'R';
  } else {
    return 'L';
  }
}

std::map<int64_t, VertexInfo> bfs(GameState begin, GameState end) {
  int64_t endEncoding = end.encode();
  int64_t beginEncoding = begin.encode();

  std::queue<GameState> statesQueue;
  statesQueue.push(begin);

  std::map<int64_t, VertexInfo> vertexInfo;

  vertexInfo[beginEncoding] = {0, {-1, -1}};

  std::vector<Moves> moves = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  while (!statesQueue.empty()) {
    auto currentState = statesQueue.front();
    statesQueue.pop();

    int64_t currentStateEncoding = currentState.encode();
    if (currentStateEncoding == endEncoding) {
      break;
    }

    auto currentStateInfo = vertexInfo[currentStateEncoding];

    for (auto move : moves) {
      GameState nextState = currentState;
      nextState.swapZeroElement({currentState.zeroPos().row + move.dx,
                                 currentState.zeroPos().col + move.dy});
      int64_t nextStateEncoding = nextState.encode();

      auto it = vertexInfo.find(nextStateEncoding);
      if (it == vertexInfo.end()) {
        vertexInfo[nextStateEncoding] = {
            currentStateInfo.distance + 1,
            {currentStateEncoding, moveToChar(move)}};

        statesQueue.push(nextState);

        continue;
      }
    }
  }

  return vertexInfo;
}

PathToSolution findPath(GameState begin) {
  PathToSolution result;

  GameState end({{1, 2, 3}, {4, 5, 6}, {7, 8, 0}});

  auto vertexInfo = bfs(begin, end);

  auto it = vertexInfo.find(end.encode());
  if (it == vertexInfo.end()) {
    result.pathExist = false;
    return result;
  }

  result.pathExist = true;

  for (auto state = it->second.prevState; state.stateEncoding != -1;
       state = vertexInfo[state.stateEncoding].prevState) {
    result.path += state.move;
  }

  std::reverse(result.path.begin(), result.path.end());

  return result;
}

GameState readBeginState() {
  int begin[GameState::size_][GameState::size_] = {};

  for (size_t i = 0; i < GameState::size_; ++i) {
    for (size_t j = 0; j < GameState::size_; ++j) {
      std::cin >> begin[i][j];
    }
  }

  return GameState(begin);
}

void printPathToSolution(const PathToSolution& path) {
  if (path.pathExist) {
    std::cout << path.path.length() << "\n";
    std::cout << path.path << "\n";
  } else {
    std::cout << "-1" << "\n";
  }
}

int main() {
  GameState beginState = readBeginState();

  PathToSolution path = findPath(beginState);

  printPathToSolution(path);
}