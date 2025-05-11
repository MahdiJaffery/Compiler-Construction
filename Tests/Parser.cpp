#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct ParseTreeNode
{
	string value;
	vector<shared_ptr<ParseTreeNode>> children;

	ParseTreeNode(const string &val) : value(val) {}

	void addChild(shared_ptr<ParseTreeNode> child) { children.push_back(child); }
};

int currentIndex = 0;
vector<string> Tokens;
shared_ptr<ParseTreeNode> parseTreeRoot;

bool Parser(shared_ptr<ParseTreeNode> &node);
bool Type(shared_ptr<ParseTreeNode> &node);
bool ArgList(shared_ptr<ParseTreeNode> &node);
bool ArgList_Prime(shared_ptr<ParseTreeNode> &node);
bool Arg(shared_ptr<ParseTreeNode> &node);
bool Declaration(shared_ptr<ParseTreeNode> &node);
bool IdentList(shared_ptr<ParseTreeNode> &node);
bool IdentList_Prime(shared_ptr<ParseTreeNode> &node);
bool Stmt(shared_ptr<ParseTreeNode> &node);
bool ExprStmt(shared_ptr<ParseTreeNode> &node);
bool ForStmt(shared_ptr<ParseTreeNode> &node);
bool OptExpr(shared_ptr<ParseTreeNode> &node);
bool WhileStmt(shared_ptr<ParseTreeNode> &node);
bool IfStmt(shared_ptr<ParseTreeNode> &node);
bool ElsePart(shared_ptr<ParseTreeNode> &node);
bool CompStmt(shared_ptr<ParseTreeNode> &node);
bool StmtList(shared_ptr<ParseTreeNode> &node);
bool Expr(shared_ptr<ParseTreeNode> &node);
bool Expr_Prime(shared_ptr<ParseTreeNode> &node);
bool Rvalue(shared_ptr<ParseTreeNode> &node);
bool Rvalue_Prime(shared_ptr<ParseTreeNode> &node);
bool Compare(shared_ptr<ParseTreeNode> &node);
bool Mag(shared_ptr<ParseTreeNode> &node);
bool Mag_Prime(shared_ptr<ParseTreeNode> &node);
bool Term(shared_ptr<ParseTreeNode> &node);
bool Term_Prime(shared_ptr<ParseTreeNode> &node);
bool Factor(shared_ptr<ParseTreeNode> &node);

void printParseTree(const shared_ptr<ParseTreeNode> &node,
					const string &prefix = "", bool isLast = true)
{
	if (!node)
		return;

	cout << prefix;
	cout << (isLast ? "└── " : "├── ");
	cout << node->value << endl;

	for (size_t i = 0; i < node->children.size(); ++i)
	{
		bool lastChild = (i == node->children.size() - 1);
		string newPrefix = prefix + (isLast ? "    " : "│   ");
		printParseTree(node->children[i], newPrefix, lastChild);
	}
}

// bool findInLiteralTable(string token) {
//   ifstream file("LiteralTable.txt");
//   if (!file) {
//     cout << "Error Opening Literal Table";
//     exit(1);
//   }

//   string Line;
//   while (getline(file, Line)) {
//     stringstream ss(Line);
//     string checkToken;
//     while (ss >> checkToken)
//       if (checkToken.substr(checkToken.length() - 1) == token)
//         return true;
//   }
//   return false;
// }

bool findInSymbolTable(string token)
{
	ifstream file("SymbolTable.txt");
	if (!file)
	{
		cout << "Error opening SymbolTable";
		exit(1);
	}

	string Line;
	while (getline(file, Line))
	{
		stringstream ss(Line);
		string checkToken;
		while (ss >> checkToken)
			if (checkToken.substr(0, checkToken.length() - 1) == token)
				return true;
	}
	return false;
}

bool findInLiteralTable(string token)
{
	ifstream file("LiteralTable.txt");
	if (!file)
	{
		cout << "Error opening LiteralTable";
		exit(1);
	}

	string Line;
	while (getline(file, Line))
	{
		stringstream ss(Line);
		string checkToken;

		while (ss >> checkToken)
			if (checkToken.substr(0, checkToken.length() - 1) == token ||
				checkToken.substr(0, checkToken.length() - 1) == "\"" + token + "\"")
				return true;
	}
	return false;
}

void setTokenVector(vector<string> &Tokens, string filename)
{
	ifstream file(filename);
	string line;

	while (getline(file, line))
	{
		stringstream ss(line);
		string token;
		string quotedToken;
		bool inQuotes = false;
		bool removed = false;

		while (ss >> token)
		{
			if (token[0] == '<' && token[token.length() - 1] == ',')
				continue;

			if (token.find('\"') != string::npos)
			{
				if (!inQuotes)
				{
					inQuotes = true;
					quotedToken = token.substr(token.find('\"') + 1);

					if (token.rfind('\"') != token.find('\"'))
					{
						quotedToken = quotedToken.substr(0, quotedToken.rfind('\"'));
						Tokens.push_back(quotedToken);
						inQuotes = false;
					}
				}
				else
				{
					quotedToken += " " + token.substr(0, token.rfind('\"'));
					Tokens.push_back(quotedToken);
					inQuotes = false;
				}
			}
			else if (inQuotes)
				quotedToken += " " + token;
			else
			{
				// size_t endPos = token.find('>');
				if (token.back() == '>')
					token = token.substr(0, token.length() - 1);

				// if (!token.empty() && token.back() == ',')
				// 	token = token.substr(0, token.size() - 1);

				if (!token.empty())
					Tokens.push_back(token);
			}
		}
	}
}

string getToken()
{
	return currentIndex <= Tokens.size() - 1 ? Tokens[currentIndex] : "";
}

string peek()
{
	return currentIndex <= Tokens.size() - 1 ? Tokens[currentIndex + 1] : "";
}

void Advance()
{
	if (currentIndex <= Tokens.size() - 1)
		currentIndex++;
	else
	{
		cout << "No more Tokens in file\n\n";
		exit(1);
	}
}

bool Parser(shared_ptr<ParseTreeNode> &node)
{
	node = make_shared<ParseTreeNode>("Parser");

	// Handle function declaration or global declaration
	auto typeNode = make_shared<ParseTreeNode>("Type");
	if (!Type(typeNode))
		return false;
	node->addChild(typeNode);

	string currentToken = getToken();
	if (!findInSymbolTable(currentToken))
		return false;
	node->addChild(make_shared<ParseTreeNode>("Symbol: " + currentToken));
	Advance();

	// Check if this is a function declaration
	if (getToken() == "(")
	{
		node->addChild(make_shared<ParseTreeNode>("("));
		Advance();

		auto argListNode = make_shared<ParseTreeNode>("ArgList");
		if (!ArgList(argListNode))
			return false;
		node->addChild(argListNode);

		if (getToken() != ")")
			return false;
		node->addChild(make_shared<ParseTreeNode>(")"));
		Advance();
	}

	auto compStmtNode = make_shared<ParseTreeNode>("CompStmt");
	if (!CompStmt(compStmtNode))
		return false;
	node->addChild(compStmtNode);

	return true;
}

bool Type(shared_ptr<ParseTreeNode> &node)
{
	string currentToken = getToken();

	if (currentToken != "Adadi" && currentToken != "Ashriya" &&
		currentToken != "Harf" && currentToken != "Matn" &&
		currentToken != "Mantiqi")
		return false;

	node->addChild(make_shared<ParseTreeNode>(currentToken));
	Advance();
	return true;
}

bool ArgList(shared_ptr<ParseTreeNode> &node)
{
	auto argNode = make_shared<ParseTreeNode>("Arg");
	if (!Arg(argNode))
		return false;
	node->addChild(argNode);

	auto argListPrimeNode = make_shared<ParseTreeNode>("ArgList_Prime");
	if (!ArgList_Prime(argListPrimeNode))
		return false;
	node->addChild(argListPrimeNode);

	return true;
}

bool ArgList_Prime(shared_ptr<ParseTreeNode> &node)
{
	if (getToken() == ",")
	{
		node->addChild(make_shared<ParseTreeNode>(","));
		Advance();

		auto argNode = make_shared<ParseTreeNode>("Arg");
		if (!Arg(argNode))
			return false;
		node->addChild(argNode);

		auto argListPrimeNode = make_shared<ParseTreeNode>("ArgList_Prime");
		if (!ArgList_Prime(argListPrimeNode))
			return false;
		node->addChild(argListPrimeNode);
	}
	return true;
}

bool Arg(shared_ptr<ParseTreeNode> &node)
{
	auto typeNode = make_shared<ParseTreeNode>("Type");
	if (!Type(typeNode))
		return false;
	node->addChild(typeNode);

	string currentToken = getToken();
	if (!findInSymbolTable(currentToken))
		return false;
	node->addChild(make_shared<ParseTreeNode>("Symbol: " + currentToken));
	Advance();

	return true;
}

bool Declaration(shared_ptr<ParseTreeNode> &node)
{
	auto typeNode = make_shared<ParseTreeNode>("Type");
	if (!Type(typeNode))
		return false;
	node->addChild(typeNode);

	auto identListNode = make_shared<ParseTreeNode>("IdentList");
	if (!IdentList(identListNode))
		return false;
	node->addChild(identListNode);

	if (getToken() != "::")
		return false;
	node->addChild(make_shared<ParseTreeNode>("::"));
	Advance();

	return true;
}

bool IdentList(shared_ptr<ParseTreeNode> &node)
{
	string currentToken = getToken();
	if (!findInSymbolTable(currentToken))
		return false;
	node->addChild(make_shared<ParseTreeNode>("Symbol: " + currentToken));
	Advance();

	auto identListPrimeNode = make_shared<ParseTreeNode>("IdentList_Prime");
	if (!IdentList_Prime(identListPrimeNode))
		return false;
	node->addChild(identListPrimeNode);

	return true;
}

bool IdentList_Prime(shared_ptr<ParseTreeNode> &node)
{
	if (getToken() == ",")
	{
		node->addChild(make_shared<ParseTreeNode>(","));
		Advance();

		string currentToken = getToken();
		if (!findInSymbolTable(currentToken))
			return false;
		node->addChild(make_shared<ParseTreeNode>("Symbol: " + currentToken));
		Advance();

		auto identListPrimeNode = make_shared<ParseTreeNode>("IdentList_Prime");
		if (!IdentList_Prime(identListPrimeNode))
			return false;
		node->addChild(identListPrimeNode);
	}
	return true;
}

bool Stmt(shared_ptr<ParseTreeNode> &node)
{
	auto forStmtNode = make_shared<ParseTreeNode>("ForStmt");
	if (ForStmt(forStmtNode))
	{
		node->addChild(forStmtNode);
		return true;
	}

	auto whileStmtNode = make_shared<ParseTreeNode>("WhileStmt");
	if (WhileStmt(whileStmtNode))
	{
		node->addChild(whileStmtNode);
		return true;
	}

	auto exprStmtNode = make_shared<ParseTreeNode>("ExprStmt");
	if (ExprStmt(exprStmtNode))
	{
		node->addChild(exprStmtNode);
		return true;
	}

	auto ifStmtNode = make_shared<ParseTreeNode>("IfStmt");
	if (IfStmt(ifStmtNode))
	{
		node->addChild(ifStmtNode);
		return true;
	}

	auto compStmtNode = make_shared<ParseTreeNode>("CompStmt");
	if (CompStmt(compStmtNode))
	{
		node->addChild(compStmtNode);
		return true;
	}

	auto declNode = make_shared<ParseTreeNode>("Declaration");
	if (Declaration(declNode))
	{
		node->addChild(declNode);
		return true;
	}

	if (getToken() == "::")
	{
		node->addChild(make_shared<ParseTreeNode>("::"));
		Advance();
		return true;
	}

	return false;
}

bool ExprStmt(shared_ptr<ParseTreeNode> &node)
{
	auto exprNode = make_shared<ParseTreeNode>("Expr");
	if (!Expr(exprNode))
		return false;
	node->addChild(exprNode);

	if (getToken() != "::")
		return false;
	node->addChild(make_shared<ParseTreeNode>("::"));
	Advance();

	return true;
}

bool ForStmt(shared_ptr<ParseTreeNode> &node)
{
	if (getToken() != "for")
		return false;
	node->addChild(make_shared<ParseTreeNode>("for"));
	Advance();

	if (getToken() != "(")
		return false;
	node->addChild(make_shared<ParseTreeNode>("("));
	Advance();

	auto exprNode = make_shared<ParseTreeNode>("Expr");
	if (!Expr(exprNode))
		return false;
	node->addChild(exprNode);

	if (getToken() != "::")
		return false;
	node->addChild(make_shared<ParseTreeNode>("::"));
	Advance();

	auto optExprNode = make_shared<ParseTreeNode>("OptExpr");
	if (!OptExpr(optExprNode))
		return false;
	node->addChild(optExprNode);

	if (getToken() != "::")
		return false;
	node->addChild(make_shared<ParseTreeNode>("::"));
	Advance();

	auto optExprNode2 = make_shared<ParseTreeNode>("OptExpr");
	if (!OptExpr(optExprNode2))
		return false;
	node->addChild(optExprNode2);

	if (getToken() != ")")
		return false;
	node->addChild(make_shared<ParseTreeNode>(")"));
	Advance();

	auto stmtNode = make_shared<ParseTreeNode>("Stmt");
	if (!Stmt(stmtNode))
		return false;
	node->addChild(stmtNode);

	return true;
}

bool OptExpr(shared_ptr<ParseTreeNode> &node)
{
	auto exprNode = make_shared<ParseTreeNode>("Expr");
	if (!Expr(exprNode))
	{
		node->addChild(make_shared<ParseTreeNode>("ε")); // Null productoin
		return true;
	}
	node->addChild(exprNode);
	return true;
}

bool WhileStmt(shared_ptr<ParseTreeNode> &node)
{
	if (getToken() != "while")
		return false;
	node->addChild(make_shared<ParseTreeNode>("while"));
	Advance();

	if (getToken() != "(")
		return false;
	node->addChild(make_shared<ParseTreeNode>("("));
	Advance();

	auto exprNode = make_shared<ParseTreeNode>("Expr");
	if (!Expr(exprNode))
		return false;
	node->addChild(exprNode);

	if (getToken() != ")")
		return false;
	node->addChild(make_shared<ParseTreeNode>(")"));
	Advance();

	auto stmtNode = make_shared<ParseTreeNode>("Stmt");
	if (!Stmt(stmtNode))
		return false;
	node->addChild(stmtNode);

	return true;
}

bool IfStmt(shared_ptr<ParseTreeNode> &node)
{
	if (getToken() != "Agar")
		return false;
	node->addChild(make_shared<ParseTreeNode>("Agar"));
	Advance();

	if (getToken() != "(")
		return false;
	node->addChild(make_shared<ParseTreeNode>("("));
	Advance();

	auto exprNode = make_shared<ParseTreeNode>("Expr");
	if (!Expr(exprNode))
		return false;
	node->addChild(exprNode);

	if (getToken() != ")")
		return false;
	node->addChild(make_shared<ParseTreeNode>(")"));
	Advance();

	auto stmtNode = make_shared<ParseTreeNode>("Stmt");
	if (!Stmt(stmtNode))
		return false;
	node->addChild(stmtNode);

	auto elsePartNode = make_shared<ParseTreeNode>("ElsePart");
	if (!ElsePart(elsePartNode))
		return false;
	node->addChild(elsePartNode);

	return true;
}

bool ElsePart(shared_ptr<ParseTreeNode> &node)
{
	if (getToken() == "Wagarna")
	{
		node->addChild(make_shared<ParseTreeNode>("Wagarna"));
		Advance();

		auto stmtNode = make_shared<ParseTreeNode>("Stmt");
		if (!Stmt(stmtNode))
			return false;
		node->addChild(stmtNode);
	}
	else
		node->addChild(make_shared<ParseTreeNode>("ε"));

	return true;
}

bool CompStmt(shared_ptr<ParseTreeNode> &node)
{
	if (getToken() != "{")
		return false;
	node->addChild(make_shared<ParseTreeNode>("{"));
	Advance();

	auto stmtListNode = make_shared<ParseTreeNode>("StmtList");
	if (!StmtList(stmtListNode))
		return false;
	node->addChild(stmtListNode);

	if (getToken() != "}")
		return false;
	node->addChild(make_shared<ParseTreeNode>("}"));
	Advance();

	return true;
}

bool StmtList(shared_ptr<ParseTreeNode> &node)
{
	auto stmtNode = make_shared<ParseTreeNode>("Stmt");
	if (Stmt(stmtNode))
	{
		node->addChild(stmtNode);

		auto stmtListNode = make_shared<ParseTreeNode>("StmtList");
		if (!StmtList(stmtListNode))
			return false;
		node->addChild(stmtListNode);
	}
	else
		node->addChild(make_shared<ParseTreeNode>("ε"));

	return true;
}

bool Expr(shared_ptr<ParseTreeNode> &node)
{
	string currentToken = getToken();

	// Handle assignment case
	if (findInSymbolTable(currentToken) && peek() == ":=")
	{
		node->addChild(make_shared<ParseTreeNode>("Symbol: " + currentToken));
		Advance();

		node->addChild(make_shared<ParseTreeNode>(":="));
		Advance();

		auto exprNode = make_shared<ParseTreeNode>("Expr");
		if (!Expr(exprNode))
			return false;
		node->addChild(exprNode);
		return true;
	}

	// Handle normal expressions
	return Rvalue(node);
}

bool Expr_Prime(shared_ptr<ParseTreeNode> &node)
{
	if (getToken() == ":=")
	{
		node->addChild(make_shared<ParseTreeNode>(":="));
		Advance();

		auto exprNode = make_shared<ParseTreeNode>("Expr: " + getToken());
		if (!Expr(exprNode))
			return false;
		node->addChild(exprNode);
	}
	else
	{
		auto rvaluePrimeNode = make_shared<ParseTreeNode>("Rvalue_Prime");
		if (!Rvalue_Prime(rvaluePrimeNode))
			return false;
		node->addChild(rvaluePrimeNode);
	}
	return true;
}

bool Rvalue(shared_ptr<ParseTreeNode> &node)
{
	auto magNode = make_shared<ParseTreeNode>("Mag");
	if (!Mag(magNode))
		return false;
	node->addChild(magNode);

	auto rvaluePrimeNode = make_shared<ParseTreeNode>("Rvalue_Prime");
	if (!Rvalue_Prime(rvaluePrimeNode))
		return false;
	node->addChild(rvaluePrimeNode);

	return true;
}

bool Rvalue_Prime(shared_ptr<ParseTreeNode> &node)
{
	auto compareNode = make_shared<ParseTreeNode>("Compare");
	if (Compare(compareNode))
	{
		node->addChild(compareNode);

		auto magNode = make_shared<ParseTreeNode>("Mag");
		if (!Mag(magNode))
			return false;
		node->addChild(magNode);

		auto rvaluePrimeNode = make_shared<ParseTreeNode>("Rvalue_Prime");
		if (!Rvalue_Prime(rvaluePrimeNode))
			return false;
		node->addChild(rvaluePrimeNode);
	}
	else
		node->addChild(make_shared<ParseTreeNode>("ε"));

	return true;
}

bool Compare(shared_ptr<ParseTreeNode> &node)
{
	string currentToken = getToken();
	if (currentToken != "==" && currentToken != "<" && currentToken != ">" &&
		currentToken != "<=" && currentToken != ">=" && currentToken != "!=" &&
		currentToken != "<>")
		return false;

	node->addChild(make_shared<ParseTreeNode>(currentToken));
	Advance();
	return true;
}

bool Mag(shared_ptr<ParseTreeNode> &node)
{
	auto termNode = make_shared<ParseTreeNode>("Term");
	if (!Term(termNode))
		return false;
	node->addChild(termNode);

	auto magPrimeNode = make_shared<ParseTreeNode>("Mag_Prime");
	if (!Mag_Prime(magPrimeNode))
		return false;
	node->addChild(magPrimeNode);

	return true;
}

bool Mag_Prime(shared_ptr<ParseTreeNode> &node)
{
	string currentToken = getToken();
	if (currentToken == "+" || currentToken == "-")
	{
		node->addChild(make_shared<ParseTreeNode>(currentToken));
		Advance();

		auto termNode = make_shared<ParseTreeNode>("Term");
		if (!Term(termNode))
			return false;
		node->addChild(termNode);

		auto magPrimeNode = make_shared<ParseTreeNode>("Mag_Prime");
		if (!Mag_Prime(magPrimeNode))
			return false;
		node->addChild(magPrimeNode);
	}
	else
		node->addChild(make_shared<ParseTreeNode>("ε"));

	return true;
}

bool Term(shared_ptr<ParseTreeNode> &node)
{
	auto factorNode = make_shared<ParseTreeNode>("Factor");
	if (!Factor(factorNode))
		return false;
	node->addChild(factorNode);

	auto termPrimeNode = make_shared<ParseTreeNode>("Term_Prime");
	if (!Term_Prime(termPrimeNode))
		return false;
	node->addChild(termPrimeNode);

	return true;
}

bool Term_Prime(shared_ptr<ParseTreeNode> &node)
{
	string currentToken = getToken();
	if (currentToken == "*" || currentToken == "/")
	{
		node->addChild(make_shared<ParseTreeNode>(currentToken));
		Advance();

		auto factorNode = make_shared<ParseTreeNode>("Factor");
		if (!Factor(factorNode))
			return false;
		node->addChild(factorNode);

		auto termPrimeNode = make_shared<ParseTreeNode>("Term_Prime");
		if (!Term_Prime(termPrimeNode))
			return false;
		node->addChild(termPrimeNode);
	}
	else
		node->addChild(make_shared<ParseTreeNode>("ε"));

	return true;
}

bool Factor(shared_ptr<ParseTreeNode> &node)
{
	if (getToken() == "(")
	{
		node->addChild(make_shared<ParseTreeNode>("("));
		Advance();

		auto exprNode = make_shared<ParseTreeNode>("Expr");
		if (!Expr(exprNode))
			return false;
		node->addChild(exprNode);

		if (getToken() != ")")
			return false;
		node->addChild(make_shared<ParseTreeNode>(")"));
		Advance();

		return true;
	}
	else if (findInSymbolTable(getToken()))
	{
		node->addChild(make_shared<ParseTreeNode>("Symbol: " + getToken()));
		Advance();
		return true;
	}
	else if (findInLiteralTable(getToken()))
	{
		node->addChild(make_shared<ParseTreeNode>("Literal: " + getToken()));
		Advance();
		return true;
	}
	return false;
}

int main()
{
	setTokenVector(Tokens, "Tokens.txt");

	if (Parser(parseTreeRoot))
	{
		cout << "Parsing Successful!\n";
		cout << "\nParse Tree:\n";
		printParseTree(parseTreeRoot);
	}
	else
		cout << "Parsing Failed!\n";

	return 0;
}