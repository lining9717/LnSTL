#include "simple_allocator.h"
#include "test.h"
using namespace std;




void alloc_test()
{
    vector<int, lnstl::simple_allocator<int>> vec{0, 1, 2, 3, 4, 5};
    for (int i = 0; i < 6; ++i)
        cout << vec[i] << " ";
    cout << endl;
}
