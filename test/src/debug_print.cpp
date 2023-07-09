#include "ftl.hpp"
#include <vector>

using namespace std;
using namespace ftl;

int main () {
    cout << debug << vector{4, 3, 2, 69} << endl
         << debug << 5 << endl
         << debug << 5.2 << endl
         << debug << true << endl
         << debug << "kekw" << endl
         << debug << str("foobar") << endl
         << debug << Slice{5, 69, 420} << endl
         << debug << (double[]){5.5, 6.9} << endl
         << debug << Some(5) << endl
         << debug << None() << endl
         << debug << Ok("stuff") << endl
         << debug << Err("error") << endl
         << debug << Some("stuff that is referred to").as_ref() << endl;

    return 0;
}
