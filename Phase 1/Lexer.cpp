#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<vector<int>> Transition;
vector<vector<int>> Advance;
vector<string> Keywords;

vector<vector<int>> getTransitionTable() {
  ifstream TransitionFile("TransitionTable.txt");

  if (!TransitionFile.is_open()) {
    cout << "Error: TransitionTable.txt not found" << endl;
    return {{}};
  }

  TransitionFile.close();
  return {{}};
}

vector<vector<int>> getAdvanceTable(vector<vector<int>> &Transition) {
  return {{}};
}

vector<string> getKeywords() {
  ifstream KeywordsFile("Keywords.txt");

  if (!KeywordsFile.is_open()) {
    cout << "Error: Keywords.txt not found" << endl;
    return {};
  }

  KeywordsFile.close();
  return {};
}

int main() { return 0; }