#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

int symbolTableID = 1;

vector<vector<int>> Transition;
vector<vector<int>> Advance;
vector<string> Keywords;
set<string> SymbolSet;
set<string> KeywordSet;

string removeTrailingSpaces(string str) {
  int i = 0;
  while (str[i] == ' ')
    i++;
  return str.substr(i, str.length());
}

bool isSpace(string str) {
  for (auto c : str)
    if (c != ' ')
      return false;
  return true;
}

bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isDigit(char c) { return c >= '0' && c <= '9'; }

bool isKeyword(string lexeme) {
  for (auto keyword : Keywords)
    if (lexeme == keyword)
      return true;
  return false;
}

int getMapped(char c) {
  if (isAlpha(c))
    return 0;
  else if (c == '_')
    return 1;
  else if (isDigit(c))
    return 2;
  else if (c == '+')
    return 3;
  else if (c == '-')
    return 4;
  else if (c == '.')
    return 5;
  else if (c == 'E')
    return 6;
  else if (c == ':')
    return 9;
  else if (c == '{')
    return 10;
  else if (c == '[')
    return 11;
  else if (c == '(')
    return 12;
  else if (c == ')')
    return 13;
  else if (c == ']')
    return 14;
  else if (c == '}')
    return 15;
  else if (!isAlpha(c) && !isDigit(c) && c != '_')
    return 7;
  else if (!isDigit(c) && c != '.')
    return 8;
  else
    return -2;
}

int Accept(int state) { return Transition[state].back(); }

void toSymbolTable(string lexeme) {
  if (SymbolSet.find(lexeme) != SymbolSet.end())
    return;

  SymbolSet.insert(lexeme);
  ofstream SymbolTable("SymbolTable.txt", ios::app);

  if (!SymbolTable.is_open()) {
    cout << "Error: SymbolTable.txt not found" << endl;
    return;
  }

  SymbolTable << "<" << symbolTableID++ << ", " << lexeme << ">" << endl;
  SymbolTable.close();

  return;
}

void toErrorTable(string inValidLex) {
  ofstream ErrorTable("ErrorTable.txt", ios::app);

  if (!ErrorTable.is_open()) {
    cout << "Error: ErrorTable.txt not found" << endl;
    return;
  }

  if (!isSpace(inValidLex)) {
    inValidLex = removeTrailingSpaces(inValidLex);
    ErrorTable << inValidLex << endl;
  }

  ErrorTable.close();

  return;
}

void toPunctuationTable(string lexeme) {
  ofstream PunctuationTable("PunctuationTable.txt", ios::app);

  if (!PunctuationTable.is_open()) {
    cout << "Error: PunctuationTable.txt not found" << endl;
    return;
  }

  lexeme = removeTrailingSpaces(lexeme);
  PunctuationTable << lexeme << endl;
  PunctuationTable.close();

  return;
}

void toKeywordTable(string lexeme) {
  if (KeywordSet.find(removeTrailingSpaces(lexeme)) != KeywordSet.end())
    return;

  ofstream KeywordTable("KeywordTable.txt", ios::app);

  if (!KeywordTable.is_open()) {
    cout << "Error: KeywordTable.txt not found" << endl;
    return;
  }

  lexeme = removeTrailingSpaces(lexeme);
  KeywordSet.insert(lexeme);

  KeywordTable << lexeme << endl;
  KeywordTable.close();

  return;
}

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
      else if (value[0] >= 'A' && value[0] <= 'Z')
        row.push_back(int(value[0] - 55));
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

vector<string> getLexemes() {
  ifstream LexemesFile("Source.txt");

  if (!LexemesFile.is_open()) {
    cout << "Error: Lexemes.txt not found" << endl;
    return {};
  }

  string line;
  int state = 0, ch;

  char *bufferPointer, *forwardPointer;

  while (getline(LexemesFile, line)) {
    line = removeTrailingSpaces(line);

    bufferPointer = &line[0];
    forwardPointer = bufferPointer;

    while (bufferPointer < &line[line.size()] &&
           forwardPointer < &line[line.size()]) {

      ch = getMapped(*forwardPointer);
      state = Transition[state][ch];

      if (state == -1) {

        if (forwardPointer - bufferPointer == 0)
          toErrorTable(string(bufferPointer, forwardPointer + 1));
        else if (!isKeyword(string(bufferPointer, forwardPointer)))
          toErrorTable(string(bufferPointer, forwardPointer));
        else
          toKeywordTable(string(bufferPointer, forwardPointer));

        forwardPointer++;
        bufferPointer = forwardPointer;
        state = 0;
      } else if (Accept(state) == 0)
        forwardPointer++;
      else if (Accept(state) == 1) {
        forwardPointer++;
        state = 0;

        string lexeme = string(bufferPointer, forwardPointer);

        if (!isKeyword(lexeme))
          toPunctuationTable(lexeme);

        bufferPointer = forwardPointer;
      } else {
        state = 0;

        string lexeme = string(bufferPointer, forwardPointer);
        forwardPointer++;

        if (!isKeyword(lexeme))
          toSymbolTable(lexeme);

        // forwardPointer--;
        bufferPointer = forwardPointer;
      }
    }
  }

  return {};
}

int main() {
  getTransitionTable();
  getAdvanceTable(Transition);
  getKeywords();

  // cout << "\nTransition Table:\n";

  // for (auto row : Transition) {
  //   for (auto cell : row) {
  //     cout << setw(5) << cell;
  //   }
  //   cout << endl;
  // }

  // cout << "\n\nAdvance Table:\n";

  // for (auto row : Advance) {
  //   for (auto cell : row)
  //     cout << setw(5) << cell;
  //   cout << endl;
  // }

  //   cout << "\nKeywords:\n";

  //   for (auto keyword : Keywords)
  //     cout << keyword << endl;

  getLexemes();

  return 0;
}