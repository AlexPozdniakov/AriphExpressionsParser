#pragma once
#include <iostream>
#include <vector>

using std::string;
using std::vector;

class A_expression
{
	string expression;	// initial string, entered by user
	string result;		// used for counting all temporary results
	bool isValid = true;

	vector<char> supportedBinOperations = { '+', '-', '*', '/', '^', '%' };
	vector<string> supportedFunctions = { "sin", "cos", "tg", "ctg", "sqrt", "lg", "ln", "abs", "exp" };


	void pre_check(); // initial check
	bool correctNums(const string& str);
	bool correctSymbols(const string& str);
	bool correctBrackets(const string& str);
	bool correctFunction(const string& str);
	bool correctBinaryOperations(const string& str);
	bool correctExpression(const string& str);

	string& parse(string& str);

	string& parseUnary(string& str);
	string& parseBrackets(string& str);
	string& parseBinary(string& str);
	string getLeftOperand(const string& str, size_t operator_pos);
	string getRightOperand(const string& str, size_t operator_pos);

public:
	A_expression() : A_expression((string)"12.5 + 3.14 * ( 5 % 2 -- 1.5)") { }
	A_expression(const char* str) : A_expression(string(str)) { }
	A_expression(const string& exp) : expression(exp), result(exp) { }

	void setExpression(const string& str);
	void setExpression(const char* str);

	void printExpression();
	void displayResult();
};