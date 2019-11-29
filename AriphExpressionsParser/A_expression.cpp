#include "A_expression.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

void A_expression::setExpression(const string& str)
{
	expression = str;
	result = str;
}

void A_expression::setExpression(const char* str)
{
	expression = str;
	result = str;
}

void A_expression::printExpression()
{
	cout << expression << endl;
}


inline size_t findNext(const string& str, const string& search, size_t pos)
{
	return pos =	pos + 1 < str.size()	? 
					str.find(search, ++pos) :
					string::npos;
}


void replaceString(string& subj, const string& search, const string& replace)
{
	size_t pos = subj.find(search);

	while (pos != string::npos)
	{
		subj.replace(pos, search.length(), replace);
		pos += replace.length();
		pos = subj.find(search, pos);
	}
}



void A_expression::pre_check()
{
	replaceString(result, " ", ""); // remove spaces;
	replaceString(result, "--", "+");
	replaceString(result, "+-", "-");

	correctNums(result);	 // throw expection if wrong numbers
	correctSymbols(result);	 // throw expection if wrong symbol
	correctFunction(result); // throw expection if wrong function
	correctBrackets(result); // throw expection if wrong structure of brackets
	correctBinaryOperations(result);
	correctExpression(result);	// throw expection is not found binary operator between arguments
}


void A_expression::displayResult()
{
	if (expression.empty())
	{
		cout << "выражение не задано" << endl;
		return;
	}

	try
	{
		pre_check();
		result = parse(result);
		double d = stod(result);

		printExpression();
		cout << "result: " << d << endl;
	}
	catch(const char* msg)
	{
		printExpression();
		cout << msg << endl;
	}
}



bool A_expression::correctNums(const string& str)
{
	size_t pos = result.find("..");
	if (pos != string::npos)
		throw "Недопустимая запись: в выражении подряд идут две десятичные точки";

	char c;
	bool correctNum = true;

	for (int i = 0; i < str.length(); i++)
	{
		c = str[i];
		if (isdigit(c) || c == '.')
		{
			int floatingPoints = 0;
			int numsAfterPoint = 0;

			while( (isdigit(c) || c == '.') && i < str.length() )
			{
				if (floatingPoints > 0)
				{
					if (isdigit(c))
					{
						numsAfterPoint++;
					}
					else if (c == '.')
					{
						throw "Недопустимая запись: более двух десятичных точек в числе";
						return correctNum = false;
					}
				}

				if (str[i] == '.')
				{
					floatingPoints++;
				}

				i++;
				c = str[i];
			}
			if (floatingPoints > 0 && numsAfterPoint == 0)
			{
				throw "Недопустимая запись: нет дробного числа после десятичной точки";
				return correctNum = false;
			}
		}
	}

	return correctNum; 
}



bool A_expression::correctSymbols(const string& str)
{
	char c;
	bool correctSymbol = true;

	for (int i = 0; i < str.size(); i++)
	{
		c = str[i];

		if (!isdigit(c) && c != '.' && c != '(' && c != ')')
		{
			bool foundBinOperation = false;

			for (int k = 0; k < supportedBinOperations.size(); k++)
			{
				if (c == supportedBinOperations[k])
				{
					foundBinOperation = true;
					break;
				}
			}

			if (!foundBinOperation && !isalpha(c))
			{
				throw "Недопустимый символ в выражении";
				return correctSymbol = false;
			}	
		}
	}

	return correctSymbol;
}



bool A_expression::correctBrackets(const string& str)
{
	bool correctBracket = true;

	int count = 0;
	size_t pos1 = str.find("(");
	size_t pos2 = str.find(")");
	int b1 = 0, b2 = 0;

	if (pos1 == string::npos && pos2 == string::npos)
	{
		return correctBracket = true; // no brackets is found
	}

	if (pos1 != string::npos)
		b1++;

	if (pos2 != string::npos)
		b2++;

	if (pos2 < pos1 || (pos1 == string::npos && pos2 != string::npos))
	{
		throw "Недопустимая запись: выражение начинается с закрывающей скобки";
		return correctBracket = false;
	}

	while (pos1 != string::npos && pos2 != string::npos)
	{
		if (pos2 < pos1)	// wrong structure of brackets
		{
			throw "Недопустимая запись: неверная структура открывающих и закрывающих скобок";
			return correctBracket = false;
		}

		pos1 = findNext(str, "(", pos1);
		pos2 = findNext(str, ")", pos2);

		if (pos1 != string::npos)
			b1++;

		if (pos2 != string::npos)
			b2++;
	}

	if (b1 != b2)
	{
		throw "Несоответствие между количеством открывающих и закрывающих скобок";
		return correctBracket = false;
	}
	else
	{
		return correctBracket;	// correct structure of brackets
	}							// N.B.! method doesn't check stuff inside brackets
}



bool A_expression::correctFunction(const string& str)
{
	char c;
	string function_identifier;
	bool correctFunction = true;

	for (int i = 0; i < str.size(); i++)
	{
		c = str[i];
		if (isalpha(c))		// check substr with bracket
		{
			do
			{
				function_identifier += c;
				i++;
				c = str[i];
			} while (isalpha(c) && i < str.size());


			bool one_of_functions = false;

			for (int j = 0; j < supportedFunctions.size(); j++) // check identifier
			{
				if (function_identifier == supportedFunctions[j])
				{
					one_of_functions = true;
					function_identifier.clear();
					break;
				}
			}
			if (!one_of_functions)
			{
				throw "Недопустимая запись функции, неизвестный идентификатор";
				return correctFunction = false;
			}

			else	// check brackets of function
			{
				int j = i;
				int brackets = 0;

				if (c == '(')
				{
					brackets++;
				}
				else
				{
					throw "Недопустимая запись функции, отсутствует открывающая скобка";
					return correctFunction = false;
				}

				while (brackets != 0 && ++j < str.size())
				{
					c = str[j];
					if (c == '(')
						brackets++;
					else if (c == ')')
						brackets--;
				}

				if (brackets > 0)
				{
					throw "Недопустимая запись функции, отсутствует закрывающая скобка";
					return correctFunction = false;
				}

			}
		} // end check substr
	}
	return correctFunction;
}



bool A_expression::correctBinaryOperations(const string& str) 
{
	char c;
	bool correctBinaryOperations = true;

	int first_and_last[2] = { 0, str.size() - 1 };

	for (int i = 0; i < 2; i++)
	{
		c = str.at(first_and_last[i]);

		for (int j = 0; j < supportedBinOperations.size(); j++)
		{
			if (c == supportedBinOperations[j])	
			{
				if (i == 0 && c != '-')	// operator '-' may be unary minus
				{
					throw "Недопустимая запись: выражение начинается со знака бинарной операции";
					return correctBinaryOperations = false;
				}
				else if (i == 1)
				{
					throw "Недопустимая запись: выражение заканчивается знаком бинарной операции";
					return correctBinaryOperations = false;
				}
			}
		}
	}


	for (int i = 0; i < str.size(); i++)
	{
		c = str[i];
		for (int j = 0; j < supportedBinOperations.size(); j++)
		{
			if (c == supportedBinOperations[j])
			{
				int next = i + 1;
				if (next < str.size())
				{
					c = str[next];
					for (int k = 0; k < supportedBinOperations.size(); k++)
					{
															// second operator '-' may be unary minus
						if (c == supportedBinOperations[k] && c != '-') 
						{										
							throw "Недопустимая запись: в выражении подряд идут два знака операции";
							return correctBinaryOperations = false;
						}
					}
				}
				break;
			}
		}
	}

	return correctBinaryOperations;
}



			// checks if binary operation is missing between arguments
bool A_expression::correctExpression(const string& str)
{
	char c;
	bool correctExpression = true;


	size_t pos = str.find(")(");

	if (pos != string::npos)
	{
		throw "Недопустимая запись: между закрывающей и открывающей скобкой отсутствует знак \nбинарной операции";
		return correctExpression = false;
	}


		// check binary operation after closing bracket
		// as well as binary operation after function (as it contains closing bracket at end)

	pos = str.find(")");	

	while (pos != string::npos)
	{
		if (pos + 1 < str.length())
		{
			c = str[pos + 1];
			bool foundBinOperation = false;

			for (int j = 0; j < supportedBinOperations.size(); j++)
			{
				if (c == supportedBinOperations[j])
				{
					foundBinOperation = true;
					break;
				}
			}
			if (!foundBinOperation)
			{
				if (isdigit(c) || c == '.')
				{
					throw "Недопустимая запись: между закрывающей скобкой и числом отсутствует знак \nбинарной операции";
					return correctExpression = false;
				}
				else if (isalpha(c))
				{
					throw "Недопустимая запись: между закрывающей скобкой и функцией отсутствует знак \nбинарной операции";
					return correctExpression = false;
				}
			}
		}

		pos = findNext(str, ")", pos);
	}


	for (int i = 0; i < str.size(); i++)	// check binary operation after number
	{
		c = str[i];

		if (isdigit(c) || c == '.')
		{
			while (++i < str.size())
			{
				c = str[i];
				if (!isdigit(c) && c != '.')
				{
					if (c == '(')
					{
						throw "Недопустимая запись: между числом и открывающей скобкой отсутствует знак \nоперации";
						return correctExpression = false;
					}
					else if (isalpha(c))
					{
						throw "Недопустимая запись: между числом и функцией отсутствует знак \nбинарной операции";
						return correctExpression = false;
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	return correctExpression;
}





string& A_expression::parse(string& str) // recursion function
{
	parseUnary(str);

	parseBrackets(str);

	parseBinary(str);

	return str;
}



string& A_expression::parseUnary(string& str)
{
	char c;

	for (int i = 0; i < str.size(); i++) // parse unary functions
	{
		c = str[i];
		if (isalpha(c))
		{
			int exp_init_pos = i;	// for further replacing
			int exp_end_pos = i;
			int exp_length = 0;

			string identifier;
			do
			{
				identifier += c;
				i++;
				c = str[i];
			} while (isalpha(c) && i < str.size());

			int argument_init_pos = i;
			int argument_end_pos = i;
			int argument_length = 0;

			int brackets = 0;

			if (c == '(')
				brackets++;
			else
				throw "Недопустимая запись функции, отсутствует открывающая скобка";

			while (brackets != 0 && ++i < str.size())
			{
				c = str[i];
				if (c == '(')
					brackets++;
				else if (c == ')')
					brackets--;
			}

			if (brackets > 0)
				throw "Недопустимая запись функции, отсутствует закрывающая скобка";
			else if (brackets == 0)
			{
				argument_end_pos = i;
				argument_length = argument_end_pos - argument_init_pos - 1;

				exp_end_pos = argument_end_pos;
				exp_length = exp_end_pos - exp_init_pos + 1;
			}
					// stuff between brackets
			string argument = str.substr(argument_init_pos + 1, argument_length);

			if (argument.empty())
			{
				throw "Недопустимая запись: отсутствует аргумент функции";
			}

			argument = parse(argument); // recursion

			double result = stod(argument);

			if (identifier == "abs")
			{
				result = abs(result);
			}
			else if (identifier == "sqrt")
			{
				if (result < 0)
					throw "Ошибка: подкоренное выражение - отрицательное число";

				result = sqrt(result);
			}
			else if (identifier == "exp")
			{
				result = exp(result);
			}
			else if (identifier == "lg")
			{
				if (result <= 0)
					throw "Ошибка: аргумент логарифмической функции - неположительное число";

				result = log10(result);
			}
			else if (identifier == "ln")
			{
				if (result <= 0)
					throw "Ошибка: аргумент логарифмической функции - неположительное число";

				result = log(result);
			}
			else if (identifier == "sin")
			{
				result = sin(result);
			}
			else if (identifier == "cos")
			{
				result = cos(result);
			}
			else if (identifier == "tg")
			{
				result = tan(result);
			}
			else if (identifier == "ctg")
			{
				result = 1 / tan(result);
			}

			string exp = to_string(result);

			str.replace(exp_init_pos, exp_length, exp);

			i = exp_init_pos + exp.length() - 1;
		}
	}

	return str;
}



	// not protected from wrong structure of bracket, need to call correctBrackets before
string& A_expression::parseBrackets(string& str)
{
	size_t pos1 = str.find("(");
	size_t pos2 = str.find(")");
	int b1 = 0, b2 = 0;

	size_t init_bracket = pos1;
	size_t end_bracket = pos1;
	size_t bracket_length = 0;
	
	while (pos1 != string::npos || pos2 != string::npos) 
	{
		pos1 = findNext(str, "(", pos1);

		while (pos1 < pos2 && pos1 != string::npos)
		{
			pos2 = findNext(str, ")", pos2);
			pos1 = findNext(str, "(", pos1);
		}

		if (pos1 > pos2 || pos1 == string::npos)
		{
			end_bracket = pos2;
			bracket_length = end_bracket - init_bracket + 1;
			size_t argument_length = bracket_length - 2; // without brackets

			string argument = str.substr(init_bracket + 1, argument_length);

			if (argument.empty())
			{
				throw "Недопустимая запись: между открывающей и закрывающей скобкой отсутствует \nвыражение";
			}

			argument = parse(argument); // recursion

			str.replace(init_bracket, bracket_length, argument);

			pos1 = init_bracket + argument.size();
			pos2 = pos1;

			pos1 = findNext(str, "(", pos1);
			pos2 = findNext(str, ")", pos2);

			init_bracket = pos1;
			end_bracket = pos1;
		}
	}

	return str;
}



string A_expression::getLeftOperand(const string& str, size_t operator_pos)
{
	string leftOperand = "";
	char c;

	if (operator_pos >= str.size() || operator_pos <= 0)
		return leftOperand;

	size_t leftOp_pos = operator_pos - 1;
	size_t leftOp_length = 0;

	for (int i = operator_pos - 1; i >= 0; i--)
	{
		c = str[i];

		if (isdigit(c) || c == '.')
		{
			leftOp_pos = i;
		}
		else if (!isdigit(c) && c != '.' && c != '-')
		{
			leftOp_pos = i + 1;
			break;
		}
		else if (c == '-')	// capture possible unary minus
		{
			if (i - 1 >= 0)
			{
				char cc = str[i - 1];

				bool foundBinOperation = false;

				for (int k = 0; k < supportedBinOperations.size(); k++)
				{
					if (cc == supportedBinOperations[k])
					{
						foundBinOperation = true;
						break;
					}
				}

				if (foundBinOperation) 
				{
					leftOp_pos = i;		// capture number with unary minus
				}
				else
				{
					leftOp_pos = i + 1;	// minus is not unary, but binary
				}
				break;
			}
			else // the leftmost symbol '-' is unary
			{
				leftOp_pos = i;
			}
		}
	}

	leftOp_length = operator_pos - leftOp_pos;

	leftOperand = str.substr(leftOp_pos, leftOp_length);

	return leftOperand;
}



string A_expression::getRightOperand(const string& str, size_t operator_pos)
{
	string rightOperand = "";
	char c;

	if (operator_pos + 1 >= str.size() || operator_pos < 0)
		return rightOperand;

	size_t rightOp_pos = operator_pos + 1;
	size_t rightOp_length = 0;

	int i = operator_pos + 1;

	if (i < str.size() && str[i] == '-')	// capture possible unary minus
	{
		rightOperand += str[i];
		i++;
		++rightOp_length;
	}
	for (; i < str.size(); i++)
	{
		c = str[i];

		if (isdigit(c) || c == '.')
		{
			rightOperand += c;

			++rightOp_length;
		}
		else
		{
			break;
		}
	}

	rightOperand = str.substr(rightOp_pos, rightOp_length);

	return rightOperand;
}



	// works only with correct numerical arguments
string& A_expression::parseBinary(string& str)
{
	string num1;
	string num2;
	char c;

	size_t operator_pos = 0;
	size_t exp_init_pos = 0;
	size_t exp_end_pos = 0;
	size_t exp_length = 0;

	for (int i = 0; i < str.length(); i++)
	{
		c = str[i];
		if (c == '*' || c == '/' || c == '%' || c == '^')
		{
			operator_pos = i;

			num1 = getLeftOperand(str, operator_pos);
			num2 = getRightOperand(str, operator_pos);

			if (num1.empty())
			{
				throw "Недопустимая запись: отсутствует левый операнд в бинарной операции";
			}
			else if (num2.empty())
			{
				throw "Недопустимая запись: отсутствует правый операнд в бинарной операции";
			}

			double n1 = stod(num1);
			double n2 = stod(num2);
			string result;

			c = str[operator_pos];

			switch (c)
			{
			case '*':
				result = to_string(n1 * n2);
				break;

			case '/':
				if (n2 == 0.0)
					throw "Деление на нуль";

				result = to_string(n1 / n2);
				break;

			case '%':
				result = to_string((int)n1 % (int)n2);
				break;

			case '^':
				if ((n1 == 0.0 && n2 <= 0.0) || (n1 < 0 && n2 != (int)n2))
					throw "Ошибка при возведении в степень. Нарушение границ домена";

				result = to_string(pow(n1, n2));
				break;

			default:
				throw "Бинарная операция не поддерживается";
				break;
			}

			exp_init_pos = operator_pos - num1.size();
			exp_end_pos = operator_pos + num2.size();
			exp_length = exp_end_pos - exp_init_pos + 1;

			str.replace(exp_init_pos, exp_length, result);

			num1.clear();
			num2.clear();

			i = exp_init_pos + result.length() - 1;

		}
	}


	for (int i = 0; i < str.length(); i++)
	{
		c = str[i];
		if (c == '+' || c == '-')
		{
			operator_pos = i;

			num1 = getLeftOperand(str, operator_pos);
			num2 = getRightOperand(str, operator_pos);

			if (c == '-' && num1.empty() && !num2.empty()) // unary minus
			{
				continue;
			}
			else if(c == '-' && num1.empty() && num2.empty())
			{
				throw "Недопустимая запись: отсутствует правый операнд в операции унарного минуса";
			}

			
			if (num1.empty())
			{
				throw "Недопустимая запись: отсутствует левый операнд в бинарной операции";
			}
			else if (num2.empty())
			{
				throw "Недопустимая запись: отсутствует правый операнд в бинарной операции";
			}

			double n1 = stod(num1);
			double n2 = stod(num2);
			string result;

			c = str[operator_pos];

			switch (c)
			{
			case '+':
				result = to_string(n1 + n2);
				break;

			case '-':
				result = to_string(n1 - n2);
				break;

			default:
				throw "Бинарная операция не поддерживается";
				break;
			}

			exp_init_pos = operator_pos - num1.size();
			exp_end_pos = operator_pos + num2.size();
			exp_length = exp_end_pos - exp_init_pos + 1;

			str.replace(exp_init_pos, exp_length, result);

			num1.clear();
			num2.clear();
			i = exp_init_pos + result.length() - 1;

		}
	}
			
	return str;
}
