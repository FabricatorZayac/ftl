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

        opt = None();
        cout << opt << endl;
        cout << opt.ok_or_else([](){ return "kek"; }) << endl;
    }
};

struct OptionRefTest {
    struct Data { int a; };
    struct Widget {
        Option<Data> foo; 
        Option<Data&> data() {
            return foo.as_ref();
        }
    };
    void operator()() {
        Widget bar{ Some(Data{ 5 }) };
        Option<Data&> data = bar.data();
        assert(data.unwrap().a == 5);
    }
};

int main() {
    ResultTest{}();
    OptionTest{}();
    OptionRefTest{}();

    return 0;
}
