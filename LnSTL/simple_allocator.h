#ifndef LN_STL_SIMPLE_ALLOCATOR_H_
#define LN_STL_SIMPLE_ALLOCATOR_H_

//此文件使用双层级配置器
#include "alloc.h"
// #define _USE_MALLOC

#ifdef _USE_MALLOC
typedef lnstl::__malloc_alloc_template<0> alloc;
#else
typedef lnstl::__default_alloc_template<false, 0> alloc;
#endif

namespace lnstl
{
    template <class T, class Alloc = alloc>
    class simple_allocator
    {
    public:
        typedef T value_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        static pointer allocate()
        {
            return static_cast<pointer>(Alloc::allocate(sizeof(T)));
        }
        static pointer allocate(size_type n)
        {
            return n == 0 ? 0 : static_cast<pointer>(Alloc::allocate(n * sizeof(T)));
        }
        static void deallocate(pointer ptr)
        {
            Alloc::deallocate(ptr, sizeof(T));
        }
        static void deallocate(pointer ptr, size_t n)
        {
            if (n != 0)
            {
                Alloc::deallocate(ptr, n * sizeof(T));
            }
        }

        static void construct(pointer ptr)
        {
            new (ptr) T(); //placement new
        }

        static void construct(pointer ptr, const T &value)
        {
            new (ptr) T(value);
        }

        static void destroy(pointer ptr)
        {
            ptr->~T();
        }

        static void destroy(T *first, T *last)
        {
            for (; first != last; ++first)
                first->~T();
        }
    };
}

#endif