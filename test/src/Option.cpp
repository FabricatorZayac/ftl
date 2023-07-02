#include <cassert>
#include "ftl.hpp"

using namespace std;
using namespace ftl;

struct Foo {
    Foo() = delete;
    Foo(int) {}
    friend ostream &operator<<(ostream &out, Foo &) {
        return out << "Foo";
    }
    bool operator==(Foo) { return true; }
};

int main () {
    Option<int> opt = Some(5);
    assert(opt == Some(5));

    assert(opt.map([](int a){ return a * 3; }) == Some(15));
    assert(opt.map([](int a){ return Foo(a); }) == Some(Foo(1)));

    Option<const int &> opt_ref = opt.as_ref();
    Option<int &> opt_mut = opt.as_mut();
    opt_mut.unwrap() *= 2;
    assert(opt == Some(10));
    assert(opt_ref.unwrap() == 10);

    Option<int> foo = None();
    assert(foo == None());

    // assert(opt.ok_or_else([](){
    //             cout << "opt else run" << endl;
    //             return str("kek");
    //         }) == Err(str("kek")));
    cout << foo.ok_or_else([](){
                cout << "foo else run" << endl;
                return str("kek");
            }) << endl;
    
    return 0;
}
