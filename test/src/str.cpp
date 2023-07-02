#include <cassert>
#include <iostream>

#include "ftl.hpp"

using namespace ftl;
using namespace std;

int main () {
    str foo = "foobar";

    cout << foo << endl;
    cout << str("foobar") << endl;

    assert(foo == foo);
    assert(foo == str("foobar"));
    assert(foo == "foobar");

    return 0;
}
