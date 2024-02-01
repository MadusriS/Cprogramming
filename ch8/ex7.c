#include <stdio.h>
#include <limits.h>

/*
    Exercise 8-7. malloc accepts a size request without checking its plausibility; free believes that the block it is asked to free contains a valid size field. Improve these
    routines so they take more pains with error checking.
*/

#define NALLOC 1024 // minimum number of Header units to request in malloc/morecore. A larger number like 1024 reduces the number of expensive syscalls needed by mymalloc
typedef long Align; // for alignment to long boundary

union header // block header
{
    struct
    {
        union header *ptr; // next block if on free list
        unsigned size; // size of this block, in Header units
    } s;
    Align x; // force alignment of blocks in memory
};
typedef union header Header;

static Header base; // empty list of free memory to get started. starts out as the left-most end of the list, but not guaranteed to always be the left-most end
static Header *freep = NULL; // start of free list. keeps track of the last block that had space to allocate memory or the next block after

void *mycalloc(unsigned n, size_t size);
void *mymalloc(unsigned nbytes);
static Header *morecore(unsigned nunits);
void myfree(void *ap);
char *sbrk(int); // syscall for more memory. Apparently you don't need to include any headers, just define the function and call it

int main()
{
    char *b = mymalloc(0);
    myfree(b);
    myfree(&base);
    myfree(freep);
    return 0;
}

// gets n times size number of bytes of memory from mymalloc and initializes it to zero
void *mycalloc(unsigned n, size_t size)
{
    char *p;
    if ((p = mymalloc(n * size)) == NULL) // call mymalloc to get memory. If NULL, failed to allocate memory
        return NULL;
    for (unsigned i = 0; i < n * size; i++) // go through bytes and set each one to zero
        p[i] = '\0';
    return p;
}

/*
    General-purpose storage allocator. Assumes that pointers to different blocks returned by sbrk can be meaningfully compared. This is not guaranteed by the standard, which
    permits pointer comparisons only within an array. So this version of malloc is portable only among machines for which general pointer comparison is meaningful. Returns a
    pointer to the memory location that contains nbytes of free memory. All memory chunks are prefixed with a Header for memory management.
*/
/*UINT_MAX is a constant defined in the C programming language representing the maximum value that can be held by an object of type unsigned int. It is defined in the <limits.h> header.*/
void *mymalloc(unsigned nbytes)
{
    Header *p, *prevp;
    if (nbytes == 0 || nbytes > UINT_MAX - sizeof(Header))
    {
        printf("Error: invalid nbytes size request %d\n", nbytes);
        return NULL;
    }
    /*
        The nunits calculation is perfect for its purpose since it figures out the least number of units necessary to allocate nbytes. If the request is for zero bytes, it
        will only need 1 unit for the header. If it is for 1 to sizeof(Header) bytes, it will equal 2 since it needs 1 unit for the Header and 1 for the memory request. If
        the request is x * sizeof(Header) + 1 to (x + 1) * sizeof(Header) number of bytes, it will need x + 2 units of memory, x + 1 to fit the request, and 1 for the header.
        The reason for nunits including the Header is to ensure that the Header is preserved for all memory requests.
    */
    unsigned nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    // This check if freep is NULL is only to initialize freep, prevp, and the base parameters. This could have been done elsewhere, but it does make the function independent
    /*union header // block header
{
    struct
    {
        union header *ptr; // next block if on free list
        unsigned size;     // size of this block, in Header units
    } s;
    Align x; // force alignment of blocks in memory
};
typedef union header Header;
*/
   if ((prevp = freep) == NULL)
    {
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    /* 
        The for loop initializes p to equal what prevp points to. Since prevp was just set to freep, it is where the last block of memory was found. This is the next Header in
        the circularly linked list structure. If the for loop iterates, it updates prevp to equal what p is and moves p to the next free block. This moves both prevp and p
        around the list structure that keeps track of the free memory available to allocate. Note: changes to p affect prevp->s.ptr as well since they point to the same thing
    */
    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr)
    {
        if (p->s.size >= nunits) // p is big enough to fit the size request
        {
            if (p->s.size == nunits) 
                prevp->s.ptr = p->s.ptr; // exact fit, update prevp to point to the next Header block since p has no free units. p's memory is returned below
            else // if the Header block found contains more units than the request, the p header moves to the tail end of its space leaving just enough for the request.
            {
                p->s.size -= nunits; // subtract number of units left in p and prevp-s.ptr by the number being allocated
                p += p->s.size; // move pointer to tail end of free memory with exact free space for request. This creates new a Header and doesn't move prevp->s.ptr Header
                p->s.size = nunits; // update new size for the new Header
            }
            /*
                update freep to be at the last block found that had memory available. If memory allocation request used up all of the available units prevp->s.ptr had, then
                prevp points to the next Header in the list. Otherwise, prevp will point to the same location as before, with fewer units now.
            */
            freep = prevp;
            return (void *)(p + 1); // the p + 1 moves the pointer 1 unit to the right before returning. This preserves the header in memory, a requirement for myfree
        }
        if (p == freep) // wrapped around free list meaning the free space left in the Headers is not big enough for request. So, allocate more space
            if ((p = morecore(nunits)) == NULL) // update p to next free block returned by morecore. If it failed to allocate space, then return NULL
                return NULL;
    }
}

// ask system for more memory. nunits must be large enough for the memory request and a Header
static Header *morecore(unsigned nunits)
{
    char *cp; // used to point to the free memory returned from sbrk syscall
    Header *up; // uses memory from sbrk to store Header at the beginning and keeps track of the amount space it has in Headers units
    if (nunits < NALLOC) // if requested memory is less than the minimum number of units to allocate, request the minimum. This reduces the number of expensive syscalls to OS
        nunits = NALLOC;
    cp = sbrk(nunits * sizeof(Header)); // get enough bytes of memory to fit the number of requested units
    if (cp == (char *) -1) // sbrk returns (void *) -1 if it failed to allocate memory
        return NULL;
    up = (Header*) cp; // the memory is managed by a Header, so cast it from cp
    up->s.size = nunits; // store the amount of memory it has in units. mymalloc's nunits takes into account the memory needed for the Header and the memory request.
    myfree((void *) (up + 1)); // calling myfree with the allocated memory points freep to the new memory. The up + 1 is because myfree expects the header to be at ap - 1
    return freep; // return the Header that points to the new memory
}

/*
   Puts allocated block of memory in the free list. This could be new memory from morecore or old memory already allocated by myalloc. It scans the free list, starting at freep,
   looking for the place to insert the free block. This is either between two existing blocks or at one end of the list. In any case, if the block being freed is adjacent to
   either neighbor, the adjacent blocks are combined. If memory is passed that wasn't allocated from mymalloc, segmentation faults will likely occur because myfree relies on the
   block header being at -1 units from ap in memory.
*/
void myfree(void *ap)
{
    Header *bp, *p;
    if (ap == NULL)
    {
        printf("Error: null pointer passed to myfree\n");
        return;
    }
    bp = (Header *) ap - 1; // points bp to the Header in the block of memory
    if (bp->s.size == 0)
    {
        printf("Error: can't free ap with 0 units\n");
        return;
    }
    /*
        This for loop assigns p to freep which points to last block that contained memory. The conditional in the for loop is to check to see if it is at an ideal place to insert
        the memory. It does this by comparing the memory address of the Header bp and the memory address of the Header p. If bp is at a higher memory address than p, then it
        checks to see if the next Header in p is greater than bp. If this is true, that means the current memory being freed is between the start of p and p->s.ptr, the next
        Header, and this causes the loop to exit due to the not operator. Finally, after each loop iteration, p is moved to the next Header in the circularly linked list.
    */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        /*
            This checks to see if p is pointed to itself (which is the case after the linked list is initialized) or if it is greater than its next pointer (which is when p is
            at the end of the list and p->s.ptr is pointing to the left end of the list). When this is true, it checks to see if the block pointer has a higher memory address
            than p. This means bp is after the right end of the list. If bp it is less than p->s.ptr, then bp is before the left end of the list. In either case, exit the loop
            since p is at the appropriate location for the code that follows and bp is either before the start of the list of after it.
        */
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break;
    
    /*
        since bp->s.size includes the Header, bp + bp->s.size is one unit past what bp has space for. If one unit past bp is p->s.ptr, join the two memory chunks together. Don't
        allow the merge of headers if the size could cause an integer overflow. Instead, just update bp's pointer to p's pointer
    */
    if (bp + bp->s.size == p->s.ptr && bp->s.size < UINT_MAX - p->s.ptr->s.size)
    {
        bp->s.size += p->s.ptr->s.size; // add the total space taken up by p->s.ptr (to include the Header) to bp.
        bp->s.ptr = p->s.ptr->s.ptr; // change the pointer to point to what p->s.ptr points to. This effectively deletes p->s.ptr and bp now contains all of its memory
    }
    else
        /*
            If bp doesn't align with what p points too, then bp does not end right before p->s.ptr begins. bp could still be in-between p and p->s.ptr though since mymalloc
            removes Headers from the circularly linked list when all of its units are used up. If that is the case, bp will now point to what p points too, the next Header in
            the list. If p is at the end of the list or p points to itself, bp will point to the left end Header (still the next Header in the list).
        */
        bp->s.ptr = p->s.ptr;

    // this does the same thing as the previous if statement above, just now it is joining p to bp if p ends right before bp begins and the new size would be less than UINT_MAX.
    if (p + p->s.size == bp && p->s.size < UINT_MAX - bp->s.size)
    {
        p->s.size += bp->s.size; // p will absorb bp, so p increases its size of how many units bp has (to include the Header)
        p->s.ptr = bp->s.ptr; // this updates p->s.ptr to point to what bp pointed too. This effectively deletes bp and p now contains all of its memory
    }
    else
        /*
            If p doesn't align with bp, then p does not end right before b begins. bp could still be in-between p and p->s.ptr though since mymalloc removes Headers from the
            circularly linked list when all of its units are used up. If bp is outside of the bounds of the list, p will be at one of the ends of the list and bp will either
            be the new left end of the list or the new right end of the list.
        */
        p->s.ptr = bp;
    freep = p; // now that p is connected to the freed memory (either by absorption or pointing to now free memory), update freep with it so future mymalloc calls start at it
}
