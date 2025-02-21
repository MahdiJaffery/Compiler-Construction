#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

vector<vector<int>> Transition;
vector<vector<int>> Advance;
vector<string> Keywords;

vector<vector<int>> getTransitionTable() {
  ifstream TransitionFile("Transitions.txt");

  if (!TransitionFile.is_open()) {
    cout << "Error: TransitionTable.txt not found" << endl;
    return {{}};
  }

  string line;

  while (getline(TransitionFile, line)) {
    stringstream ss(line);
    string value;

    vector<int> row;

    while (ss >> value) {
      if (value == "-") // If value is "-", push -1
        row.push_back(-1);
      else
        row.push_back(stoi(value)); // Convert string to int
    }

    Transition.push_back(row);
    row.clear();
  }

  TransitionFile.close();
  return Transition;
}

vector<vector<int>> getAdvanceTable(vector<vector<int>> &Transition) {
  vector<int> row;

  for (int i = 0; i < Transition.size(); i++) {
    for (int j = 0; j < Transition[i].size() - 1; j++)
      row.push_back(Transition[i][j]);

    Advance.push_back(row);
    row.clear();
  }

  return Advance;
}

vector<string> getKeywords() {
  ifstream KeywordsFile("Keywords.txt");

  if (!KeywordsFile.is_open()) {
    cout << "Error: Keywords.txt not found" << endl;
    return {};
  }

  string line;
  string value;

  while (getline(KeywordsFile, line)) {
    stringstream ss(line);
    ss >> value;
    Keywords.push_back(value);
  }

  KeywordsFile.close();
  return {};
}

int main() {
  getTransitionTable();
  getAdvanceTable(Transition);
  getKeywords();

  //   cout << "\nTransition Table:\n";

  //   for (auto row : Transition) {
  //     for (auto cell : row) {
  //       cout << setw(5) << cell;
  //     }
  //     cout << endl;
  //   }

  //   cout << "\n\nAdvance Table:\n";

  //   for (auto row : Advance) {
  //     for (auto cell : row)
  //       cout << setw(5) << cell;
  //     cout << endl;
  //   }

//   cout << "\nKeywords:\n";

//   for (auto keyword : Keywords)
//     cout << keyword << endl;

  return 0;
}