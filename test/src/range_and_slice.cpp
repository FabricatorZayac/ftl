#include <iostream>
#include <cassert>

#include "ftl.hpp"

using namespace ftl;
using namespace std;

int main() {
    Slice a_slice{10, 15, 20, 25, 30};

    assert(a_slice == Slice({10, 15, 20, 25, 30}));
    assert(a_slice == ((int[]){10, 15, 20, 25, 30}));
    assert(a_slice[3] == 25);
    assert(a_slice[(Range{1, 4})] == (Slice{15, 20, 25}));
    assert((a_slice[{1, 4}]) == (Slice{15, 20, 25}));

    str foo = "foobar";
    assert(foo.len() == 6);

    cout << foo << endl;

    assert(foo == foo);
    assert(foo == str("foobar"));
    assert(foo == "foobar");
    assert(str("foobar", 1) == "f");

    return 0;
}
