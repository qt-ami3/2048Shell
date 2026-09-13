
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
#include <cstdlib>
#include <filesystem>
#include "./include/sysInfo.hpp"
#include "./include/functions.hpp"
#include "../libs/inih/INIReader.h"

#ifdef PORTABLE_BUILD
 //Portable build: every data file lives next to the binary, so the game runs
 //straight out of build/ without anything being installed under /usr.
string binDir() {
  error_code ec;
  filesystem::path exe = filesystem::read_symlink("/proc/self/exe", ec);
  if (ec) {
    return ".";
  }
  return exe.parent_path().string();
}
#endif

string getLeaderBoardPath() {
#ifdef PORTABLE_BUILD
  return binDir() + "/leaderBoard.ini";
#else
  const char* xdg = getenv("XDG_DATA_HOME");
  string dir;
  if (xdg && xdg[0]) {
    dir = string(xdg) + "/2048shell";
  } else {
    const char* home = getenv("HOME");
    dir = string(home ? home : ".") + "/.local/share/2048shell";
  }
  filesystem::create_directories(dir);
  return dir + "/leaderBoard.ini";
#endif
}

string getNoticePath() {
#ifdef PORTABLE_BUILD
  return binDir() + "/copywriteNotice.txt";
#else
  return "/usr/share/2048shell/copywriteNotice.txt";
#endif
}

string getLicensePath() {
#ifdef PORTABLE_BUILD
  return binDir() + "/LICENSE";
#else
  return "/usr/share/licenses/2048shell/LICENSE";
#endif
}

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

int scoreCheck(int score, int& first, int& second, int& third,
               const string& firstName, const string& secondName, const string& thirdName,
               const string& iniPath) {
  int pos = 0;
  if (score > first) {
    first = score; pos = 1;
  } else if (score > second) {
    second = score; pos = 2;
  } else if (score > third) {
    third = score; pos = 3;
  } else {
    return 0;
  }
  ofstream ini(iniPath);
  ini << "[leaderBoard]\n"
    << "first="      << first      << "\n"
    << "firstName="  << firstName  << "\n"
    << "second="     << second     << "\n"
    << "secondName=" << secondName << "\n"
    << "third="      << third      << "\n"
    << "thirdName="  << thirdName  << "\n";
  return pos;
}

void printGame(int playingGrid[4][4], int& first, int& second, int& third,
               const string& firstName, const string& secondName, const string& thirdName,
               int score) {
  bool isFirst  = score > 0 && score == first;
  bool isSecond = !isFirst && score > 0 && score == second;
  bool isThird  = !isFirst && !isSecond && score > 0 && score == third;

  const int W = 28;
  auto visLen = [](const string& s) -> int {
    int n = 0; bool esc = false;
    for (char c : s) {
      if (c == '\033') { esc = true; continue; }
      if (esc) { if (c == 'm') esc = false; continue; }
      n++;
    }
    return n;
  };
  auto makeLabel = [&](const string& prefix, int val, const string& name, const string& suffix = "") -> string {
    string valStr = to_string(val);
    string nameStr = name.size() > 12 ? name.substr(0, 12) : name;
    string s = prefix + valStr;
    if (!nameStr.empty()) s += " " + nameStr;
    int vis = visLen(prefix) + (int)valStr.size() + (nameStr.empty() ? 0 : 1 + (int)nameStr.size());
    while (vis++ < W) s += ' ';
    s += suffix;
    return s;
  };

  string labels[4] = {
    makeLabel(isFirst  ? "\033[48;5;209m\033[30m Current 1st: " : " 1st: ",  first,  firstName,  isFirst  ? "\033[0m" : ""),
    makeLabel(isSecond ? "\033[48;5;220m\033[30m Current 2nd: " : " 2nd: ", second, secondName, isSecond ? "\033[0m" : ""),
    makeLabel(isThird  ? "\033[48;5;230m\033[30m Current 3rd: " : " 3rd: ",  third,  thirdName,  isThird  ? "\033[0m" : ""),
    (isFirst || isSecond || isThird) ? string(W, ' ') : makeLabel("\033[48;5;230m\033[30m Score: ", score, "", "\033[0m")
  };

  const string hbar  = "────────────────────────────";
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
  cout << "└────┴────┴────┴────┘" << hbar << "┘" << endl;
}

int main() {

  bool won = false;

  string lbPath = getLeaderBoardPath();
  INIReader reader(lbPath);

  if (reader.ParseError() < 0) {
    cout<<"failed to load leaderBoard\n";
  }

  int lbFirst  = reader.GetInteger("leaderBoard", "first",  0);
  int lbSecond = reader.GetInteger("leaderBoard", "second", 0);
  int lbThird  = reader.GetInteger("leaderBoard", "third",  0);

  string lbFirstName  = reader.Get("leaderBoard", "firstName",  "");
  string lbSecondName = reader.Get("leaderBoard", "secondName", "");
  string lbThirdName  = reader.Get("leaderBoard", "thirdName",  "");

  int score = 0;

  ifstream file(getNoticePath());

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
    printGame(playingGrid, lbFirst, lbSecond, lbThird, lbFirstName, lbSecondName, lbThirdName, score);
    //printSysInfo(ramEnabled);

  setBufferedInput(false);

  int origFirst = lbFirst, origSecond = lbSecond;
  string origFirstName = lbFirstName, origSecondName = lbSecondName, origThirdName = lbThirdName;
  int bestPosition = 0;

  while (true) { //main loop.
    char cont = getchar();

    if (cont == 'q') {
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
          getScore(playingGrid, score);
          int pos = scoreCheck(score, lbFirst, lbSecond, lbThird, lbFirstName, lbSecondName, lbThirdName, lbPath);
          if (pos > 0 && (bestPosition == 0 || pos < bestPosition)) {
            bestPosition = pos;
          }
          clearScreen();
          printGame(playingGrid, lbFirst, lbSecond, lbThird, lbFirstName, lbSecondName, lbThirdName, score);
          //printSysInfo(ramEnabled);

      if (!canMove(playingGrid)) { //lose condition.
        cout <<"Game Over!" << endl << "score: "<< score << endl;
        break;
      }
    }

    for (int i = 0; i < 4; i++) {
      for (int j = 0; j<4; j++) {
        if (playingGrid[i][j]==2048 && !won) { //win condition
          cout<<endl;
          cout<<"Congrats! You win! Keep playing for a higher score or press 'q' to quit."<<endl<<"score : "<<score<<endl;
          std::this_thread::sleep_for(std::chrono::seconds(2));
          won = true;
        }
      }
    }
  } //End of main game loop.

  if (bestPosition > 0) {
    setBufferedInput(true);
    tcflush(STDIN_FILENO, TCIFLUSH);
    cout << endl << "You reached #" << bestPosition << "! Enter your name: ";
    string playerName;
    cin >> playerName;
    if (bestPosition == 1) {
      lbThird = origSecond; lbThirdName = origSecondName;
      lbSecond = origFirst; lbSecondName = origFirstName;
      lbFirst = score; lbFirstName = playerName;
    } else if (bestPosition == 2) {
      lbThird = origSecond; lbThirdName = origSecondName;
      lbSecond = score; lbSecondName = playerName;
      lbFirst = origFirst; lbFirstName = origFirstName;
    } else {
      lbThird = score; lbThirdName = playerName;
      lbSecond = origSecond; lbSecondName = origSecondName;
      lbFirst = origFirst; lbFirstName = origFirstName;
    }
    ofstream ini(lbPath);
    ini << "[leaderBoard]\n"
      << "first="      << lbFirst      << "\n"
      << "firstName="  << lbFirstName  << "\n"
      << "second="     << lbSecond     << "\n"
      << "secondName=" << lbSecondName << "\n"
      << "third="      << lbThird      << "\n"
      << "thirdName="  << lbThirdName  << "\n";
  }

  setBufferedInput(true);

}

else if (noticeA=="d") {

  ifstream file(getLicensePath());

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
