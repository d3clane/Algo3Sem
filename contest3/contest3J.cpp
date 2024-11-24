#include <stddef.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
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
    size_t row;
    size_t col;
  };

  using InitGameState = const int (&)[size_][size_];

  GameState(InitGameState gameState);

  void swapZeroElement(Position other);
  uint64_t encode() const;

  Position zeroPos() const;

 private:
  int state_[size_][size_];

  Position zeroPos_;
};

void GameState::swapZeroElement(GameState::Position other) {
  if (other.row >= size_ || other.col >= size_) {
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
        zeroPos_ = GameState::Position{i, j};
        break;
      }
    }
  }
}

GameState::Position GameState::zeroPos() const { return zeroPos_; }

uint64_t GameState::encode() const {
  uint64_t result = 0;

  const int* state = &state_[0][0];
  for (size_t i = 0; i < size_ * size_; ++i) {
    result = result * 10 + state[i];
  }

  return result;
}

struct PathToSolution {
  bool pathExist;
  std::string path;
};

struct Moves {
  int dx;
  int dy;
};

static const uint64_t invalidEncoding = 0;

struct PrevState {
  uint64_t stateEncoding = invalidEncoding;
  char move;
};

struct VertexInfo {
  int distance = -1;

  PrevState prevState = {invalidEncoding, 0};
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

std::map<uint64_t, VertexInfo> bfs(GameState begin, GameState end) {
  uint64_t endEncoding = end.encode();
  uint64_t beginEncoding = begin.encode();

  std::queue<GameState> statesQueue;
  statesQueue.push(begin);

  std::map<uint64_t, VertexInfo> vertexInfo;

  vertexInfo[beginEncoding] = VertexInfo{0, PrevState{invalidEncoding, 0}};

  std::vector<Moves> moves = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  while (!statesQueue.empty()) {
    GameState currentState = statesQueue.front();
    statesQueue.pop();

    uint64_t currentStateEncoding = currentState.encode();
    if (currentStateEncoding == endEncoding) {
      break;
    }

    VertexInfo currentStateInfo = vertexInfo[currentStateEncoding];

    for (Moves move : moves) {
      GameState nextState = currentState;
      nextState.swapZeroElement({currentState.zeroPos().row + move.dx,
                                 currentState.zeroPos().col + move.dy});
      uint64_t nextStateEncoding = nextState.encode();

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

  auto vertexInfoMap = bfs(begin, end);

  auto it = vertexInfoMap.find(end.encode());
  if (it == vertexInfoMap.end()) {
    result.pathExist = false;
    return result;
  }

  result.pathExist = true;

  for (auto state = it->second.prevState;
       state.stateEncoding != invalidEncoding;
       state = vertexInfoMap[state.stateEncoding].prevState) {
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