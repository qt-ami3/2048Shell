#include <iostream>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <random>
#include <cstdlib>

using namespace std;

void clearScreen() { //self explanetory, clears screen.
  #ifdef _WIN32
    system("cls");
  #else
    cout<<"\033[2J\033[H";
  #endif
}

void setBufferedInput(bool enable) { //sets terminal input to repeat and detect without `enter`.
  static bool enabled = true;
  static struct termios old;
  struct termios newt;

  if (enable && !enabled) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    enabled = true;
  }
  else if (!enable && enabled) {
    tcgetattr(STDIN_FILENO, &old);
    newt = old;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    enabled = false;
  }
}

int getRandomNumber(int min, int max) { //also pretty self explanetory, generates a random number based on a minimum and maximum.
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> distr(min, max);
  return distr(gen);
}

bool newRandomBox(int playingGrid[4][4]) { //prints new random box with '2' everytime the player makes a valid move
  vector<pair<int, int>> emptyCells;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (playingGrid[i][j] == 0) {
        emptyCells.push_back({i, j});
      }
    }
  }
  
  if (emptyCells.empty()) {
    return false;
  }
  
  auto [row, col] = emptyCells[getRandomNumber(0, emptyCells.size() - 1)];
  playingGrid[row][col] = (getRandomNumber(0, 9) < 9) ? 2 : 4;
  return true;
}

//lines 130-281 contains the functions for moving values through the array.

bool moveLeft(int grid[4][4]) {
  bool moved = false;
  for (int i = 0; i < 4; i++) {
    vector<int> row;
    for (int j = 0; j < 4; j++) {
      if (grid[i][j] != 0) {
        row.push_back(grid[i][j]);
      }
    }

    for (int j = 0; j < (int)row.size() - 1; j++) {
      if (row[j] == row[j + 1]) {
        row[j] *= 2;
        row.erase(row.begin() + j + 1);
        moved = true;
      }
    }

    while (row.size() < 4) {
      row.push_back(0);
    }
    
    for (int j = 0; j < 4; j++) {
      if (grid[i][j] != row[j]) {
        grid[i][j] = row[j];
        moved = true;
      }
    }
  }
  return moved;
}

bool moveRight(int grid[4][4]) {
  bool moved = false;
  for (int i = 0; i < 4; i++) {
    vector<int> row;
    for (int j = 3; j >= 0; j--) {
      if (grid[i][j] != 0)
        row.push_back(grid[i][j]);
    }

    for (int j = 0; j < (int)row.size() - 1; j++) {
      if (row[j] == row[j + 1]) {
        row[j] *= 2;
        row.erase(row.begin() + j + 1);
        moved = true;
      }
    }

    while (row.size() < 4)
      row.push_back(0);

    for (int j = 0; j < 4; j++) {
      if (grid[i][3 - j] != row[j]) {
        grid[i][3 - j] = row[j];
        moved = true;
      }
    }
  }
  return moved;
}

bool moveUp(int grid[4][4]) {
  bool moved = false;
  for (int j = 0; j < 4; j++) {
    vector<int> col;
    for (int i = 0; i < 4; i++) {
      if (grid[i][j] != 0) {
      col.push_back(grid[i][j]);
      }
    }

    for (int i = 0; i < (int)col.size() - 1; i++) {
      if (col[i] == col[i + 1]) {
        col[i] *= 2;
        col.erase(col.begin() + i + 1);
        moved = true;
      }
    }

    while (col.size() < 4)
      col.push_back(0);

    for (int i = 0; i < 4; i++) {
      if (grid[i][j] != col[i]) {
        grid[i][j] = col[i];
        moved = true;
      }
    }
  }
  return moved;
}

bool moveDown(int grid[4][4]) {
  bool moved = false;
  for (int j = 0; j < 4; j++) {
    vector<int> col;
    for (int i = 3; i >= 0; i--) {
      if (grid[i][j] != 0)
        col.push_back(grid[i][j]);
    }
    for (int i = 0; i < (int)col.size() - 1; i++) {
      if (col[i] == col[i + 1]) {
        col[i] *= 2;
        col.erase(col.begin() + i + 1);
        moved = true;
      }
    }

    while (col.size() < 4)
      col.push_back(0);

    for (int i = 0; i < 4; i++) {
      if (grid[3 - i][j] != col[i]) {
        grid[3 - i][j] = col[i];
        moved = true;
      }
    }
  }
  return moved;
}

bool canMove(int grid[4][4]) { //can it move?
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (grid[i][j] == 0) {  
        return true;
      } 
    } 
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {  
      if (grid[i][j] == grid[i][j + 1]) {
        return true;
      }
    }
  }

  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 3; i++) {
      if (grid[i][j] == grid[i + 1][j]) {
        return true;
      }
    }
  }
  return false;
}

void getScore (int array[4][4],int &score) { //gets total score of all the different cubes combines.
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      score = score + array[i][j];
    }
  } 
}

