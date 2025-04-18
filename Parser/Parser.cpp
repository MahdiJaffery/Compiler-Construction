#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// #define line "\n\n"
// #define space "\t"

using namespace std;

int currentIndex = 0;
vector<string> Tokens;

bool findInLiteralTable(string token) {
  ifstream file("LiteralTable.txt");

  if (!file) {
    cout << "Error Opening Literal Table";
    exit(1);
  }

  string Line;

  while (getline(file, Line)) {
    stringstream ss(Line);
    string checkToken;

    while (ss >> checkToken)
      if (checkToken.substr(checkToken.length() - 1) == token)
        return true;
  }
  return false;
}

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

        if (!token.empty() && token.back() == ',')
          token = token.substr(0, token.size() - 1);

        if (!token.empty())
          Tokens.push_back(token);
      }
    }
  }
  return;
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

  return;
}

bool Function();
bool ArgList();
bool ArgList_Prime();
bool Arg();
bool Declaration();
bool Type();
bool IdentList();
bool IdentList_Prime();
bool Stmt();
bool ExprStmt();
bool ForStmt();
bool OptExpr();
bool WhileStmt();
bool IfStmt();
bool ElsePart();
bool CompStmt();
bool StmtList();
bool Expr();
bool Expr_Prime();
bool Rvalue();
bool Rvalue_Prime();
bool Compare();
bool Mag();
bool Mag_Prime();
bool Term();
bool Term_Prime();
bool Factor();

bool Parser() {
  if (!Type())
    return false;

  if (!findInSymbolTable(getToken()))
    return false;
  Advance();

  if (getToken() != "(")
    return false;
  Advance();

  if (!ArgList())
    return false;

  if (getToken() != ")")
    return false;
  Advance();

  if (!CompStmt())
    return false;

  return true;
}

bool Type() {
  string currentToken = getToken();

  if (currentToken != "Adadi" && currentToken != "Ashroya" &&
      currentToken != "Harf" && currentToken != "Matn" &&
      currentToken != "Mantiqi")
    return false;
  Advance();
  return true;
}

bool ArgList() {
  if (!Arg())
    return false;
  return ArgList_Prime();
}

bool ArgList_Prime() {
  if (getToken() == ",") {
    Advance();
    if (!Arg())
      return false;
    return ArgList_Prime();
  }
  return true; //  Null Productoin
}

bool Arg() {
  if (!Type())
    return false;
  if (!findInSymbolTable(getToken()))
    return false;
  Advance();
  return true;
}

bool Declaration() {
  if (!Type())
    return false;
  if (!IdentList())
    return false;
  if (getToken() != "::")
    return false;
  Advance();
  return true;
}

bool IdentList() {
  if (!findInSymbolTable(getToken()))
    return false;
  Advance();
  return IdentList_Prime();
}

bool IdentList_Prime() {
  if (getToken() == ",") {
    Advance();
    if (!findInSymbolTable(getToken()))
      return false;
    Advance();
    return IdentList_Prime();
  }
  return true;
}

bool Stmt() {
  if (ForStmt())
    return true;
  if (WhileStmt())
    return true;
  if (ExprStmt())
    return true;
  if (IfStmt())
    return true;
  if (CompStmt())
    return true;
  if (Declaration())
    return true;

  if (getToken() == "::") {
    Advance();
    return true;
  }

  return false;
}

bool ExprStmt() {
  if (!Expr())
    return false;
  if (getToken() != "::")
    return false;
  Advance();
  return true;
}

bool ForStmt() {
  if (getToken() != "for")
    return false;
  Advance();
  if (getToken() != "(")
    return false;
  Advance();
  if (!Expr())
    return false;
  if (getToken() != "::")
    return false;
  Advance();
  if (!OptExpr())
    return false;
  if (getToken() != "::")
    return false;
  return true;
  if (!OptExpr())
    return false;
  if (getToken() != ")")
    return false;
  return Stmt();
}

bool OptExpr() {
  if (!Expr())
    return false;
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
  return Stmt();
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
  return ElsePart();
}

bool ElsePart() {
  if (getToken() == "Wagarna") {
    Advance();
    return Stmt();
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
  return true;
}

bool StmtList() {
  if (Stmt()) {
    return StmtList();
  }
  return true;
}

bool Expr() {
  if (!findInSymbolTable(getToken()))
    return false;
  Advance();
  return Expr_Prime();
}

bool Expr_Prime() {
  if (getToken() == ":=") {
    Advance();
    return Expr();
  } else
    return Rvalue_Prime();
}

bool Rvalue() {
  if (!Mag())
    return false;
  return Rvalue_Prime();
}

bool Rvalue_Prime() {
  if (Compare()) {
    if (!Mag())
      return false;
    return Rvalue_Prime();
  }
  return true;
}

bool Compare() {
  string currentToken = getToken();
  if (currentToken != "==" && currentToken != "<" && currentToken != ">" &&
      currentToken != "<=" && currentToken != ">=" && currentToken == "!=" &&
      currentToken != "<>")
    return false;
  Advance();
  return true;
}

bool Mag() {
  if (!Term())
    return false;
  return Mag_Prime();
}

bool Mag_Prime() {
  string currentToken = getToken();
  if (currentToken == "+" || currentToken == "-") {
    Advance();
    if (!Term())
      return false;
    return Mag_Prime();
  }
  return true;
}

bool Term() {
  if (!Factor())
    return false;
  return Term_Prime();
}

bool Term_Prime() {
  string currentToken = getToken();
  if (currentToken == "*" || currentToken == "/") {
    if (!Factor())
      return false;
    return Term_Prime();
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

  for (auto token : Tokens) {
    cout << token << endl;
  }

  if (Parser())
    printf("Parsing Successful!\n");
  else
    printf("Parsing Failed!\n");
  return 0;
}