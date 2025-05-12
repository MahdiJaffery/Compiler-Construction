#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Rule
{
	// Compare
	string relop;

	// General Values
	string val;

	// If-Else
	string trueLabel, falseLabel, label;

	// Types
	string type;

	// While Loop
	string start, looplabel, end;

	// For Loop
	string init, cond, incr;

	Rule() : relop(""), val(""), trueLabel(""), falseLabel(""), label(""),
			 type(""), start(""), looplabel(""), end(""), init(""), cond(""), incr("") {}
};

ofstream TacFile("Tac.txt", ios::app);
int tempIndex = 1, labelValue = 1;
vector<string> Tokens;
int currentIndex = 0;

void gen(string tac)
{
	TacFile << tac << endl;
	return;
}

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

bool ThreeAddressCode();
bool Type(Rule &myRule);
bool ArgList(Rule &myRule);
bool ArgList_Prime(Rule &myRule);
bool Arg(Rule &myRule);
bool Declaration(Rule &myRule);
bool IdentList(Rule &myRule);
bool IdentList_Prime(Rule &myRule);
bool Stmt(Rule &myRule);
bool ExprStmt(Rule &myRule);
bool ForStmt(Rule &myRule);
bool OptExpr(Rule &myRule);
bool WhileStmt(Rule &myRule);
bool IfStmt(Rule &myRule);
bool ElsePart(Rule &myRule);
bool CompStmt(Rule &myRule);
bool StmtList(Rule &myRule);
bool Expr(Rule &myRule);
bool Expr_Prime(Rule &myRule);
bool Rvalue(Rule &myRule);
bool Rvalue_Prime(Rule &myRule);
bool Compare(Rule &myRule);
bool Mag(Rule &myRule);
bool Mag_Prime(Rule &myRule);
bool Term(Rule &myRule);
bool Term_Prime(Rule &myRule);
bool Factor(Rule &myRule);

bool Type(Rule &myRule)
{
	string tok = getToken();
	if (tok == "Adadi")
		myRule.type = "int";
	else if (tok == "Ashriya")
		myRule.type = "float";
	else if (tok == "Mantiqi")
		myRule.type = "bool";
	else if (tok == "Matn")
		myRule.type = "string";
	else if (tok == "Harf")
		myRule.type = "char";
	else
		return false;

	Advance();
	return true;
}

bool ArgList(Rule &myRule)
{
	if (!Arg(myRule))
		return false;

	if (!ArgList_Prime(myRule))
		return false;

	return true;
}

bool ArgList_Prime(Rule &myRule)
{
	if (getToken() == ",")
	{
		Advance();

		Rule argRule;
		if (!Arg(argRule))
			return false;

		if (!ArgList_Prime(argRule))
			return false;
	}
	return true;
}

bool Arg(Rule &myRule)
{
	if (!Type(myRule))
		return false;

	string currentToken = getToken();
	if (!findInSymbolTable(currentToken))
		return false;

	myRule.val = currentToken;
	gen("Arg: " + currentToken);
	Advance();

	return true;
}

bool Declaration(Rule &myRule)
{
	if (!Type(myRule))
		return false;

	if (!IdentList(myRule))
		return false;

	if (getToken() != "::")
		return false;
	Advance();

	gen("Declare " + myRule.val + " as " + myRule.type);
	return true;
}

bool IdentList(Rule &myRule)
{
	string currentToken = getToken();
	if (!findInSymbolTable(currentToken))
		return false;

	Advance();
	myRule.val = currentToken;

	if (!IdentList_Prime(myRule))
		return false;

	return true;
}

bool IdentList_Prime(Rule &myRule)
{
	if (getToken() == ",")
	{
		Advance();

		string currentToken = getToken();
		if (!findInSymbolTable(currentToken))
			return false;

		myRule.val += ", " + currentToken;
		// gen("Ident: " + currentToken);
		Advance();

		if (!IdentList_Prime(myRule))
			return false;
	}
	return true;
}

bool Stmt(Rule &myRule)
{
	Rule forRule;
	if (ForStmt(forRule))
	{
		myRule.val = forRule.val;
		return true;
	}

	Rule whileRule;
	if (WhileStmt(whileRule))
	{
		myRule.val = whileRule.val;
		return true;
	}

	Rule exprStmtRule;
	if (ExprStmt(exprStmtRule))
	{
		myRule.val = exprStmtRule.val;
		return true;
	}

	Rule ifRule;
	if (IfStmt(ifRule))
	{
		myRule.val = ifRule.val;
		return true;
	}

	Rule compStmtRule;
	if (CompStmt(compStmtRule))
	{
		myRule.val = compStmtRule.val;
		return true;
	}

	Rule declRule;
	if (Declaration(declRule))
	{
		myRule.val = declRule.val;
		return true;
	}

	if (getToken() == "::")
	{
		Advance();
		return true;
	}

	return false;
}

bool ExprStmt(Rule &myRule)
{
	if (!Expr(myRule))
		return false;

	if (getToken() != "::")
		return false;
	Advance();

	return true;
}

bool ForStmt(Rule &myRule)
{
	if (getToken() != "for")
		return false;
	Advance();

	if (getToken() != "(")
		return false;
	Advance();

	// Initialization
	Rule initRule;
	if (!Expr(initRule))
		return false;
	myRule.init = initRule.val;

	if (getToken() != "::")
		return false;
	Advance();

	// Condition
	Rule condRule;
	if (!OptExpr(condRule))
		return false;
	myRule.cond = condRule.val;

	if (getToken() != "::")
		return false;
	Advance();

	// Increment
	Rule incrRule;
	if (!OptExpr(incrRule))
		return false;
	myRule.incr = incrRule.val;

	if (getToken() != ")")
		return false;
	Advance();

	// Generate TAC for for loop
	myRule.start = "L" + to_string(labelValue++);
	myRule.looplabel = "L" + to_string(labelValue++);
	myRule.end = "L" + to_string(labelValue++);

	gen(myRule.start + ":");
	if (!myRule.cond.empty())
	{
		gen("if " + myRule.cond + " goto " + myRule.looplabel);
		gen("goto " + myRule.end);
	}
	gen(myRule.looplabel + ":");

	Rule stmtRule;
	if (!Stmt(stmtRule))
		return false;

	if (!myRule.incr.empty())
	{
		gen(myRule.incr);
	}
	gen("goto " + myRule.start);
	gen(myRule.end + ":");

	return true;
}

bool OptExpr(Rule &myRule)
{
	if (Expr(myRule))
	{
		return true;
	}
	return true;
}

bool WhileStmt(Rule &myRule)
{
	if (getToken() != "while")
		return false;
	Advance();

	if (getToken() != "(")
		return false;
	Advance();

	Rule ExprRule;
	if (!Expr(ExprRule))
		return false;

	myRule.start = "L" + to_string(labelValue++);
	myRule.looplabel = "L" + to_string(labelValue++);
	myRule.end = "L" + to_string(labelValue++);

	gen(myRule.start + ":");
	gen("if " + ExprRule.val + " goto " + myRule.looplabel);
	gen("goto " + myRule.end);
	gen(myRule.looplabel + ":");

	if (getToken() != ")")
		return false;
	Advance();

	Rule stmtRule;
	if (!Stmt(stmtRule))
		return false;

	gen("goto " + myRule.start);
	gen(myRule.end + ":");
	return true;
}

bool IfStmt(Rule &myRule)
{
	if (getToken() != "Agar")
		return false;
	Advance();

	if (getToken() != "(")
		return false;
	Advance();

	Rule ExprRule;
	if (!Expr(ExprRule))
		return false;

	myRule.trueLabel = "L" + to_string(labelValue++);
	myRule.falseLabel = "L" + to_string(labelValue++);

	gen("if " + ExprRule.val + " goto " + myRule.trueLabel);
	gen("goto " + myRule.falseLabel);
	gen(myRule.trueLabel + ":");

	if (getToken() != ")")
		return false;
	Advance();

	Rule stmtRule;
	if (!Stmt(stmtRule))
		return false;

	gen(myRule.falseLabel + ":");
	
	Rule elsePartRule;
	if (!ElsePart(elsePartRule))
		return false;

	return true;
}

bool ElsePart(Rule &myRule)
{
	if (getToken() == "Wagarna")
	{
		Advance();

		Rule stmtRule;
		if (!Stmt(stmtRule))
			return false;
		myRule.val = stmtRule.val;
	}
	return true;
}

bool CompStmt(Rule &myRule)
{
	if (getToken() != "{")
		return false;
	Advance();

	Rule stmtListRule;
	if (!StmtList(stmtListRule))
		return false;
	myRule.val = stmtListRule.val;

	if (getToken() != "}")
		return false;
	Advance();

	return true;
}

bool StmtList(Rule &myRule)
{
	Rule stmtRule;
	if (Stmt(stmtRule))
	{
		myRule.val = stmtRule.val;

		Rule stmtListRule;
		if (StmtList(stmtListRule))
		{
			myRule.val += "\n" + stmtListRule.val;
		}
	}
	return true;
}

bool Expr(Rule &myRule)
{
	string currentToken = getToken();
	bool isAssignment = peek() == ":=";

	if (isAssignment && findInSymbolTable(currentToken))
	{
		string varName = currentToken;
		Advance();
		if (getToken() != ":=")
			return false;
		Advance();

		if (!Expr(myRule))
			return false;

		gen(varName + " = " + myRule.val);
		myRule.val = varName;
		return true;
	}
	return Rvalue(myRule);
}

bool Rvalue(Rule &myRule)
{
	if (!Mag(myRule))
		return false;

	if (!Rvalue_Prime(myRule))
		return false;

	return true;
}

bool Rvalue_Prime(Rule &myRule)
{
	Rule compareRule;
	if (Compare(compareRule))
	{
		Rule magRule;
		if (!Mag(magRule))
			return false;

		string tempVar = "T" + to_string(tempIndex++);
		gen(tempVar + " = " + myRule.val + " " + compareRule.relop + " " + magRule.val);
		myRule.val = tempVar;

		if (!Rvalue_Prime(myRule))
			return false;
	}
	return true;
}

bool Compare(Rule &myRule)
{
	string tok = getToken();
	if (tok == "==" || tok == "<" || tok == ">" ||
		tok == "<=" || tok == ">=" || tok == "!=" || tok == "<>")
	{
		myRule.relop = (tok == "<>") ? "!=" : tok;
		Advance();
		return true;
	}
	return false;
}

bool Mag(Rule &myRule)
{
	if (!Term(myRule))
		return false;

	if (!Mag_Prime(myRule))
		return false;

	return true;
}

bool Mag_Prime(Rule &myRule)
{
	string tok = getToken();
	if (tok == "+" || tok == "-")
	{
		Advance();
		Rule termRule;
		if (!Term(termRule))
			return false;

		string tempVar = "T" + to_string(tempIndex++);
		gen(tempVar + " = " + myRule.val + " " + tok + " " + termRule.val);
		myRule.val = tempVar;

		if (!Mag_Prime(myRule))
			return false;
	}
	return true;
}

bool Term(Rule &myRule)
{
	if (!Factor(myRule))
		return false;

	if (!Term_Prime(myRule))
		return false;

	return true;
}

bool Term_Prime(Rule &myRule)
{
	string tok = getToken();
	if (tok == "*" || tok == "/")
	{
		Advance();
		Rule factorRule;
		if (!Factor(factorRule))
			return false;

		string tempVar = "T" + to_string(tempIndex++);
		gen(tempVar + " = " + myRule.val + " " + tok + " " + factorRule.val);
		myRule.val = tempVar;

		if (!Term_Prime(myRule))
			return false;
	}
	return true;
}

bool Factor(Rule &myRule)
{
	if (getToken() == "(")
	{
		Advance();
		Rule exprRule;
		if (!Expr(exprRule))
			return false;
		if (getToken() != ")")
			return false;
		Advance();
		myRule.val = exprRule.val;
		return true;
	}
	else if (findInSymbolTable(getToken()) || findInLiteralTable(getToken()))
	{
		myRule.val = getToken();
		Advance();
		return true;
	}
	return false;
}

bool ThreeAddressCode()
{
	Rule typeRule;
	if (!Type(typeRule))
		return false;

	string currentToken = getToken();
	if (!findInSymbolTable(currentToken))
		return false;
	Advance();

	if (getToken() != "(")
		return false;
	Advance();

	Rule argListRule;
	if (!ArgList(argListRule))
		return false;

	if (getToken() != ")")
		return false;
	Advance();

	Rule compStmtRule;
	if (!CompStmt(compStmtRule))
		return false;

	return true;
}

int main()
{
	if (!TacFile)
	{
		cout << "Failed to open Tac.txt" << endl;
		return 1;
	}

	setTokenVector(Tokens, "Tokens.txt");

	Rule mainRule;
	if (ThreeAddressCode())
	{
		cout << "TAC generation successful!" << endl;
	}
	else
		cout << "TAC generation failed!" << endl;

	TacFile.close();
	return 0;
}