#include <iostream>
#include <string>
#include <conio.h>
#include "A_expression.h"

using namespace std;

void main()
{
	setlocale(LC_CTYPE, "RUS");
	setlocale(LC_NUMERIC, "en_US.UTF-8");

	A_expression exp;


	cout << "1 - test your own expression" << endl;
	cout << "2 - test default examples" << endl;

	int answ = 0;
	cin >> answ;

	if (answ == 1)
	{
		string user_expression;

		do
		{
			system("cls");
			cout << "Please enter your expression" << endl;

			if (cin.peek() == '\n')
				cin.get();

			getline(cin, user_expression);

			if (user_expression == "exit")
			{
				break;
			}
			else
			{
				exp.setExpression(user_expression);
				exp.displayResult();
			}

			cout << "\n\n" << "test again?" << endl;
			cout << "1 - yes \t any other - no" << endl;
			cin >> answ;

		} while (answ == 1);
	}

	else if (answ == 2)
	{
		system("cls");

		cout << "1 - test working default expressions" << endl;
		cout << "2 - test default expressions with errors" << endl;
		cin >> answ;
		cout << "\n\n";

		if (answ == 1)
		{
			vector<string> default_expressions = {
				"sqrt(9.0) * (3 + 5 % 4)",			// 3 * (3 + 1) = 12
				".1 * 10 + ( 6 / 2 * (3 + 1) )",	// 1 + (6 / 2) * 4 = 1 + 12 = 13
				"sin(15)^2 + cos(15)^2",			// 1	
				"exp(1)",							// 2.17...
				"abs( -15.6 / 30 ) -- 1",			// 0.52 + 1 = 1.52
				"15 / 3 + 6 / 2 % 2"				// 5 + 3 % 2 = 5 + 1 = 6
			};

			do
			{
				system("cls");

				for (int i = 0; i < default_expressions.size(); i++)
				{
					cout << "#" << i + 1 << "\t" << default_expressions[i] << endl;
				}

				cout << "\n\n" << "select one of expressions" << endl;
				cin >> answ;

				if (answ > 0 && answ <= default_expressions.size())
				{
					system("cls");
					exp.setExpression(default_expressions[answ - 1]);
					exp.displayResult();
					_getch();

					cout << "\n\n" << "test again?" << endl;
					cout << "1 - yes \t any other - no" << endl;
					cin >> answ;
				}

			} while (answ == 1);

		}
		else if (answ == 2)
		{
			vector<string> default_errors =
			{
				"12 ? 5",

				"12..15",
				"12.1.5",
				"12.",

				"15 + 5)",
				"(15) + 3) + (2 * (-5)",
				"(15 + (5)",
				"5 + ( )",
				"(2 + 2) (3 + 2)",
				"(2 + 2) 3",
				"(3 + 2) exp(12)",
				"3 (2^8)",
				"45.5 sin(3.14)",

				"f(5)",
				"abs",
				"abs(-5",
				"abs( )",

				"+ 2",
				"2 +",
				".12 % + 12",

				"5 * (-)",
				"5 * (  + 2)",
				"5 ^ (2 +  )",

				"1 / 0",
				"sqrt(-5)",
				"lg(-5)",
				"0^0",
				"-1^1.5",
				""
			};

			for (int i = 0; i < default_errors.size(); i++)
			{
				exp.setExpression(default_errors[i]);
				exp.displayResult();
				cout << "\n\n";
				_getch();
			}
		}
	}
}