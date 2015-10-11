#include <iostream>
#include "console.h"
#include "queue.h"
#include "simpio.h"
#include "strlib.h"
#include "hashmap.h"
#include "tokenscanner.h"
#include "stack.h"
#include "math.h"
#include "gwindow.h"
using namespace std;


// Function, which create a HashMap with all operators and mathematical functions as a key and priority of execution as a value.
HashMap<string, int> createMapPriority(){
    HashMap<string, int> priority;
    priority["="] = 0;
    priority["+"] = 1;
    priority["-"] = 1;
    priority["*"] = 2;
    priority["/"] = 2;
    priority["^"] = 3;
    priority["sqrt"] = 4;
    priority["sin"] = 4;
    priority["asin"] = 4;
    priority["cos"] = 4;
    priority["acos"] = 4;
    priority["tan"] = 4;
    priority["atan"] = 4;
    priority["exp"] = 4;
    priority["log"] = 4;
    priority["log2"] = 4;
    priority["log10"] = 4;
    priority["lg"] = 4;
    priority["ln"] = 4;
    priority["exp"] = 4;
    priority["abs"] = 4;
    priority["ceil"] = 4;
    priority["floor"] = 4;

    return priority;
}

//checks the correct placement of parentheses
bool isBracketsPlacedCorrectly(string formula);

//checks first symbol in a formula and if it is '-' takes the whole expression in parentheses
//also it replaces all the commas by points
void formattingString(string & formula);

//calculates a predetermined formula and optionally draws a graph of the function
string calculation(string  formula, HashMap<string, int> &priory);

Queue<string> createReversePolishNotation(string formula, HashMap<string, int> &priority);

//reads reverse polish notation of the formula in general
string applyingShuntingYardAlgoritm(Queue<string> & outputNotation, HashMap<string, int> & priority);

//reads reverse polish notation of the formula with request variables
string applyingShuntingYardAlgoritmWithRequestVariables(Queue<string> & outputNotation, HashMap<string, int> & priority);

//reads reverse polish notation of the formula with determination x for draw the grapth
string applyingShuntingYardAlgoritmWithDeterminationVariable(Queue<string> outputNotation, HashMap<string, int> & priority, double value);

//check next symbol after open parenthes and if it is '-' and next token is function or variable then instead of '-' uses '-1' and adds operator '*'
//if next token a number then merges two token '-' and number and get a negative number
void checkAndAddNegativeValue(TokenScanner &scanner, Queue<string> &outputNotation, HashMap<string, int> & priority);

//checks token for the absence of digit (operators previously drop out)
bool isNumber(string token);

void executeArithmeticOperation(string & token, Stack<string> & operands, Queue<string> & outputNotation);

//breckets nagative operands if it contains a variable
void bracketingNegativeOperand(string & operand);

//for operators
double executeCalculation(string token, double a, double b,  Stack<string> & operands, Queue<string> & outputNotation);

//for mathematical functions
double calculationFunction(string token, double a);

void plottingFunctions(Queue<string> & reverseNotation, HashMap<string, int> & priority);

void requestAndCheckTheSetValuesOfTheLimit(double & minLimit, double & maxLimit, double & interval); //for x for plotting function

//checks a point of the graph for being within a window
bool isPointInWindow(GWindow & gw, double coordX, double coordY);

//offers to enter the value for x or limits for plotting the graph
string requestValuesForX();


int main() {
    HashMap<string, int> priority = createMapPriority();
    while(true){
        string formula = "";
        cout << "Enter a formula (RETURN to quit): ";
        cin >> formula;
        cout << endl;
        //string formula = getLine("Enter a formula (RETURN to quit): ");
        formula = toLowerCase(formula);
        if (formula == "") break; // if user enters nothing then the program stops execution
        else if(isBracketsPlacedCorrectly(formula)){
            formattingString(formula);
        }
        else{
            cout << "The brackets placed incorrectly" << endl;
            break;
        }
        cout << "Result: " << calculation(formula, priority) << endl;
    }
    return 0;
}


//=====================================================================================================

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
    if(formula[0] == '-'){
        formula = "(" + formula + ")";
    }
    while(formula.find(",") != string::npos){
        formula.replace(formula.find(","), 1, ".");
    }
}


//=====================================================================================================

string calculation(string  formula, HashMap<string, int> &priority){

    Queue<string> outputNotation = createReversePolishNotation(formula, priority);
    string resultingFormula = applyingShuntingYardAlgoritm(outputNotation, priority);

    if(isNumber(resultingFormula)){
        return resultingFormula;
    }
    else {
        cout << resultingFormula << endl;
        while (true) {
            string answer = "";
            cout << "Do you want to determine the values of variables to obtain a numerical value or limits to graph the function? (Y/N) : ";
            cin >> answer;
            cout << endl;
            answer = toUpperCase(answer);



            //string answer = toUpperCase(getLine("Do you want to determine the values of variables to obtain a numerical value or limits to graph the function? (Y/N) : "));
            if(answer == "Y"){
                Queue<string> reverseNotation = createReversePolishNotation(resultingFormula, priority);
                resultingFormula = applyingShuntingYardAlgoritmWithRequestVariables(reverseNotation, priority);
                if(isNumber(resultingFormula)){
                    return resultingFormula;
                }
                else{
                    Queue<string> reverseNotation = createReversePolishNotation(resultingFormula, priority);
                    plottingFunctions(reverseNotation, priority);
                    resultingFormula = "Graph of the function y = " + resultingFormula;
                    break;
                }
            }
            else if(answer == "N"){
                resultingFormula = "y = " + resultingFormula;
                break;
            }
            else{
                cout << "Please select one of three options: V (values), L (limits) or Q(exit)";
            }
        }

    }


    return resultingFormula;
}

//==================================================================================================================

Queue<string> createReversePolishNotation(string formula, HashMap<string, int> &priority){
    Queue<string> outputNotation;
    Stack<string> operators;
    operators.push("="); // push '=' to stack was not empty for easier processing
    TokenScanner scanner(formula);
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    while(scanner.hasMoreTokens()){
        string token = scanner.nextToken();
        if(isdigit(token[0])){      // add digit to the Queque
            outputNotation.enqueue(token);
        }
        else if(token == "("){
            operators.push(token); // add "(" to the Stack
            checkAndAddNegativeValue(scanner, outputNotation, priority);  // add negative number or variable to Queue
        }
        else if((!priority.containsKey(token)) && (token != ")")){
            outputNotation.enqueue(token);   // add variable to the Queue
        }
        else if(token == ")"){
            while(operators.peek() != "("){
                outputNotation.enqueue(operators.pop()); // add all operators and functions concluded between the brackets to the Queue
            }
            operators.pop(); // remove "(" from Stack
        }
        else if((operators.peek() == "(") || (priority.get(token) > priority.get(operators.peek())) || ((operators.peek() == "^") && (token == "^"))){
            operators.push(token);  // add operator or function to the Stack
        }
        else {
            while(priority.get(token) <= priority.get(operators.peek())) {  // move all operators or function from the Stack to the Queue if their priority less or equal then new one
                if((operators.peek() == "^") && (token == "^")) break; // excludes equality right associative operators
                else {
                    outputNotation.enqueue(operators.pop());
                }
                if(operators.peek() == "=") break; // check the Stack to achieve the beginning where the "=" sign
                }
             operators.push(token); // add operator or funcrion to the Stack
        }
    }
    while(!(operators.peek() == "=")){
        outputNotation.enqueue(operators.pop()); // move all operators and function from the Stack to the Queue until we reach the beginning of the Stack where the "=" sign
    }
    return outputNotation;
}




void checkAndAddNegativeValue(TokenScanner &scanner, Queue<string> &outputNotation, HashMap<string, int> & priority){

    string token = scanner.nextToken();
    if(token == "-"){
        string nextToken = scanner.nextToken();
        if(isdigit(nextToken[0])){
            token += nextToken;
        }
        else{
            token += "1";
            scanner.saveToken(nextToken);
            scanner.saveToken("*");
        }
        outputNotation.enqueue(token);
    }
    else{
        scanner.saveToken(token);
    }

}


//====================================================================================================

string applyingShuntingYardAlgoritm(Queue<string> & outputNotation, HashMap<string, int> & priority){

    Stack<string> operands;

    while(!outputNotation.isEmpty()){
        string token = outputNotation.dequeue();
        if(!priority.containsKey(token)){
            operands.push(token);
        }
        else {
            executeArithmeticOperation(token, operands, outputNotation);
        }
    }

    return operands.pop();
}


//============================================================================================================

string applyingShuntingYardAlgoritmWithRequestVariables(Queue<string> & outputNotation, HashMap<string, int> & priority){

    Stack<string> operands;

    while(!outputNotation.isEmpty()){
        string token = outputNotation.dequeue();
        string valueX = "";
        if(!priority.containsKey(token)){
            if(!isNumber(token)) {
                if(token == "x"){
                    if(valueX == ""){
                        valueX = requestValuesForX();
                    }
                    token = valueX;
                }
                else{
                    string value = "";
                    cout << "Enter a value for the variable " << token << " = ";
                    cin >> value;
                    cout << endl;
                    token = value;
                   // token = getLine("Enter a value for the variable " + token + " = ");
                }
            }
            operands.push(token);
        }
        else {
            executeArithmeticOperation(token, operands, outputNotation);
        }
    }

    return operands.pop();
}

//=====================================================================================================

string applyingShuntingYardAlgoritmWithDeterminationVariable(Queue<string> outputNotation, HashMap<string, int> & priority, double value){

    Stack<string> operands;
    while(!outputNotation.isEmpty()){
        string token = outputNotation.dequeue();


        if(!priority.containsKey(token)){
            if(!isNumber(token)) {
                token = doubleToString(value);
            }
            operands.push(token);
        }
        else {
            executeArithmeticOperation(token, operands, outputNotation);
        }
    }

    return operands.pop();
}


//====================================================================================================================================================

string requestValuesForX(){
    string value = "";
        while (true) {
            string answer = "";
            cout << "Do you want to determine the value of x as a single numerical value or limits to graph the function? (V/L) : ";
            cin >> answer;
            cout << endl;
            answer = toUpperCase(answer);
            if(answer == "V"){

                cout << "Enter a value for the variable x = ";
                cin >> value;
                cout << endl;
                break;
            }
            else if(answer == "L"){
                value = "x";
                break;
            }
            else{
                cout << "Please select one of two options: V (values), L (limits).";
            }
        }


    return value;
}




//============================================================================================================


bool isNumber(string token){
    for(char symbol : token){
        if(isalpha(symbol)){
            return false;
        }
    }
    return true;
}


void executeArithmeticOperation(string & token, Stack<string> & operands, Queue<string> & outputNotation){
    string operand = operands.pop();
    if(token.length() == 1){
        string operandFirst = operands.pop();
        if((isNumber(operand)) && (isNumber(operandFirst))){
            double a = stringToDouble(operandFirst);
            double b = stringToDouble(operand);
            token = doubleToString(executeCalculation(token, a, b, operands, outputNotation));
        }
        else{
            bracketingNegativeOperand(operand);
            bracketingNegativeOperand(operandFirst);
            token = "(" + operandFirst + token + operand + ")";
        }
    }
    else{
        if(isNumber(operand)){
            double a = stringToDouble(operand);
            token = doubleToString(calculationFunction(token, a));
        }
        else{
            token = token + "(" + operand + ")";
        }
    }
    operands.push(token);

}

void bracketingNegativeOperand(string & operand){
    if(operand[0] == '-'){
        operand = "(" + operand + ")";
    }
}

double executeCalculation(string token, double a, double b, Stack<string> & operands, Queue<string> & outputNotation){
        double result = 0;
        char operation = stringToChar(token);
        switch(operation){
            case '+':
                result = a + b;
                break;
            case '-':
                result = a - b;
                break;
            case '*':
                result = a * b;
                break;
            case '/':
                if (b == 0){
                    while(!operands.isEmpty()){
                        operands.pop();
                    }
                    operands.push("Eror");
                    while(!outputNotation.isEmpty()){
                        outputNotation.dequeue();
                    }
                    cout << "Further calculations are impossible because of the division by zero." << endl << "Result is not correct" << endl;
                }
                else result = a / b;
                break;
            case '^':
                result = pow(a, b);
                break;
        }
        return round(result*1000)/1000;
}

double calculationFunction(string token, double a){
    double result = 0;
    if(token == "sin"){
        result = sin(a);
    }
    else if(token == "cos"){
        result = cos(a);
    }
    else if(token == "abs"){
        result = abs((int)a);
    }
    else if(token == "acos"){
        result = acos(a);
    }
    else if(token == "asin"){
        result = asin(a);
    }
    else if(token == "tan"){
        result = tan(a);
    }
    else if(token == "atan"){
        result = atan(a);
    }
    else if(token == "ceil"){
        result = ceil(a);
    }
    else if(token == "exp"){
        result = exp(a);
    }
    else if(token == "sqrt"){
        result = sqrt(a);
    }
    else if(token == "floor"){
        result = floor(a);
    }
    else if((token == "ln") || (token == "log")){
        result = log(a);
    }
    else if((token == "lg") || (token == "log10")){
        result = log10(a);
    }
    else if(token == "log2"){
        result = log2(a);
    }
    return round(result*1000)/1000;
}



//=====================================================================================================

void plottingFunctions(Queue<string> & reverseNotation, HashMap<string, int> & priority){
    double minLimit = 0;
    double maxLimit = 0;
    double interval = 0;
    requestAndCheckTheSetValuesOfTheLimit(minLimit, maxLimit, interval);

    double scale = 1;
    cout << "Enter a value graph's scale, which must be greater than zero: ";
    cin >> scale;
    cout << endl;
    scale <= 0 ? 1 : scale;

    GWindow gw(800, 600); // create a window for plotting the grapth
    gw.drawLine(0, gw.getHeight()/2, gw.getWidth(), gw.getHeight()/2); // adds X axis
    gw.drawLine(gw.getWidth()/2, 0, gw.getWidth()/2, gw.getHeight());  // adds Y axis



    while(minLimit <= maxLimit){
        string result = applyingShuntingYardAlgoritmWithDeterminationVariable(reverseNotation, priority, minLimit);
        double X = minLimit;
        double Y = stringToDouble(result);
        cout << X << " " << Y << endl;
        minLimit += interval;



        double radiusOval = 1; // radius of the point of the graph
        double coordX = gw.getWidth()/2 + X*scale - radiusOval;
        double coordY = gw.getHeight()/2 - Y*scale - radiusOval;
        if(isPointInWindow(gw, coordX, coordY)){
            gw.fillOval(coordX, coordY, 2*radiusOval, 2*radiusOval); // adds point of the graph as a fill oval with radius equal 1
        }
    }


    return;
}


void requestAndCheckTheSetValuesOfTheLimit(double & minLimit, double & maxLimit, double & interval){
    while (true) {

      /*  minLimit = stringToDouble(getLine("Enter a min value of the limit = "));
        maxLimit = stringToDouble(getLine("Enter a max value of the limit = "));
        interval = stringToDouble(getLine("Enter a value of interval = "));*/



        cout << "Enter min and max value of variable and value of the interval:" << endl;
        cout << "min value of the limit = " ;
        cin >> minLimit;
        cout << " " << "max value of the limit = ";
        cin >> maxLimit;
        cout << " " << "value of interval = ";
        cin >> interval;
        if((minLimit + interval <= maxLimit) && (interval > 0)){
            return;
        }
        else{
            cout << "The entered values are not valid. Try again.";
        }
    }
}


bool isPointInWindow(GWindow & gw, double coordX, double coordY){
    if((coordX >= gw.getWidth()) || (coordX <= 0) || (coordY >= gw.getHeight()) || (coordY <= 0)){
        return false;
    }
    return true;
}

