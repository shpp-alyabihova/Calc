#ifndef EXPRESSION
#define EXPRESSION

#include "string"
#include "math.h"
using namespace std;



class Expression {
public:
   virtual double evaluate() const = 0;
   virtual ~Expression(){}
};

class Number : public Expression {
public:
    Number(double value)
        : value(value) {}
    double evaluate() const {
        return value;
    }
private:
    double value;
};

class Variable : public Expression {
public:
    Variable(string name, double value)
        : name(name), value(value) {}
    double evaluate() const {
        return value;
    }
private:
    string name;
    double value;
};


class BinaryOperation : public Expression {
public:

    BinaryOperation(Expression const * left, char op, Expression const * right)
        : left(left), op(op), right(right) {}
    double evaluate() const{
        switch(op){
            case '+' :
                return left->evaluate() + right->evaluate();
            case '-' :
                return left->evaluate() - right->evaluate();

            case '*' :
                return left->evaluate() * right->evaluate();
            case '/' :
            {
                if (right->evaluate() != 0)
                    return left->evaluate() / right->evaluate();
                else
                    error ("Further calculations are impossible because of the division by zero.");
            }
            case '^' :
                return pow(left->evaluate(), right->evaluate());
        }
        return 0;
    }


    ~BinaryOperation(){
        delete left;
        delete right;
    }

private:
    Expression const * left;
    char op;
    Expression const * right;
};

class Function : public Expression{
public:
    Function(string func, Expression const * right)
        : func(func), right(right){}

     double evaluate() const{
         if(func == "sin")
             return sin(right->evaluate());
         else if(func == "cos")
             return cos(right->evaluate());
         else if((func == "tan") || (func == "tg"))
             return tan(right->evaluate());
         return 0;
     }

    ~Function(){
        delete right;
    }
private:
    string func;
    Expression const * right;

};




#endif // EXPRESSION

