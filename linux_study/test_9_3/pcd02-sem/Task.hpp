#pragma once
#include <iostream>


template<class T, class D>
class Task{

public:
    Task(T fun, D num1, D num2):_fun(fun),_arg0(num1),_arg1(num2)
    {}

    T fun() {
        return _fun;
    }

    D arg0() {
        return _arg0;
    }
    D arg1() {
        return _arg1;
    }

private:
    T _fun;
    D _arg0;
    D _arg1;
};