
  //2048 clone in the linux terminal.
  //2048 clone Copyright (c) 2007 Free Software Foundation, Inc. <https://fsf.org/>
  //author: River Knuuttila (common alias: Annie, Ann, Annie Knuuttila)

  //This program is free software: you can redistribute it and/or modify
  //it under the terms of the GNU General Public License as published by
  //the Free Software Foundation, either version 3 of the License, or
  //(at your option) any later version.

  //This program is distributed in the hope that it will be useful,
  //but WITHOUT ANY WARRANTY; without even the implied warranty of
  //MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  //GNU General Public License for more details.

  //You should have received a copy of the GNU General Public License
  //along with this program.  If not, see <https://www.gnu.org/licenses/>.
  
  //Concerns or questions? Reach out at: riverknuuttila2@outlook.com

#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "./include/sysInfo.hpp"
#include "./include/functions.hpp"
#include "../libs/inih/INIReader.h"
  
string tileColor(int value) {
  switch (value) {
    case 0:    return "\033[48;5;187m"; // empty (#cdc1b4)
    case 2:    return "\033[48;5;254m"; // #eee4da
    case 4:    return "\033[48;5;230m"; // #ede0c8
    case 8:    return "\033[48;5;215m"; // #f2b179
    case 16:   return "\033[48;5;209m"; // #f59563
    case 32:   return "\033[48;5;208m"; // #f67c5f
    case 64:   return "\033[48;5;202m"; // #f65e3b
    case 128:  return "\033[48;5;221m"; // #edcf72
    case 256:  return "\033[48;5;220m"; // #edcc61
    case 512:  return "\033[48;5;178m"; // #edc850
    case 1024: return "\033[48;5;178m"; // #edc53f
    case 2048: return "\033[48;5;220m"; // #edc22e
    default:   return "\033[48;5;187m"; // fallback (empty)
  }
}

void emptyBox() {
  cout << "│" << tileColor(0) << "    " << "\033[0m";
}

void printBox(int displayNumber) {
  string bg = tileColor(displayNumber);
  string fg = "\033[38;5;0m"; // black text

  if (displayNumber > 0) {
    if (displayNumber >= 1000) {
      cout << "│" << bg << fg << displayNumber << "\033[0m";
      }
    else if (displayNumber >= 100) {
      cout << "│" << bg << fg << displayNumber << " " << "\033[0m";
      }
    else if (displayNumber >= 10) {
      cout << "│" << bg << fg << displayNumber << "  " << "\033[0m";
      }
    else {
      cout << "│" << bg << fg << " " << displayNumber << "  " << "\033[0m";
    }
  }
  else {
    emptyBox();
  }
}

void scoreCheck(int score, int& first, int& second, int& third, const string& iniPath) {
  if (score > first) {
    first = score;
  } else if (score > second) {
    second = score;
  } else if (score > third) {
    third = score;
  } else {
    return;
  }
  ofstream ini(iniPath);
  ini << "[leaderBoard]\n"
    << "first="  << first  << "\n"
    << "second=" << second << "\n"
    << "third="  << third  << "\n";
}

void printGame(int playingGrid[4][4], int& first, int& second, int& third, int score) {
  bool isFirst  = score > 0 && score == first;
  bool isSecond = !isFirst && score > 0 && score == second;
  bool isThird  = !isFirst && !isSecond && score > 0 && score == third;

  const int W = 20;
  auto visLen = [](const string& s) -> int {
    int n = 0; bool esc = false;
    for (char c : s) {
      if (c == '\033') { esc = true; continue; }
      if (esc) { if (c == 'm') esc = false; continue; }
      n++;
    }
    return n;
  };
  auto makeLabel = [&](const string& prefix, int val, const string& suffix = "") -> string {
    string valStr = to_string(val);
    string s = prefix + valStr;
    int vis = visLen(prefix) + (int)valStr.size();
    while (vis++ < W) s += ' ';
    s += suffix;
    return s;
  };

  string labels[4] = {
    makeLabel(isFirst  ? "\033[48;5;209m\033[30m Current 1st: " : " 1st: ",  first,  isFirst  ? "\033[0m" : ""),
    makeLabel(isSecond ? "\033[48;5;220m\033[30m Current 2nd: " : " 2nd: ", second, isSecond ? "\033[0m" : ""),
    makeLabel(isThird  ? "\033[48;5;230m\033[30m Current 3rd: " : " 3rd: ",  third,  isThird  ? "\033[0m" : ""),
    (isFirst || isSecond || isThird) ? string(W, ' ') : makeLabel("\033[48;5;230m\033[30m Score: ", score, "\033[0m")
  };

  const string hbar  = "────────────────────";
  const string blank(W, ' ');

  cout << "┌────┬────┬────┬────┐" << hbar << "┐" << endl;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printBox(playingGrid[i][j]);
    }
    cout << "│" << labels[i] << "│" << endl;

    if (i < 3) {
      cout << "├────┼────┼────┼────┤" << blank << "│" << endl;
    }
  }
  cout << "└────┴────┴────┴────┘" << hbar << "┘";
}

int main() {

  bool won = false;

  INIReader reader("../usr/leaderBoard.ini");
  
  if (reader.ParseError() < 0) {
    cout<<"failed to load leaderBoard\n";
  }

  int lbFirst  = reader.GetInteger("leaderBoard", "first",  0);
  int lbSecond = reader.GetInteger("leaderBoard", "second", 0);
  int lbThird  = reader.GetInteger("leaderBoard", "third",  0);

  int score = 0;
  
  ifstream file("../assets/copywriteNotice.txt");
    
  if (!file) { 
    cerr<<"Could not open the file!"<<endl;
    return 1;
  }

  stringstream uffer;
  uffer<<file.rdbuf();

  string contentS=uffer.str();

  cout<<contentS<<endl;

  string noticeA;

  cin>>noticeA;

if (noticeA=="r") {
  
  //cout << "Enable RAM readings? (y/n): ";
  //string ramToggle;
  //cin >> ramToggle;
  //bool ramEnabled = (ramToggle == "y");

  int playingGrid[4][4] = {0}; //multidimensional array initialized to zero. represented by logic of functions above.

  newRandomBox(playingGrid);
    newRandomBox(playingGrid);

  getScore(playingGrid, score);

  clearScreen();
    printGame(playingGrid, lbFirst, lbSecond, lbThird, score);
    //printSysInfo(ramEnabled);

  setBufferedInput(false);

  while (true) { //main loop.
    char cont = getchar();

    if (cont == 'q') {
      cout << endl;
        scoreCheck(score, lbFirst, lbSecond, lbThird, "../usr/leaderBoard.ini");
      break;
    }

    bool moved = false;

    if (cont == '\033') {
      if (getchar() == '[') {
        char direction = getchar();
        switch (direction) { //switch statement for player inputs
        case 'A': // up key
          moved = moveUp(playingGrid);
            break;
        case 'B': // down key
            moved = moveDown(playingGrid);
            break;
        case 'C': // right key
          moved = moveRight(playingGrid);
            break;
        case 'D': // left key
          moved = moveLeft(playingGrid);
            break;
        }
      }
    }

    if (moved) {
      newRandomBox(playingGrid);
        clearScreen();
          getScore(playingGrid, score);
          scoreCheck(score, lbFirst, lbSecond, lbThird, "../usr/leaderBoard.ini");
          printGame(playingGrid, lbFirst, lbSecond, lbThird, score);
          //printSysInfo(ramEnabled);

      if (!canMove(playingGrid)) { //lose condition.
        cout<< endl <<"Game Over!" << endl << "score: "<< score << endl;
        scoreCheck(score, lbFirst, lbSecond, lbThird, "../usr/leaderBoard.ini");
        break;
      }
    }

    for (int i = 0; i < 4; i++) {
      for (int j = 0; j<4; j++) {
        if (playingGrid[i][j]==2048 && !won) { //win condition
          getScore(playingGrid,score);
          cout<<endl;

          cout<<"Congrats! You win! Keep playing for a higher score or press 'q' to quit."<<endl<<"score : "<<score<<endl; 
          scoreCheck(score, lbFirst, lbSecond, lbThird, "../usr/leaderBoard.ini");
          std::this_thread::sleep_for(std::chrono::seconds(2));
          won = true;
        }
      }
    }
  } //End of main game loop.

  setBufferedInput(true);
}
else if (noticeA=="d") {
  
  ifstream file("../LICENSE");
  
  if (!file) { 
    cerr<<"Could not open the file!"<<endl;
    return 1;
  }

  stringstream uuffer;
  uuffer<<file.rdbuf();

  string ccontentS=uuffer.str();

  cout<<ccontentS<<endl;
}
else if (noticeA=="q") {
  cout<<"Exiting!";
}

return 0;}
