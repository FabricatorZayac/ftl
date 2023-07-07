#include <cassert>
#include <functional>
#include <iostream>

#include "ftl.hpp"

using namespace ftl;
using namespace std;

Result<double, str> checked_div(double a, double b) {
    if (b == 0) {
        return Err(str("div by zero"));
    }
    return Ok(a / b);
}

struct Foo {
    Foo() {}
    Foo(const Foo &) {}
    bool operator=(const Foo &) { return true; }
};

struct Bar {
    int a;
    bool flag;

    Bar(int a, bool flag) : a(a), flag(flag) {}
    Bar(const Bar &other) : a(other.a), flag(other.flag) {}

    Result<int &, str> get() {
        if (flag) return Ok(ref(a));
        else return Err(str("flag is false"));
    }
};

int main() {
    Result<double, str> res = checked_div(5, 2);
    assert(res.is_ok_and([](double a){ return a == 2.5; }));
    assert(res == Ok(2.5));
    assert(res.map([](double a){ return a * 4; }) == Ok(10));

    res = checked_div(5, 0);
    assert(res.is_err_and([](str a){ return a == "div by zero"; }));
    assert(res == Err(str("div by zero")));

    Result<Foo, str> foo = Ok(Foo());
    Result<Foo, str> foo_copy = foo;

    Bar bar(5, true);

    Result<int &, str> bar_res = bar.get();
    bar_res.unwrap()++;
    assert(bar.a == 6);

    bar.flag = false;
    assert(bar.get().unwrap_err() == "flag is false");

    Result<int &, str> bar_res_copy = bar_res;
    bar_res_copy.unwrap()++;
    assert(bar.a == 7);

    assert(Ok(5).map([](int a){ return a * 2; }).unwrap() == 10);

    return 0;
}
