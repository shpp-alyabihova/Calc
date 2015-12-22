#include <iostream>
#include "console.h"
#include "simpio.h"
#include "tokenscanner.h"
#include "string"
#include "error.h"
#include "strlib.h"
#include "expression.h"
using namespace std;

//Checks the correct placement of parentheses of input string with formula
bool isBracketsPlacedCorrectly(string formula);

//Gets rid of the unary minus and replaces all commas with points
void formattingString(string & formula);

//Replaces negative values in parentheses (-value) with expression (0 - value)
void processingOfUnaryMinus(string &formula);

//Return priority of operation
int priority(string token);

//Prompts the user for the value of the variable
double requestValue(string variable);

//Reads an expression and checks for extra token
Expression * parseExp(TokenScanner & scanner);

//Reads operators and subexpressions until it finds an operator with higher priority.
//Next it is called recursively for operators with heigher priority.
Expression *readExp(TokenScanner & scanner, int priority);

//Reads terms and returns an operand of the expression
Expression *readTerm(TokenScanner & scanner);

//Prompts the user for the formula that should be calculated and outputs result after calculations
int main() {
    while (true){
        string formula = "";
        cout << "Please, input an expression: ";
        getline(cin, formula);

        if (formula == "") break; // if user enters nothing then the program stops execution
        else if(isBracketsPlacedCorrectly(formula)){
            formattingString(formula);
        }
        else{
            cout << "The brackets placed incorrectly" << endl;
            break;
        }

        TokenScanner scanner(formula);
        scanner.ignoreWhitespace();
        scanner.scanNumbers();
        Expression * parserExp = parseExp(scanner);
        cout << parserExp->evaluate() << endl;
    }
    return 0;
}


//======================================================================================================================

bool isBracketsPlacedCorrectly(string formula){
    int balance = 0;
    for(char symbol : formula){
        if(symbol == '(')
            ++balance;
        if(symbol == ')')
            --balance;
        if(balance < 0)
            return false;
    }
    if(balance != 0)
        return false;
    return true;
}

void formattingString(string & formula){
    processingOfUnaryMinus(formula);
    while(formula.find(",") != string::npos){
        formula.replace(formula.find(","), 1, ".");
    }
}

//it is assumed that all negative values are given in brackets
void processingOfUnaryMinus(string &formula){
    for(int i = 0; i < formula.length(); ++i){
        if ((formula[i] == '(') && (formula[i+1] == '-')){
            formula.insert(i + 1, "0");
        }
    }
}

//========================================================================================================================



Expression * parseExp(TokenScanner & scanner){
    Expression * exp = readExp(scanner, 0);
    if (scanner.hasMoreTokens()){
        error ("Unexpected token \"" + scanner.nextToken() + "\"");
    }
    return exp;
}


Expression *readExp(TokenScanner & scanner, int passPriority){
    Expression * exp = readTerm(scanner);
    string token = "";
    while(true){
        token = scanner.nextToken();
        int currentPriority = priority(token);
        if (currentPriority <= passPriority)
            break;
        Expression * rhs = readExp(scanner, currentPriority);
        if (isalpha(token[0]))
            exp = new Function(token, rhs);
        else
            exp = new BinaryOperation(exp, token[0], rhs);
    }
    scanner.saveToken(token);
    return exp;
}


Expression *readTerm(TokenScanner & scanner){
    string token = scanner.nextToken();
    if (isalpha(token[0])){
        if (token.length() > 1){ //it is assumed that variable has a single character
            token = toLowerCase(token); // corrects spelling name of function in lowercase letters
            scanner.saveToken(token); // puts back function to the TokenScanner for reading it as an operator
            return 0;
        }
        else
            return new Variable(token, requestValue(token));
    }
    if (isdigit(token[0])){
        return new Number(stringToDouble(token));
    }
    //for opening parenthesis:
    Expression * exp = readExp(scanner, 0); // starts read new expression with reset priority
    scanner.nextToken(); // read closing parenthesis
    return exp;
}

int priority(string token){
    if (token == "=")
        return 1;
    if ((token == "+") || (token == "-"))
        return 2;
    if ((token == "*") || (token == "/"))
        return 3;
    if (token == "^")
        return 4;
    if ((token == "sin") || (token == "cos") || (token == "tan") || (token == "tg"))
        return 5;
    return 0;
}


double requestValue(string variable){
    double value;
    cout << "Input value of variable " << variable << " = ";
    cin >> value;
    return value;
}
