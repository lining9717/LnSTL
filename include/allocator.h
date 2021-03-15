#ifndef _LN_STL_ALLOCTOR_H_
#define _LN_STL_ALLOCTOR_H_

#include <new>
#include <cstddef>
#include <climits>
#include <iostream>

namespace lnstl
{

    // template <class T>
    // inline T *_allocate();

    template <class T>
    inline T *_allocate(ptrdiff_t size);

    template <class T>
    inline void _deallocate(T *ptr);

    template <class T1, class T2>
    inline void _construct(T1 *ptr);

    template <class T1, class T2>
    inline void _construct(T1 *ptr, const T2 &value);

    template <class T>
    inline void _destroy(T *ptr);

    template <class T>
    class allocator
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
        // static pointer allocate()
        // {
        //     return _allocate();
        // }
        static pointer allocate(size_type n);

        static void deallocate(pointer ptr);
        static void deallocate(pointer ptr, size_type);

        static void construct(pointer ptr);
        static void construct(pointer ptr, const T &value);

        static void destroy(pointer ptr);

        pointer address(reference ref);
        const_pointer const_address(const_reference c_ref);
        size_type max_size() const;
    };
} // namespace lnst

#endif