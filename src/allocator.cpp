#include "allocator.h"

namespace lnstl
{
    template <class T>
    inline T *_allocate()
    {
        std::set_new_handler(0);
        T *tmp = static_cast<T *>(::operator new(sizeof(T)));
        if (tmp == 0)
        {
            std::cerr << "Out of memory" << std::endl;
            exit(1);
        }
        return tmp;
    }

    template <class T>
    inline T *_allocate(ptrdiff_t n)
    {
        std::set_new_handler(0);
        T *tmp = static_cast<T *>(::operator new((size_t)(n * sizeof(T))));
        if (tmp == 0)
        {
            std::cerr << "Out of memory" << std::endl;
            exit(1);
        }
        return tmp;
    }

    

    template <class T>
    inline void _deallocate(T *ptr)
    {
        if (ptr == nullptr)
            return;
        ::operator delete(ptr);
    }

    template <class T1, class T2>
    inline void _construct(T1 *ptr)
    {
        new (ptr) T1(); //placement new
    }

    template <class T1, class T2>
    inline void _construct(T1 *ptr, const T2 &value)
    {
        new (ptr) T1(value); //placement new
    }

    template <class T>
    inline void _destroy(T *ptr)
    {
        if (ptr == nullptr)
            return;
        ptr->~T();
    }

    // template <class T>
    // typename allocator<T>::pointer allocator<T>::allocate()
    // {
    //     return _allocate();
    // }

    template <class T>
    typename allocator<T>::pointer allocator<T>::allocate(size_type n)
    {
        // return _allocate((difference_type)n);
        std::set_new_handler(0);
        T *tmp = static_cast<T *>(::operator new((size_t)(n * sizeof(T))));
        if (tmp == 0)
        {
            std::cerr << "Out of memory" << std::endl;
            exit(1);
        }
        return tmp;
    }

    template <class T>
    void allocator<T>::deallocate(allocator<T>::pointer ptr)
    {
        _deallocate(ptr);
    }

    template <class T>
    void allocator<T>::deallocate(allocator<T>::pointer ptr, size_type)
    {
        _deallocate(ptr);
    }

    template <class T>
    void allocator<T>::construct(allocator<T>::pointer ptr)
    {
        _construct(ptr);
    }

    template <class T>
    void allocator<T>::construct(allocator<T>::pointer ptr, const T &value)
    {
        _construct(ptr);
    }

    template <class T>
    void allocator<T>::destroy(allocator<T>::pointer ptr)
    {
        _destroy(ptr);
    }

    template <class T>
    typename allocator<T>::pointer allocator<T>::address(allocator<T>::reference ref)
    {
        return static_cast<pointer>(&ref);
    }

    template <class T>
    typename allocator<T>::const_pointer allocator<T>::const_address(allocator<T>::const_reference c_ref)
    {
        return static_cast<const_pointer>(&c_ref);
    }

    template <class T>
    typename allocator<T>::size_type allocator<T>::max_size() const
    {
        return size_type(UINT_MAX / sizeof(T));
    }

}
