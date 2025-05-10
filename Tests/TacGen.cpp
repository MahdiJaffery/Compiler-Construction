#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

struct Rule
{
	//	Compare
	string relop;

	//	General Values
	string val;

	//	If-Else
	string trueLabel, falseLabel, label;

	//	Types
	string type;

	//	While Loop
	string start, looplabel, end;

	Rule() : relop(""), val(""), trueLabel(""), falseLabel(""), label(""), type(""), start(""), looplabel(""), end("") {}
};

ofstream TacFile("Tac.txt", ios::app);

vector<string> Tokens;
int currentIndex = 0, tempIndex = 1, labelValue = 1;

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

bool Stmt(Rule &myRule);
bool Declaration(Rule &myRule);
bool Type(Rule &myRule);
bool IdentList(Rule &myRule);
bool IdentList_Prime(Rule &myRule);
bool WhileStmt(Rule &myRule);
bool IfStmt(Rule &myRule);
bool ElsePart(Rule &myRule);
bool CompStmt(Rule &myRule);
bool StmtList(Rule &myRule);
bool Expr(Rule &myRule);
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

bool IdentList(Rule &myRule)
{
	string currentToken = getToken();
	if (!findInSymbolTable(currentToken))
		return false;

	Advance();
	myRule.val = currentToken;

	if (getToken() == ",")
	{
		Advance();
		Rule IdentListRule;
		if (!IdentList(IdentListRule))
			return false;
		myRule.val += ", " + IdentListRule.val;
	}
	return true;
}

bool Declaration(Rule &myRule)
{
	Rule TypeRule;
	if (!Type(TypeRule))
		return false;

	Rule IdentListRule;
	if (!IdentList(IdentListRule))
		return false;

	if (getToken() != "::")
		return false;
	Advance();

	gen("Declare " + IdentListRule.val + " as " + TypeRule.type);
	return true;
}

bool Factor(Rule &myRule)
{
	if (getToken() == "(")
	{
		Advance();
		Rule ExprRule;
		if (!Expr(ExprRule))
			return false;
		if (getToken() != ")")
			return false;
		Advance();
		myRule.val = ExprRule.val;
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

bool Term_Prime(Rule &myRule)
{
	string tok = getToken();
	if (tok == "*" || tok == "/")
	{
		Advance();
		Rule FactorRule;
		if (!Factor(FactorRule))
			return false;

		string tempVar = "T" + to_string(tempIndex++);
		gen(tempVar + " = " + myRule.val + tok + FactorRule.val);
		myRule.val = tempVar;

		Rule Term_Prime_Rule;
		Term_Prime_Rule.val = myRule.val;
		if (!Term_Prime(Term_Prime_Rule))
			return false;
		myRule.val = Term_Prime_Rule.val;
	}
	return true;
}

bool Term(Rule &myRule)
{
	Rule FactorRule;
	if (!Factor(FactorRule))
		return false;

	myRule.val = FactorRule.val;

	Rule Term_Prime_Rule;
	Term_Prime_Rule.val = myRule.val;
	if (!Term_Prime(Term_Prime_Rule))
		return false;
	myRule.val = Term_Prime_Rule.val;

	return true;
}

bool Mag_Prime(Rule &myRule)
{
	string tok = getToken();
	if (tok == "+" || tok == "-")
	{
		Advance();
		Rule TermRule;
		if (!Term(TermRule))
			return false;

		string tempVar = "T" + to_string(tempIndex++);
		gen(tempVar + " = " + myRule.val + tok + TermRule.val);
		myRule.val = tempVar;

		Rule Mag_Prime_Rule;
		Mag_Prime_Rule.val = myRule.val;
		if (!Mag_Prime(Mag_Prime_Rule))
			return false;
		myRule.val = Mag_Prime_Rule.val;
	}
	return true;
}

bool Mag(Rule &myRule)
{
	Rule TermRule;
	if (!Term(TermRule))
		return false;

	myRule.val = TermRule.val;

	Rule Mag_Prime_Rule;
	Mag_Prime_Rule.val = myRule.val;
	if (!Mag_Prime(Mag_Prime_Rule))
		return false;
	myRule.val = Mag_Prime_Rule.val;

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

bool Rvalue_Prime(Rule &myRule)
{
	Rule CompareRule;
	if (Compare(CompareRule))
	{
		Rule MagRule;
		if (!Mag(MagRule))
			return false;

		string tempVar = "T" + to_string(tempIndex++);
		gen(tempVar + " = " + myRule.val + " " + CompareRule.relop + " " + MagRule.val);
		myRule.val = tempVar;

		Rule Rvalue_Prime_Rule;
		Rvalue_Prime_Rule.val = myRule.val;
		if (!Rvalue_Prime(Rvalue_Prime_Rule))
			return false;
		myRule.val = Rvalue_Prime_Rule.val;
	}
	return true;
}

bool Rvalue(Rule &myRule)
{
	Rule MagRule;
	if (!Mag(MagRule))
		return false;

	myRule.val = MagRule.val;

	Rule Rvalue_Prime_Rule;
	Rvalue_Prime_Rule.val = myRule.val;
	if (!Rvalue_Prime(Rvalue_Prime_Rule))
		return false;
	myRule.val = Rvalue_Prime_Rule.val;

	return true;
}

bool Expr(Rule &myRule)
{
	Rule RvalueRule;
	bool isAssignment = peek() == ":=";

	if (!isAssignment)
	{
		if (Rvalue(RvalueRule))
		{
			myRule.val = RvalueRule.val;
			return true;
		}
		return false;
	}
	else
		if (findInSymbolTable(getToken()))
		{
			string varName = getToken();
			Advance();
			if (getToken() != ":=")
				return false;
			Advance();

			Rule ExprRule;
			if (!Expr(ExprRule))
				return false;

			gen(varName + " = " + ExprRule.val);
			myRule.val = varName;
			return true;
		}
	else return false;
}

bool ElsePart(Rule &myRule)
{
	if (getToken() == "Wagarna")
	{
		Advance();
		Rule StmtRule;
		if (!Stmt(StmtRule))
			return false;
		myRule.val = StmtRule.val;
	}
	return true;
}

bool CompStmt(Rule &myRule)
{
	if (getToken() != "{")
		return false;
	Advance();

	Rule StmtListRule;
	if (!StmtList(StmtListRule))
		return false;
	myRule.val = StmtListRule.val;

	if (getToken() != "}")
		return false;
	Advance();

	return true;
}

bool StmtList(Rule &myRule)
{
	Rule StmtRule;
	if (Stmt(StmtRule))
	{
		myRule.val = StmtRule.val;

		Rule StmtListRule;
		if (StmtList(StmtListRule))
		{
			myRule.val += "\n" + StmtListRule.val;
		}
	}
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

	if (getToken() != ")")
		return false;
	Advance();

	myRule.trueLabel = "L" + to_string(labelValue++);
	myRule.falseLabel = "L" + to_string(labelValue++);

	gen("if " + ExprRule.val + " goto " + myRule.trueLabel);
	gen("goto " + myRule.falseLabel);
	gen(myRule.trueLabel + ":");

	Rule StmtRule;
	if (!Stmt(StmtRule))
		return false;

	Rule ElsePartRule;
	if (!ElsePart(ElsePartRule))
		return false;

	gen(myRule.falseLabel + ":");
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

	if (getToken() != ")")
		return false;
	Advance();

	myRule.start = "L" + to_string(labelValue++);
	myRule.looplabel = "L" + to_string(labelValue++);
	myRule.end = "L" + to_string(labelValue++);

	gen(myRule.start + ":");
	gen("if " + ExprRule.val + " goto " + myRule.looplabel);
	gen("goto " + myRule.end);
	gen(myRule.looplabel + ":");

	Rule StmtRule;
	if (!Stmt(StmtRule))
		return false;

	gen("goto " + myRule.start);
	gen(myRule.end + ":");
	return true;
}

bool Stmt(Rule &myRule)
{
	Rule WhileRule;
	if (WhileStmt(WhileRule))
	{
		myRule.val = WhileRule.val;
		return true;
	}

	Rule IfRule;
	if (IfStmt(IfRule))
	{
		myRule.val = IfRule.val;
		return true;
	}

	Rule ExprRule;
	if (Expr(ExprRule))
	{
		if (getToken() != "::")
			return false;
		Advance();
		myRule.val = ExprRule.val;
		return true;
	}

	Rule CompStmtRule;
	if (CompStmt(CompStmtRule))
	{
		myRule.val = CompStmtRule.val;
		return true;
	}

	Rule DeclarationRule;
	if (Declaration(DeclarationRule))
	{
		myRule.val = DeclarationRule.val;
		return true;
	}

	if (getToken() != "::")
		return false;
	Advance();
	return true;
}

bool ThreAddressCode(vector<Rule> &RuleList)
{
	bool flag = true;
	while (flag && currentIndex < Tokens.size())
	{
		flag = flag && Stmt(RuleList[0]);
	}
	return flag;
}

int main()
{
	vector<Rule> RuleList(1);

	if (!TacFile)
	{
		cout << "Failed to open Tac.txt" << endl;
		return 1;
	}

	setTokenVector(Tokens, "Tokens.txt");

	if (ThreAddressCode(RuleList))
		cout << "TAC generation successful!" << endl;
	else
		cout << "TAC generation failed!" << endl;

	TacFile.close();
	return 0;
}
