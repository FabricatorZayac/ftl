#include <cassert>
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

int main() {
    Result res = checked_div(5, 2);
    assert(res == Ok(2.5));
    assert(res.map([](double a){ return a * 4; }) == Ok(10));

    res = checked_div(5, 0);
    assert(res == Err(str("div by zero")));

    return 0;
}
