#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

class Expression;
using ExprPtr = shared_ptr<Expression>;

ExprPtr makeAdd(ExprPtr a, ExprPtr b);
ExprPtr makeMul(ExprPtr a, ExprPtr b);

class Expression {
public:
    virtual ~Expression() {}
    virtual double evaluate(double x) = 0;
    virtual ExprPtr derivative() = 0;
    virtual string toString() = 0;

    virtual bool isZero() const { return false; }
    virtual bool isOne() const { return false; }
};

class Constant : public Expression {
public:
    Constant(double v) : value_(v) {}
    double evaluate(double) override { return value_; }
    ExprPtr derivative() override { return make_shared<Constant>(0); }
    string toString() override {
        ostringstream os;
        os << value_;
        return os.str();
    }
    bool isZero() const override { return value_ == 0; }
    bool isOne() const override { return value_ == 1; }
private:
    double value_;
};

class Variable : public Expression {
public:
    double evaluate(double x) override { return x; }
    ExprPtr derivative() override { return make_shared<Constant>(1); }
    string toString() override { return "x"; }
};

class BinaryOperation : public Expression {
public:
    BinaryOperation(ExprPtr l, ExprPtr r) : left_(l), right_(r) {}
    string toString() override {
        return "(" + left_->toString() + " " + symbol() + " " + right_->toString() + ")";
    }
protected:
    virtual string symbol() const = 0;
    ExprPtr left_, right_;
};

class Add : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;
    double evaluate(double x) override { return left_->evaluate(x) + right_->evaluate(x); }
    ExprPtr derivative() override { return makeAdd(left_->derivative(), right_->derivative()); }
protected:
    string symbol() const override { return "+"; }
};

class Multiply : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;
    double evaluate(double x) override { return left_->evaluate(x) * right_->evaluate(x); }
    ExprPtr derivative() override {
        return makeAdd(makeMul(left_->derivative(), right_),
                       makeMul(left_, right_->derivative()));
    }
protected:
    string symbol() const override { return "*"; }
};

class Power : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;
    double evaluate(double x) override { return pow(left_->evaluate(x), right_->evaluate(x)); }
    ExprPtr derivative() override {
        double n = right_->evaluate(0);   // el exponente es constante
        ExprPtr newPow;
        if (n - 1 == 0)      newPow = make_shared<Constant>(1);
        else if (n - 1 == 1) newPow = left_;
        else                 newPow = make_shared<Power>(left_, make_shared<Constant>(n - 1));
        return makeMul(makeMul(make_shared<Constant>(n), newPow), left_->derivative());
    }
protected:
    string symbol() const override { return "^"; }
};

class UnaryFunction : public Expression {
public:
    UnaryFunction(ExprPtr arg) : arg_(arg) {}
    string toString() override { return name() + "(" + arg_->toString() + ")"; }
protected:
    virtual string name() const = 0;
    ExprPtr arg_;
};

class Sin : public UnaryFunction {
public:
    using UnaryFunction::UnaryFunction;
    double evaluate(double x) override { return sin(arg_->evaluate(x)); }
    ExprPtr derivative() override;   // se define abajo (usa Cos)
protected:
    string name() const override { return "sin"; }
};

class Cos : public UnaryFunction {
public:
    using UnaryFunction::UnaryFunction;
    double evaluate(double x) override { return cos(arg_->evaluate(x)); }
    ExprPtr derivative() override;   // se define abajo (usa Sin)
protected:
    string name() const override { return "cos"; }
};

ExprPtr Sin::derivative() {
    return makeMul(make_shared<Cos>(arg_), arg_->derivative());
}

ExprPtr Cos::derivative() {
    return makeMul(makeMul(make_shared<Constant>(-1), make_shared<Sin>(arg_)),
                   arg_->derivative());
}

ExprPtr makeAdd(ExprPtr a, ExprPtr b) {
    if (a->isZero()) return b;
    if (b->isZero()) return a;
    return make_shared<Add>(a, b);
}

ExprPtr makeMul(ExprPtr a, ExprPtr b) {
    if (a->isZero() || b->isZero()) return make_shared<Constant>(0);
    if (a->isOne()) return b;
    if (b->isOne()) return a;
    return make_shared<Multiply>(a, b);
}

int main() {
    auto x = make_shared<Variable>();
    auto expr =
        make_shared<Multiply>(
            make_shared<Add>(
                make_shared<Power>(
                    x,
                    make_shared<Constant>(2)
                ),
                make_shared<Multiply>(
                    make_shared<Constant>(3),
                    x
                )
            ),
            make_shared<Sin>(x)
        );

    auto deriv = expr->derivative();

    cout << "f(x) = " << expr->toString() << endl;
    cout << "f'(x) = " << deriv->toString() << endl;
    cout << "f(2) = " << expr->evaluate(2) << endl;
}