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
    assert(res.is_ok());
    assert(res.unwrap() == 5. / 2);

    res = checked_div(5, 0);
    assert(res.is_err());
    cout << res << endl;

    return 0;
}
