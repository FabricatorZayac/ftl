#include <cassert>
#include <iostream>

#include "ftl.hpp"

using namespace ftl;
using namespace std;

int main () {
    str foo = "foobar";
    assert(foo.len() == 6);

    cout << foo << endl;
    cout << str("foobar") << endl;

    assert(foo == foo);
    assert(foo == str("foobar"));
    assert(foo == "foobar");

    return 0;
}
