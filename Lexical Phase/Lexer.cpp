#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

int symbolTableID = 1, keywordTableID = 1, puncTableID = 1, literalID = 1;

vector<vector<int>> Transition;
vector<vector<int>> Advance;
vector<string> Keywords;

set<pair<string, int>> SymbolSet;
set<pair<string, int>> KeywordSet;
set<pair<string, int>> PunctuationSet;
set<pair<string, int>> LiteralSet;

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
  else if (c == '{')
    return 7;
  else if (c == '[')
    return 8;
  else if (c == '(')
    return 9;
  else if (c == ')')
    return 10;
  else if (c == ']')
    return 11;
  else if (c == '}')
    return 12;
  else if (c == ':')
    return 13;
  else if (c == '<')
    return 14;
  else if (c == '>')
    return 15;
  else if (c == '=')
    return 16;
  else if (c == '!')
    return 17;
  else if (c == '|')
    return 18;
  else if (c == '&')
    return 19;
  else if (c == '%')
    return 20;
  else if (c == '\"')
    return 21;
  else if (c == '*')
    return 22;
  else if (c == '/')
    return 23;
  else if (!isAlpha(c) && !isDigit(c) && c != '_')
    return 24;
  else if (c != 'E' && !isDigit(c) && c != '.')
    return 25;
  else if (c != '>' && c != '<' && c != '=' && c != '+')
    return 26;
  else
    return -2;
}

int Accept(int state) { return Transition[state].back(); }

void toSymbolTable(string lexeme, vector<pair<string, int>> &Lexeme,
                   int lineCount) {
  auto it =
      find_if(SymbolSet.begin(), SymbolSet.end(),
              [&](const pair<string, int> &p) { return p.first == lexeme; });

  if (it == SymbolSet.end())
    SymbolSet.insert(make_pair(lexeme, symbolTableID++));

  ofstream SymbolTable("SymbolTable.txt", ios::app);

  if (!SymbolTable.is_open()) {
    cout << "Error: SymbolTable.txt not found" << endl;
    return;
  }

  if (it == SymbolSet.end()) {
    SymbolTable << "<" << to_string(symbolTableID - 1) << ", " << lexeme << ">"
                << endl;
    Lexeme.push_back(make_pair(
        "<s" + to_string(symbolTableID - 1) + ", " + lexeme + ">", lineCount));
  } else
    Lexeme.push_back(make_pair(
        "<s" + to_string(it->second) + ", " + it->first + ">", lineCount));

  SymbolTable.close();
  return;
}

void toLiteralTable(string lexeme, vector<pair<string, int>> &Lexeme,
                    int lineCount) {
  auto it =
      find_if(LiteralSet.begin(), LiteralSet.end(),
              [&](const pair<string, int> &p) { return p.first == lexeme; });

  if (it == LiteralSet.end())
    LiteralSet.insert(make_pair(lexeme, literalID++));

  ofstream LiteralTable("LiteralTable.txt", ios::app);

  if (it == LiteralSet.end()) {
    LiteralTable << "<l" << to_string(literalID - 1) << ", " << lexeme << ">"
                 << endl;
    Lexeme.push_back(make_pair(
        "<l" + to_string(literalID - 1) + ", " + lexeme + ">", lineCount));
  } else
    Lexeme.push_back(make_pair(
        "<l" + to_string(it->second) + ", " + it->first + ">", lineCount));

  LiteralTable.close();

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

void toPunctuationTable(string lexeme, vector<pair<string, int>> &Lexeme,
                        int lineCount) {
  auto it =
      find_if(PunctuationSet.begin(), PunctuationSet.end(),
              [&](const pair<string, int> &p) { return p.first == lexeme; });

  if (it == PunctuationSet.end())
    PunctuationSet.insert(make_pair(lexeme, puncTableID++));

  ofstream PunctuationTable("PunctuationTable.txt", ios::app);

  if (!PunctuationTable.is_open()) {
    cout << "Error: PunctuationTable.txt not found" << endl;
    return;
  }

  lexeme = removeTrailingSpaces(lexeme);
  if (it == PunctuationSet.end()) {
    PunctuationTable << '<' << to_string(puncTableID - 1) << ", " << lexeme
                     << ">" << endl;
    Lexeme.push_back(make_pair(
        "<p" + to_string(puncTableID - 1) + ", " + lexeme + ">", lineCount));
  } else
    Lexeme.push_back(make_pair(
        "<p" + to_string(it->second) + ", " + it->first + ">", lineCount));

  PunctuationTable.close();
  return;
}

void toKeywordTable(string lexeme, vector<pair<string, int>> &Lexeme,
                    int lineCount) {
  auto it =
      find_if(KeywordSet.begin(), KeywordSet.end(),
              [&](const pair<string, int> &p) { return p.first == lexeme; });

  if (it == KeywordSet.end())
    KeywordSet.insert(make_pair(lexeme, keywordTableID++));

  ofstream KeywordTable("KeywordTable.txt", ios::app);

  if (!KeywordTable.is_open()) {
    cout << "Error: KeywordTable.txt not found" << endl;
    return;
  }

  if (it == KeywordSet.end()) {
    KeywordTable << "<" << to_string(keywordTableID - 1) << ", " << lexeme
                 << ">" << endl;
    Lexeme.push_back(make_pair(
        "<k" + to_string(keywordTableID - 1) + ", " + lexeme + ">", lineCount));
  } else
    Lexeme.push_back(make_pair(
        "<k" + to_string(it->second) + ", " + it->first + ">", lineCount));

  KeywordTable.close();
  return;
}

void toTokenFile(vector<pair<string, int>> Lexemes) {
  ofstream TokenFile("Tokens.txt");

  if (!TokenFile.is_open()) {
    cout << "Error: Tokens.txt not found" << endl;
    return;
  }

  int lineCount = 1;
  for (auto lexeme : Lexemes)
    if (lexeme.second != lineCount) {

      TokenFile << endl << lexeme.first << "\t";
      lineCount++;
    } else
      TokenFile << lexeme.first << "\t";

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
      if (value == "-")
        row.push_back(-1);
      else
        row.push_back(stoi(value));
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

vector<pair<string, int>> getLexemes() {
  ifstream LexemesFile("Source.txt");

  if (!LexemesFile.is_open()) {
    cout << "Error: Lexemes.txt not found" << endl;
    return {};
  }

  vector<pair<string, int>> Lexemes;

  string line;
  int state = 0, ch, lineCount = 0;

  char *bufferPointer, *forwardPointer;

  while (getline(LexemesFile, line)) {
    lineCount++;
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
          toLiteralTable(string(bufferPointer, forwardPointer), Lexemes,
                         lineCount);
        else
          toKeywordTable(string(bufferPointer, forwardPointer), Lexemes,
                         lineCount);

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
          toPunctuationTable(lexeme, Lexemes, lineCount);

        bufferPointer = forwardPointer;
      } else {
        state = 0;

        string lexeme = string(bufferPointer, forwardPointer);
        forwardPointer++;

        if (!isKeyword(lexeme))
          toSymbolTable(lexeme, Lexemes, lineCount);

        bufferPointer = forwardPointer;
      }
    }
  }

  toTokenFile(Lexemes);

  return Lexemes;
}

int main() {
  getTransitionTable();
  getAdvanceTable(Transition);
  getKeywords();

  vector<pair<string, int>> Tokens = getLexemes();

  if (!Tokens.empty())
    cout << "Program Compiled Successfully!\n";
  else
    cout << "Error Compiling Program\n";
  return 0;
}