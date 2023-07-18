#ifndef _OS_SHARED_MACROS_H
#define _OS_SHARED_MACROS_H

#ifndef align_up
#define align_up(num, align) \
    (((num) + ((align)-1)) & ~((align)-1))
#endif

#endif