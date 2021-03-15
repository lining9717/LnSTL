#include "allocator.h"
#include "test.h"
using namespace std;

void allocator_test()
{
    vector<int, lnstl::allocator<int>> vec{0, 1, 2, 3, 4, 5};
    for (int i = 0; i < 6; ++i)
        cout << vec[i] << " ";
    cout << endl;
}
