#ifndef _PAGESWAP_H
#define _PAGESWAP_H

#define NSWAPSLOTS 800

struct swapslot
{
    int is_free;
    int page_perm;
};

extern struct swapslot swaptable[NSWAPSLOTS];
extern struct spinlock swaplock;

#endif