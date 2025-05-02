#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

int currentIndex = 0;
vector<string> Tokens;

bool findInSymbolTable(string token) {
  ifstream file("SymbolTable.txt");
  if (!file) {
    cout << "Error opening SymbolTable";
    exit(1);
  }

  string Line;
  while (getline(file, Line)) {
    stringstream ss(Line);
    string checkToken;
    while (ss >> checkToken)
      if (checkToken.substr(0, checkToken.length() - 1) == token)
        return true;
  }
  return false;
}

bool findInLiteralTable(string token) {
  ifstream file("LiteralTable.txt");
  if (!file) {
    cout << "Error opening LiteralTable";
    exit(1);
  }

  string Line;
  while (getline(file, Line)) {
    stringstream ss(Line);
    string checkToken;

    while (ss >> checkToken)
      if (checkToken.substr(0, checkToken.length() - 1) == token ||
          checkToken.substr(0, checkToken.length() - 1) == "\"" + token + "\"")
        return true;
  }
  return false;
}

void setTokenVector(vector<string> &Tokens, string filename) {
  ifstream file(filename);
  string line;

  while (getline(file, line)) {
    stringstream ss(line);
    string token;
    string quotedToken;
    bool inQuotes = false;

    while (ss >> token) {
      if (token[0] == '<')
        continue;

      if (token.find('\"') != string::npos) {
        if (!inQuotes) {
          inQuotes = true;
          quotedToken = token.substr(token.find('\"') + 1);

          if (token.rfind('\"') != token.find('\"')) {
            quotedToken = quotedToken.substr(0, quotedToken.rfind('\"'));
            Tokens.push_back(quotedToken);
            inQuotes = false;
          }
        } else {
          quotedToken += " " + token.substr(0, token.rfind('\"'));
          Tokens.push_back(quotedToken);
          inQuotes = false;
        }
      } else if (inQuotes)
        quotedToken += " " + token;
      else {
        size_t endPos = token.find('>');
        if (endPos != string::npos)
          token = token.substr(0, endPos);

        if (!token.empty())
          Tokens.push_back(token);
      }
    }
  }
}

string getToken() {
  return currentIndex <= Tokens.size() - 1 ? Tokens[currentIndex] : "";
}

void Advance() {
  if (currentIndex <= Tokens.size() - 1)
    currentIndex++;
  else {
    cout << "No more Tokens in file\n\n";
    exit(1);
  }
}

bool Stmt();
bool Declaration();
bool Type();
bool IdentList();
bool IdentList_Prime();

bool WhileStmt();
bool IfStmt();
bool ElsePart();
bool CompStmt();
bool StmtList();

bool Expr();
bool Rvalue();
bool Rvalue_Prime();
bool Compare();
bool Mag();
bool Mag_Prime();
bool Term();
bool Term_Prime();
bool Factor();

bool Stmt() {
  if (WhileStmt())
    return true;

  if (Expr()) {
    string currentToken = getToken();

    if (currentToken != "::")
      return false;

    Advance();
    return true;
  }

  if (IfStmt())
    return true;

  if (CompStmt())
    return true;

  if (Declaration())
    return true;

  string currentToken = getToken();

  if (currentToken == "::") {
    Advance();
    return true;
  }

  return false;
}

bool Declaration() {
  if (!Type())
    return false;

  if (!IdentList)
    return false;

  if (getToken() != "::")
    return false;

  Advance();
  return true;
}

bool Type() {
  string currentToken = getToken();

  if (currentToken == "Adadi" || currentToken == "Ashriya" ||
      currentToken == "Harf" || currentToken == "Matn" ||
      currentToken == "Mantiqi") {
    Advance();
    return true;
  }
  return false;
}

bool IdentList() {
  if (!findInSymbolTable(getToken()))
    return false;
  Advance();

  if (!IdentList_Prime())
    return false;

  return true;
}

bool IdentList_Prime() {
  if (getToken() == ",") {
    Advance();

    if (!IdentList())
      return false;
  }
  return true;
}

bool WhileStmt() {
  if (getToken() != "while")
    return false;

  Advance();
  if (getToken() != "(")
    return false;

  Advance();
  if (!Expr())
    return false;

  if (getToken() != ")")
    return false;

  Advance();
  if (!Stmt())
    return false;

  return true;
}

bool IfStmt() {
  if (getToken() != "Agar")
    return false;

  Advance();
  if (getToken() != "(")
    return false;

  Advance();
  if (!Expr())
    return false;

  if (getToken() != ")")
    return false;

  Advance();
  if (!Stmt())
    return false;

  if (!ElsePart())
    return false;

  return true;
}

bool ElsePart() {
  if (getToken() == "Wagarna") {
    Advance();

    if (!Stmt())
      return false;
  }
  return true;
}

bool CompStmt() {
  if (getToken() != "{")
    return false;

  Advance();
  if (!StmtList())
    return false;

  if (getToken() != "}")
    return false;

  Advance();
  return true;
}

bool StmtList() {
  if (Stmt()) {
    if (!StmtList())
      return false;
  }
  return true;
}

bool Expr() {
  if (findInSymbolTable(getToken())) {
    Advance();
    if (getToken() != ":=")
      return false;

    Advance();
    if (!Expr())
      return false;
    return true;
  } else if (Rvalue())
    return true;

  return false;
}

bool Rvalue() {
  if (!Mag())
    return false;

  if (!Rvalue_Prime())
    return false;
  return true;
}

bool Rvalue_Prime() {
  if (Compare()) {
    if (!Mag())
      return false;

    if (!Rvalue_Prime())
      return false;
  }
  return true;
}

bool Compare() {
  string curTok = getToken();

  if (curTok == "==" || curTok == "<" || curTok == ">" || curTok == "<=" ||
      curTok == ">=" || curTok == "!=" || curTok == "<>") {
    Advance();
    return true;
  }
  return false;
}

bool Mag() {
  if (!Term())
    return false;

  if (!Mag_Prime())
    return false;
  return true;
}

bool Mag_Prime() {
  string currentToken = getToken();
  if (currentToken == "+" || currentToken == "-") {
    Advance();

    if (!Term())
      return false;

    if (!Mag_Prime())
      return false;
  }
  return true;
}

bool Term() {
  if (!Factor())
    return false;

  if (!Term_Prime())
    return false;
  return true;
}

bool Term_Prime() {
  string currentToken = getToken();
  if (currentToken == "*" || currentToken == "/") {
    Advance();

    if (!Factor())
      return false;

    if (!Term_Prime())
      return false;
  }
  return true;
}

bool Factor() {
  if (getToken() == "(") {
    Advance();

    if (!Expr())
      return false;

    if (getToken() != ")")
      return false;

    Advance();
    return true;
  } else if (findInSymbolTable(getToken())) {
    Advance();
    return true;
  } else if (findInLiteralTable(getToken())) {
    Advance();
    return true;
  }
  return false;
}

int main() {
  setTokenVector(Tokens, "Tokens.txt");

  for (auto str : Tokens)
    cout << str << endl;

  return 0;
}