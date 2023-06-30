#include <cassert>
#include <iostream>
#include <ostream>
#include "ftl.hpp"

using namespace ftl;
using namespace std;

struct ResultTest {
    Result<double, str> checked_div(double a, double b) {
        if (b == 0) {
            return Err(str{"div by zero"});
        }
        return Ok(a / b);
    }
    void operator()() {
        Result<double, str> res = checked_div(5, 2);
        cout << res << endl;
        cout << res.map(Some<double>) << endl;

        res = checked_div(5, 0);
        cout << res << endl;
    }
};

struct OptionTest {
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
    void operator()() {
        Option<int> opt = Some(5);
        cout << opt << endl;
        cout << opt.map([](int a){ return a * 2; }) << endl;
        cout << opt.map([](int a){ return Foo(a); }) << endl;

        Option<int &> opt_mut = opt.as_mut();
        opt_mut.unwrap()++;
        cout << opt << endl;

        Option<const int &> opt_ref = opt.as_ref();
        cout << opt_ref << endl;

        Option<int> foo = None();
        cout << foo << endl;
        cout << foo.ok_or_else([](){ return "kek"; }) << endl;
    }
};


int main() {
    ResultTest{}();
    OptionTest{}();

    return 0;
}
