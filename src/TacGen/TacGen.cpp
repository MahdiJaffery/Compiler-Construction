#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

struct Node
{
};

ofstream TacFile("Tac.txt", ios::app);

vector<string> Tokens;
int currentIndex = 0;

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

		while (ss >> token)
		{
			if (token[0] == '<')
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

bool ThreAddressCode()
{
	return Stmt();
}

bool Stmt()
{
	if (WhileStmt())
		return true;

	if (Expr())
		if (getToken() != "::")
			return false;
		else
		{
			Advance();
			return true;
		}

	if (IfStmt())
		return true;

	if (CompStmt())
		return true;

	if (Declaration())
		return true;

	if (getToken() != "::")
		return false;

	Advance();
	return true;
}

bool Declaration()
{
	if (!Type())
		return false;

	if (!IdentList())
		return false;

	if (getToken() != "::")
		return false;

	Advance();
	return true;
}

bool Type()
{
	string tok = getToken();

	if (tok == "Adadi" || tok == "Ashriya" || tok == "Mantiqi" || tok == "Matn" || tok == "Harf")
	{
		Advance();
		return true;
	}

	return false;
}

bool IdentList()
{
	string currentToken = getToken();

	if (!findInSymbolTable(currentToken))
		return false;

	Advance();
	if (!IdentList_Prime())
		return false;

	return true;
}

bool IdentList_Prime()
{
	if (getToken() == ",")
	{
		Advance();

		if (!IdentList())
			return false;
	}
	return true;
}

bool WhileStmt()
{
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

bool IfStmt()
{
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

bool ElsePart()
{
	if (getToken() == "Wagarna")
	{
		Advance();

		if (!Stmt())
			return false;
	}
	return true;
}

bool CompStmt()
{
	if (getToken() != "{")
		return false;

	Advance();
	if (!StmtList())
		return false;

	if (getToken() != "}")
		return false;

	return true;
}

bool StmtList()
{
	if (Stmt())
	{
		if (!StmtList())
			return false;
	}
	return true;
}

bool Expr()
{
	if (findInSymbolTable(getToken()))
	{

		Advance();
		if (getToken() != ":=")
			return false;

		Advance();
		if (!Expr())
			return false;
		return true;
	}
	else
		return Rvalue();
}

bool Rvalue()
{
	if (!Mag())
		return false;

	if (!Rvalue_Prime())
		return false;

	return true;
}

bool Rvalue_Prime()
{
	if (Compare())
	{
		if (!Mag())
			return false;

		if (!Rvalue_Prime())
			return false;
	}
	return true;
}

bool Compare()
{
	string tok = getToken();

	if (tok == "==" || tok == "<" || tok == ">" || tok == "<=" || tok == ">=" || tok == "!=" || tok == "<>")
	{
		Advance();
		return true;
	}
	return false;
}

bool Mag()
{
	if (!Term())
		return false;

	if (!Mag_Prime())
		return false;

	return true;
}

bool Mag_Prime()
{
	string tok = getToken();

	if (tok == "+" || tok == "-")
	{
		Advance();

		if (!Term())
			return false;

		if (!Mag_Prime())
			return false;
	}
	return true;
}

bool Term()
{
	if (!Factor())
		return false;

	if (!Term_Prime())
		return false;

	return true;
}

bool Term_Prime()
{
	string tok = getToken();

	if (tok == "*" || tok == "/")
	{
		Advance();

		if (!Factor())
			return false;

		if (!Term_Prime())
			return false;
	}
	return true;
}

bool Factor()
{
	if (getToken() == "(")
	{
		Advance();

		if (!Expr())
			return false;

		if (getToken() != ")")
			return false;

		Advance();
		return true;
	}
	else if (findInSymbolTable(getToken()))
	{
		Advance();
		return true;
	}
	else if (findInLiteralTable(getToken()))
	{
		Advance();
		return true;
	}

	return false;
}

int main()
{
	if (!TacFile)
	{
		printf("Failed to Open TacGen.txt\n");
		return 1;
	}

	setTokenVector(Tokens, "Tokens.txt");

	if (ThreAddressCode())
		cout << "Successful\n";
	else
		cout << "Failed\n";

	return 0;
}