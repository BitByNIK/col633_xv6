#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "param.h"
#include "fs.h"
#include "buf.h"
#include "pageswap.h"

struct swapslot swaptable[NSWAPSLOTS];
struct spinlock swaplock;

int nblocks_in_swap_slot = NSWAPBLOCKS / NSWAPSLOTS;

// Set up the swap table and initialize the swap slots to be free.
void swapinit(void)
{
    initlock(&swaplock, "swaplock");
    acquire(&swaplock);
    for (int i = 0; i < NSWAPSLOTS; i++)
        swaptable[i].is_free = 1;
    release(&swaplock);
}

int allocswap(void)
{
    acquire(&swaplock);
    for (int i = 0; i < NSWAPSLOTS; i++)
    {
        if (swaptable[i].is_free)
        {
            swaptable[i].is_free = 0;
            release(&swaplock);
            return i;
        }
    }
    release(&swaplock);
    return -1; // No free swap slots available
}

void freeswap(int slot)
{
    acquire(&swaplock);
    if (slot >= 0 && slot < NSWAPSLOTS)
        swaptable[slot].is_free = 1;
    release(&swaplock);
}

void writeswap(char *page, int slot)
{
    for (int i = 0; i < nblocks_in_swap_slot; i++)
    {
        struct buf *b = bread(ROOTDEV, 2 + slot * nblocks_in_swap_slot + i);
        memmove(b->data, page + i * 512, 512);
        bwrite(b);
        brelse(b);
    }
}

void readswap(char *page, int slot)
{
    for (int i = 0; i < nblocks_in_swap_slot; i++)
    {
        struct buf *b = bread(ROOTDEV, 2 + slot * nblocks_in_swap_slot + i);
        memmove(page + i * 512, b->data, 512);
        brelse(b);
    }
}
