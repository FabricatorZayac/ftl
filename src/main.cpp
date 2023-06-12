#include <cassert>
#include <iostream>
#include "ftl.hpp"

using namespace ftl;

Result<double, const char *> checked_div(double a, double b) {
    if (b == 0) {
        return Err("div by zero");
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

int main () {
    auto res = checked_div(5, 2);
    std::cout << res << std::endl;

    res = checked_div(5, 0);
    std::cout << res << std::endl;

    auto error = fail();
    std::cout << error << std::endl;
    error = success();
    std::cout << error<< std::endl;

    return 0;
}
