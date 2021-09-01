
/*
    list_hacks.h -- Modifications to make list.h usuable in userspace

    Copyright (C) 2018, 2021 Ioannis Boutsikas

    All rights reserved.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __KERNEL__
#ifndef LIST_HACKS_H
#define LIST_HACKS_H
#include <stddef.h>
 /**
  * Casts a member of a structure out to the containing structure
  * @param ptr        the pointer to the member.
  * @param type       the type of the container struct this is embedded in.
  * @param member     the name of the member within the struct.
  *
  */
#define container_of(ptr, type, member) ({                      \
        const typeof(((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

# define POISON_POINTER_DELTA 0
  /*
   * These are non-NULL pointers that will result in page faults
   * under normal circumstances, used to verify that nobody uses
   * non-initialized list entries.
   */
#define LIST_POISON1  ((void *) 0x100 + POISON_POINTER_DELTA)
#define LIST_POISON2  ((void *) 0x200 + POISON_POINTER_DELTA)

   /*
    * WRITE_ONCE and READ_ONCE do NOT provide their Kernel utility
    * and are not in any way, shape or form equivalent to their kernel
    * implementations. Do NOT use this file with the linux kernel
    */
#define WRITE_ONCE(x, val) x = (val)

#define READ_ONCE(x) x

struct list_head {
    struct list_head *next, *prev;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};

struct hlist_head {
    struct hlist_node *first;
};

typedef _Bool  bool;

enum {
    false = 0,
    true = 1
};
#endif //LIST_HACKS_H
#endif // __KERNEL__
