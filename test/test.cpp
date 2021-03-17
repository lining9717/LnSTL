#include "allocator_test.h"
#include "alloc_test.h"

int main(int argc, char const *argv[])
{
    cout << "allocator test:" << endl;
    allocator_test();

    cout << "alloc test:" << endl;
    alloc_test();

    cout << "----------------------------------------------------" << endl;
    return 0;
}