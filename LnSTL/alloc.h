#ifndef _LN_STL_ALLOC_H_
#define _LN_STL_ALLOC_H_

#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream>
#define __THROW_BAD_ALLOC            \
    cerr << "out of memory" << endl; \
    exit(1)
#endif

// 第一级配置器
template <int inst>
class __malloc_alloc_template
{
private:
    // 以下函数用于处理内存不足的情况
    static void *oom_malloc(size_t);
    static void *oom_realloc(void *, size_t);
    static void (*__malloc_alloc_oom_handler)();

public:
    static void *allocate(size_t n)
    {
        void *result = malloc(n); //直接使用malloc分配空间
        if (result == 0)
            result = oom_malloc(n);
        return result;
    }

    static void deallocate(void *p, size_t /*n*/)
    {
        free(p);
    }

    static void *reallocate(void *p, size /*old_sz*/, size_t new_sz)
    {
        void *result = realloc(p, new_sz); //直接使用realloc分配空间
        if (result == 0)
            result = oom_realloc(p, new_sz);
        return result;
    }

    // 以下为模仿c++的set_new_handler(), 可自己设置
    static void (*set_alloc_oom_handler(void(*f)))()
    {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return old;
    }
};

template <int inst>
void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void *__malloc_alloc_template<inst>::oom_malloc(size_t)
{
    void (*my_malloc_handler)();
    void *result;

    while (1) //不断尝试
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (my_malloc_handler == 0)
            __THROW_BAD_ALLOC;
        (*my_malloc_handler)();
        result = malloc(n);
        if (result)
            return (result);
    }
}

template <int inst>
void *__malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
    void (*my_malloc_handler)();
    void *result;

    while (1) //不断尝试
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (my_malloc_handler == 0)
            __THROW_BAD_ALLOC;
        (*my_malloc_handler)();
        result = remalloc(n);
        if (result)
            return (result);
    }
}

typedef __malloc_alloc_template<0> malloc_alloc;

//第二级配置器
enum
{
    __ALIGN = 8 // 上调小内存区域的倍数
};

enum
{
    __MAX_BYTES = 128 // 小型区域的上限
};

enum
{
    __NFRESSLITS = __MAX_BYTES / __ALIGN //free lists的数目
};

template <bool threads, int inst>
class __default_alloc_template
{
private:
    //将bytes上调值8的倍数
    static size_t ROUND_UP(size_t bytes)
    {
        return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
    }
    union obj //freelists的节点构造
    {
        union obj *free_list_link;
        char client_data[1];
    };

    static obj *volatile free_list[__NFRESSLITS];

    //根据区块的大小，决定使用第几个freelist
    static size_t FREELIST_INDEX(size_t bytes)
    {
        return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
    }

    //返回一个大小为n的对象，情切有时候会为适当的freelist增加节点
    static void *refill(size_t n)
    {
        int nobjs = 20;

        //调用chunk_alloc(),尝试取得nobjs个区块作为free list的新节点
        //nobjs作为引用传递
        char *chunk = chunk_alloc(n, nobjs);

        // 如果只获得了一个区块，将其返回给调用者用，freelist没有其他的新节点
        if (nobjs == 1)
            return (chunk);

        obj *volatile *my_free_list;
        obj *result;
        obj *current_obj, *next_obj;
        int i;
        // 调整freelist， 纳入新节点
        my_free_list = free_list + FREELIST_INDEX(n);

        //以下在chunk空间内建立free list
        result = (obj *)chunk;
        // 以下引导freelist指向新配置的空间(取自内存池)
        *my_free_list = next_obj = (obj *)(chunk + n);
        //以下将free list的各节点串接起来
        for (i = 1;; ++i) //从第1个开始，因为第0个返回给调用者了
        {
            current_obj = next_obj;
            next_obj = (obj *)((char *)next_obj + n);
            if (nobjs - 1 == i)
            {
                current_obj->free_list_link = nullptr;
                break;
            }
            current_obj->free_list_link = next_obj;
        }
        return (result);
    }

    /**
     * @brief 配置一大块空间，可以容纳nobjs哥大小为size的区域，如果配置nojbs个区块有所不便，nojbs可能会降低
     * 
     * @param size 一个块的大小
     * @param nobjs 块的个数
     * @return char* 起始块的地址
     */
    static char *chunk_alloc(size_t size, int &nobjs)
    {
        char *result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;

        if (bytes_left >= total_bytes) // 内存池剩余空间完全满足需求量
        {
            result = start_free;
            start_free += total_bytes;
            return (result);
        }
        else if (bytes_left >= size) //内存池不能完全满足需求量，但是能够提供一个块的大小
        {
            nobjs = bytes_left / size;
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return (result);
        }
        else //内存池上剩余空间连一个块的大小都无法满足
        {
            // 原本的两倍大小，并且加上一个附加量ROUND_UP(heap_size >> 4)
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
            if (bytes_to_get > 0)
            {
                //内存池内还有一些剩余空间，分配给适当的free list
                //寻找合适的free lists
                obj *volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
                // 调整free list
                ((obj *)start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj *)start_free; // 有点搞不懂，待理解
            }

            // 配置heap空间，用来补充内存池
            start_free = (char *)malloc(bytes_to_get);
            if (start_free == 0) // heap空间不足，malloc失败
            {
                int i;
                obj *volatile *my_free_list, *p;
                // 不再寻找更小的区域，在free list中找到尚未用的区域，且区域足够大的freelist
                for (i = size; i <= __MAX_BYTES; i += __ALIGN)
                {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (p != 0) // free list内尚有未使用的区域
                    {
                        *my_free_list = p->free_list_link;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        //递归调用自己，为了修正nobjs
                        return (chunk_alloc(size, nobjs));
                    }
                }
                end_free = 0; // 到处都没有内存可用

                // 调用第一级配置器，看看oom机制能否成功找到空间
                // 抛出异常或者得到改善
                start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            }
            heap_size += bytes_to_get();
            end_free = start_free + bytes_to_get;
            //递归调用自己，为了修正nobjs
            return (chunk_alloc(size, nobjs));
        }
    }

    static char *start_free; //内存池起始位置
    static char *end_free;   //内存池结束位置
    static size_t heap_size;

public:
    static void *allocate(size_t n)
    {
        obj *volatile *my_free_list;
        obj *result;
        if (n > (size_t)__MAX_BYTES) // 大于128bytes就调用第一级配置器
            return (malloc_alloc::allocate(n));

        //寻找16哥free lists中适当的一个
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == 0)
        {
            // 没有找到可用的free lists，用refill重新填充
            void *r = refill(ROUND_UP(n));
            return r;
        }
        // 调整free lists
        *my_free_list = result->free_list_link;
        return (result);
    }

    static void deallocate(void *p, size_t n)
    {
        obj *q = (obj *)p;
        obj *volatile *my_free_list;

        if (n > __MAX_BYTES)
        {
            malloc_alloc::deallocate(p, n);
            return;
        }

        my_free_list = free_list + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
};

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0;

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = 0;

template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
__default_alloc_template<threads, inst>::obj *volatile __default_alloc_template<threads, inst>::free_list[__NFRESSLITS] =
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif
