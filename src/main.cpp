#include <iostream>
#include <ostream>
#include "ftl.hpp"

using namespace ftl;
using namespace std;

Result<double, str> checked_div(double a, double b) {
    if (b == 0) {
        return Err((str)"div by zero");
    }
    return Ok(a / b);
}

Result<void, const char *> fail() {
    return Err("fail");
}

Result<void, const char *> success() {
    return Ok();
}

Option<int> some_int() {
    return Some(5);
}

struct Foo {
    Foo() = delete;
    Foo(int) {}
    friend ostream &operator<<(ostream &out, Foo &self) {
        (void)self;
        return out << "Foo";
    }
    ~Foo() {
        cout << "deleted Foo" << endl;
    }
};

int main() {
    auto res = checked_div(5, 2);
    cout << res << endl;
    cout << res.map(Some<double>) << endl;

    res = checked_div(5, 0);
    cout << res << endl;

    auto error = fail();
    cout << error << endl;

    auto opt = some_int();
    cout << opt << endl;
    cout << opt.map([](int a){return a * 2; }) << endl;
    cout << opt.map([](int a){ return Foo(a); }) << endl;

    opt = None();
    cout << opt << endl;
    cout << opt.ok_or_else([](){ return "kek"; }) << endl;

    return 0;
}
