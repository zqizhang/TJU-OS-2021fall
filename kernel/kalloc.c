// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

char *kmem_lock_name[] = {
  "kmem_cpu0", "kmem_cpu1", "kmem_cpu2", "kmem_cpu3", "kmem_cpu4", "kmem_cpu5", "kmem_cpu6", "kmem_cpu7"
};// names of kmems

void
kinit()
{
  int i;
  for(i = 0; i < NCPU; ++i){
    initlock(&kmem[i].lock, kmem_lock_name[i]);
  }
  // initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  push_off();
  int cpu = cpuid();
  pop_off();

  acquire(&kmem[cpu].lock);
  r->next = kmem[cpu].freelist;
  kmem[cpu].freelist = r;
  release(&kmem[cpu].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();
  int cpu = cpuid();
  pop_off();

  acquire(&kmem[cpu].lock);
  r = kmem[cpu].freelist;

// steal memory from other CPU
  if(!r){
    int steal_page = 32;//steal 32 pages once
    int i = 0;
    for(i = 0; i < NCPU; ++i){
      if(i == cpu)
        continue;
      acquire(&kmem[i].lock);
      struct run *r_ = kmem[i].freelist;

      // steal in freelist
      while(r_ && steal_page){
        kmem[i].freelist = r_->next;
        r_->next = r;
        r = r_;
        r_ = kmem[i].freelist;
        steal_page--;
      }

      release(&kmem[i].lock);
      if(steal_page == 0)
        break;
    }
  }

  if(r)
    kmem[cpu].freelist = r->next;
  release(&kmem[cpu].lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
