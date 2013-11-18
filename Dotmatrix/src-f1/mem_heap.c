/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : mem_heap.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Heap memory driver
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "mem_heap.h"
#include "mutex.h"
#include "calltrace.h"
#include "common.h"


/* Private define ------------------------------------------------------------*/
#define MEMHEAP_PAD_CHAR      0xaa

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
void                    InitBlockHeader(mem_block_header_type *block_ptr, unsigned long size);
void*                   mem_calloc(mem_heap_type* pHeap, size_t elt_count, size_t elt_size);
void*                   mem_malloc(mem_heap_type* pHeap, size_t size);
void*                   mem_realloc(mem_heap_type* pHeap,void *ptr, size_t size);
void                    mem_free(mem_heap_type* pHeap,void *ptr);


mem_block_header_type*  GetNextBlock(mem_heap_type* pHeap,const mem_block_header_type *block_ptr);
mem_block_header_type*  FindFreeBlock(mem_heap_type* pHeap,unsigned long  size_needed);

unsigned long           GetBlockLogicalSize( const mem_block_header_type *block_hdr_ptr );

void                    block_iterator_init(mem_heap_block_iterator_type *block_iter_ptr);
void                    block_iterator_reset(mem_heap_block_iterator_type *block_iter_ptr);
int                     block_iterator_next(mem_heap_type* pHeap,mem_heap_block_iterator_type *block_iter_ptr, \
                                        mem_block_header_type *out_block_ptr, int fake_free_coalesced);


/*******************************************************************************
* Function Name  : MemHeap_Create
* Description    : Create Heap memory
* Input          : None.
* Return         : None.
*******************************************************************************/
void MemHeap_Create(mem_heap_type* pHeap, char* pHeapPool,int nHeapSize, char* Name)
{
  char *memory_end_ptr;
  char *memory_start_ptr;
  unsigned long chunks;


  strncpy(pHeap->heap_name, Name, MAX_HEAPNAME_SIZE);

  pHeap->pHeapMem  = pHeapPool;
  pHeap->nHeapSize = nHeapSize;

  memory_start_ptr = (char *)pHeapPool;
  memory_end_ptr   = memory_start_ptr + nHeapSize;

  while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  {
    ++memory_start_ptr;
  }

  chunks = (unsigned long) ((memory_end_ptr - memory_start_ptr) / kMinChunkSize);

  pHeap->first_block            = (mem_block_header_type *) memory_start_ptr;
  pHeap->next_block             = pHeap->first_block;

  InitBlockHeader(pHeap->first_block, chunks * kMinChunkSize);
  pHeap->first_block->last_flag = (char) kLastBlock;
  pHeap->total_blocks           = 1;
  pHeap->max_used               = 0;
  pHeap->max_request            = 0;
  pHeap->used_bytes             = 0;
  pHeap->total_bytes            = chunks * kMinChunkSize;
}


void InitBlockHeader( mem_block_header_type *block_ptr, unsigned long size )
{
  ASSERT(block_ptr);
  ASSERT(size >= sizeof(mem_block_header_type));

  block_ptr->free_flag   = (char) kBlockFree;
  block_ptr->last_flag   = 0;
  block_ptr->pad1        = MEMHEAP_PAD_CHAR;
  block_ptr->extra       = 0;
  block_ptr->forw_offset = size;
  return;
}

void* mem_calloc(mem_heap_type* pHeap, size_t elt_count, size_t elt_size)
{

  register size_t total_length = elt_count * elt_size;
  void *answer;

  answer = mem_malloc(pHeap, total_length);

  if( answer )
  {
    memset(answer, 0, total_length);
  }
  return answer;
}


void* mem_malloc(mem_heap_type* pHeap, size_t size)
{
  unsigned long chunks;
  unsigned long actualSize;
  unsigned char bonusBytes;
  mem_block_header_type *freeBlock;
  void *answer = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!size) return NULL;

  chunks = ((unsigned long) size + sizeof(mem_block_header_type) + kMinChunkSize - 1) / kMinChunkSize;
  actualSize = chunks * kMinChunkSize;
  bonusBytes = (unsigned char)(actualSize - size - sizeof(mem_block_header_type));

  Mutex_Lock();

  freeBlock = FindFreeBlock(pHeap,actualSize);
  if (freeBlock)
  {
    /* split the block (if necessary) and return the new block */
    ASSERT(freeBlock->forw_offset > 0);

    if (freeBlock->forw_offset > (actualSize+sizeof(mem_block_header_type)))
    {
      /* must split into two free blocks */
      mem_block_header_type *newBlock = (mem_block_header_type *)((char *) freeBlock + actualSize);
      InitBlockHeader(newBlock, freeBlock->forw_offset-actualSize);
      newBlock->last_flag = freeBlock->last_flag;
      freeBlock->forw_offset = actualSize;
      freeBlock->last_flag = 0;
      ++pHeap->total_blocks;
    }

    /*  set up next block to search for next allocation request */
    pHeap->next_block = GetNextBlock(pHeap, freeBlock);

    /* mark the block as used and return it */
    freeBlock->free_flag = kBlockUsed;
    freeBlock->extra = bonusBytes;
    pHeap->used_bytes += size;
    if (pHeap->used_bytes > pHeap->max_used)
    {
      pHeap->max_used = pHeap->used_bytes;
    }
    if (size > pHeap->max_request)
    {
      pHeap->max_request = size;
    }
    answer = (char *) freeBlock + sizeof(mem_block_header_type);
  }

  Mutex_UnLock();

  return answer;
}


void mem_free(mem_heap_type* pHeap, void *ptr)
{
  mem_block_header_type *theBlock;

  ASSERT(ptr != NULL);

  /* free the passed in block */

  /* NULL is a NOP */
  if (!ptr)
  {
    return;
  }

  ASSERT(pHeap->first_block);
  theBlock = (mem_block_header_type*)((char*)ptr-sizeof(mem_block_header_type));

  ASSERT(theBlock>=pHeap->first_block && (char*)ptr<(char*)pHeap->first_block+pHeap->total_bytes);

  if (theBlock<pHeap->first_block || (char*)ptr>=(char*)pHeap->first_block+pHeap->total_bytes)
  {
    return; /* Outside heap memory is NOP */
  }

  /* Try to detect corruption. */
  ASSERT(theBlock->pad1 == MEMHEAP_PAD_CHAR);
  ASSERT(!theBlock->free_flag);  /* Attempt to detect multiple frees of same block */
  /* Make sure forw_offset is reasonable */
  ASSERT(theBlock->forw_offset >= sizeof(mem_block_header_type));
  /* Make sure extra is reasonable */
  ASSERT(theBlock->extra < kMinChunkSize);

  Mutex_Lock();

  if (!theBlock->free_flag) /* Be intelligent about not munging the heap if a multiple free of the same block is detected */
  {
    mem_block_header_type *next_block;

    ASSERT((theBlock->forw_offset-sizeof(mem_block_header_type)-theBlock->extra)<=pHeap->used_bytes);
    pHeap->used_bytes -= theBlock->forw_offset - sizeof(mem_block_header_type)-theBlock->extra;
    theBlock->free_flag = (char) kBlockFree;

    /* now backup the next pointer if applicable */
    next_block = GetNextBlock(pHeap, theBlock);
    if (next_block == pHeap->next_block)
    {
      /* Backup now to lessen possible fragmentation */
      pHeap->next_block = theBlock;
    }

    /* reset heap to initial state if everything is now freed */
    if (!pHeap->used_bytes)
    {
      /* reset heap now, but retain statistics */
      pHeap->next_block = pHeap->first_block;
      InitBlockHeader(pHeap->first_block, pHeap->total_bytes);
      pHeap->first_block->last_flag = (char) kLastBlock;
      pHeap->total_blocks = 1;
    }
  }

  Mutex_UnLock();
} /* END mem_free */

void* mem_realloc(mem_heap_type* pHeap, void *ptr, size_t size)
{
  size_t oldSize;
  unsigned long chunks;
  unsigned long actualSize;
  unsigned char bonusBytes;
  long sizeChange;
  mem_block_header_type *theBlock;
  mem_block_header_type *followingBlock;

  if (!ptr)
  {
    return mem_malloc(pHeap, size);
  }

  if (!size)
  {
    mem_free(pHeap, ptr);
    return NULL;
  }

  ASSERT(pHeap->first_block);
  theBlock = (mem_block_header_type*)((char*)ptr-sizeof(mem_block_header_type));

  ASSERT(theBlock>=pHeap->first_block && (char*)ptr<(char*)pHeap->first_block+pHeap->total_bytes);

  if (theBlock<pHeap->first_block ||(char*)ptr>=(char*)pHeap->first_block+pHeap->total_bytes)
  {
    return NULL; /* Outside heap memory is NOP */
  }

  /* Attempt to detect a free block passed in */
  ASSERT(!theBlock->free_flag);

  if (theBlock->free_flag)
  {
    return NULL; /* Behave sensibly then */
  }

  oldSize = (size_t) GetBlockLogicalSize(theBlock);
  if (oldSize == size)
  {
    return ptr; /* No size change, return same pointer */
  }

  sizeChange = (long) size - (long) oldSize;
  chunks = ((unsigned long)size+sizeof(mem_block_header_type)+kMinChunkSize-1)/kMinChunkSize;
  actualSize = chunks * kMinChunkSize;
  bonusBytes = (unsigned char)(actualSize-size-sizeof(mem_block_header_type));

  if (theBlock->forw_offset < actualSize)
  {
    void *answer = NULL;
    mem_block_header_type *newBlock, *nextFollowingBlock;
    unsigned long sizeNeeded = actualSize - theBlock->forw_offset;

    /* Begin by attempting (1) */
    Mutex_Lock();

    /* This code looks a lot like FindFreeBlock, but it's not exactly the same */
    followingBlock = GetNextBlock(pHeap,theBlock);
    if (followingBlock > theBlock && followingBlock->free_flag)
    {
      nextFollowingBlock = GetNextBlock(pHeap, followingBlock);
      while (followingBlock->forw_offset < sizeNeeded
      && nextFollowingBlock->free_flag
      && nextFollowingBlock > followingBlock)
      {
        /* collapse adjacent free blocks into one if it
             * will allow us to satisfy the request
             */

        unsigned char fixupNext = pHeap->next_block == nextFollowingBlock;
        followingBlock->forw_offset += nextFollowingBlock->forw_offset;
        followingBlock->last_flag = nextFollowingBlock->last_flag;
        --pHeap->total_blocks;
        nextFollowingBlock = GetNextBlock(pHeap, followingBlock);

        if (fixupNext)
        {
          pHeap->next_block = nextFollowingBlock;
        }
      }

      if (followingBlock->forw_offset >= sizeNeeded)
      {
        unsigned char fixupNext;

        if (followingBlock->forw_offset > sizeNeeded)
        {
          /* must split free block into two free blocks */

          nextFollowingBlock = GetNextBlock(pHeap,followingBlock);
          newBlock = (mem_block_header_type *)
          ((char *) followingBlock + sizeNeeded);
          InitBlockHeader(newBlock,
          followingBlock->forw_offset - sizeNeeded);
          newBlock->last_flag = followingBlock->last_flag;
          followingBlock->forw_offset = sizeNeeded;
          followingBlock->last_flag = 0;
          ++pHeap->total_blocks;

          if (pHeap->next_block == nextFollowingBlock)
          {
            /* Backup our next fit searching pointer */
            pHeap->next_block = newBlock;
          }
        }

        /* Now join the correct size free block onto the end of theBlock */
        theBlock->last_flag = followingBlock->last_flag;
        theBlock->forw_offset += followingBlock->forw_offset;
        --pHeap->total_blocks;
        pHeap->used_bytes = (unsigned long)((long) pHeap->used_bytes + sizeChange);
        theBlock->extra = bonusBytes;
        fixupNext = (pHeap->next_block == followingBlock);
        if (pHeap->used_bytes > pHeap->max_used)
        {
          pHeap->max_used = pHeap->used_bytes;
        }
        if (size > pHeap->max_request)
        {
          pHeap->max_request = size;
        }
        followingBlock = GetNextBlock(pHeap, theBlock);

        if (fixupNext)
        {
          pHeap->next_block = followingBlock;
        }
        ASSERT(theBlock->forw_offset == actualSize);
        answer = ptr; /* Trigger return when out of critical section */
      }
    }

    Mutex_UnLock();

    if (answer)
    {
      return ptr; /* (1) succeeded, return now */
    }

    /* Must use method (2) */

    answer = mem_malloc(pHeap, size);
    if (!answer)
    {
      return NULL; /* No room return NULL, orig. ptr unchanged */
    }

    memcpy(answer, ptr, oldSize);
    mem_free(pHeap, ptr);
    return answer; /* Return new block */
  }

  Mutex_Lock();

  if (theBlock->forw_offset > actualSize)
  {
      /* We must shrink the block down in size which
      means splitting it into two and marking the
      new block as free */
    mem_block_header_type *newBlock;

    followingBlock = GetNextBlock(pHeap, theBlock);
    newBlock = (mem_block_header_type *) ((char *) theBlock + actualSize);
    InitBlockHeader(newBlock, theBlock->forw_offset - actualSize);
    newBlock->last_flag = theBlock->last_flag;
    theBlock->forw_offset = actualSize;
    theBlock->last_flag = 0;
    ++pHeap->total_blocks;

    if (pHeap->next_block == followingBlock)
    {
      /* Backup our next fit searching pointer */
      pHeap->next_block = newBlock;
    }
  }

  pHeap->used_bytes = (unsigned long)((long)pHeap->used_bytes+sizeChange);
  theBlock->extra = bonusBytes;
  if (pHeap->used_bytes > pHeap->max_used)
  {
    pHeap->max_used = pHeap->used_bytes;
  }
  if (size > pHeap->max_request)
  {
    pHeap->max_request = size;
  }

  Mutex_UnLock();

  return ptr;
} /* END mem_realloc */

size_t GetHeapSize(mem_heap_type* pHeap)
{
  return pHeap->nHeapSize;
}

size_t GetUsed(mem_heap_type* pHeap)
{
  size_t used = 0;
  used = pHeap->used_bytes;
  return used;
}

size_t GetOverhead ()
{
  return sizeof (mem_block_header_type);
}

size_t GetConsumed(mem_heap_type* pHeap)
{
  size_t consumed_cnt = 0;
  consumed_cnt = pHeap->used_bytes;
  consumed_cnt += pHeap->total_blocks * sizeof (mem_block_header_type);
  return consumed_cnt;
}

mem_block_header_type* GetNextBlock(mem_heap_type* pHeap, const mem_block_header_type *block_ptr)
{
  ASSERT(block_ptr->pad1 == MEMHEAP_PAD_CHAR);
  return block_ptr->last_flag  ? pHeap->first_block
         : (mem_block_header_type *) ((char *) block_ptr + block_ptr->forw_offset);
}


mem_block_header_type* FindFreeBlock(mem_heap_type* pHeap, unsigned long  size_needed)
{
  long searchBlocks;
  mem_block_header_type *followingBlock;
  /* a loop variable used to walk through the blocks of the heap */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(pHeap->first_block);

  searchBlocks = (long) pHeap->total_blocks;

  followingBlock = GetNextBlock(pHeap, pHeap->next_block);

  for (; searchBlocks > 0; --searchBlocks, pHeap->next_block=followingBlock,
                  followingBlock=GetNextBlock(pHeap,pHeap->next_block))
  {
    ASSERT(pHeap->next_block->forw_offset > 0);
    ASSERT(followingBlock->forw_offset > 0);

    if (pHeap->next_block->free_flag)
    {
      while (pHeap->next_block->forw_offset < size_needed
             && followingBlock->free_flag && followingBlock > pHeap->next_block)
      {
        /* collapse adjacent free blocks into one if it
              * will allow us to satisfy a request
              */
        pHeap->next_block->forw_offset += followingBlock->forw_offset;
        pHeap->next_block->last_flag = followingBlock->last_flag;
        --pHeap->total_blocks;
        --searchBlocks; /* don't search more than we need */
        followingBlock = GetNextBlock(pHeap, pHeap->next_block);
      }
      if (pHeap->next_block->forw_offset >= size_needed)
      {
        return pHeap->next_block;
      }
    }
  }
  return 0;       /* didn't find anything */
} /* END FindFreeBlock */

unsigned long GetBlockLogicalSize(const mem_block_header_type *block_hdr_ptr)
{
  return block_hdr_ptr->forw_offset-sizeof(mem_block_header_type)-(block_hdr_ptr->free_flag?0:block_hdr_ptr->extra);
}

void GetTotals(mem_heap_type* pHeap, mem_heap_totals_type *totals)
{
  mem_heap_block_iterator_type heapIter;
  /* heap iterator used to collect heap statistics */

  mem_block_header_type theBlock;
  /* holds info returned by heap iterator */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(totals);
  Mutex_Lock();

  memset(totals, 0, sizeof(mem_heap_totals_type));
  block_iterator_init(&heapIter);
  totals->max_logical_used = pHeap->max_used;
  totals->max_logical_request = pHeap->max_request;
  while (block_iterator_next(pHeap,&heapIter, &theBlock, 1))
  {
    unsigned long blkSize = GetBlockLogicalSize(&theBlock);
    ++totals->current_block_count;
    totals->header_bytes += sizeof(mem_block_header_type);
    if (theBlock.free_flag)
    {
      totals->free_bytes += blkSize;
      if (blkSize > totals->largest_free_block)
      {
        totals->largest_free_block = blkSize;
      }
    }
    else
    {
      ++totals->used_blocks;
      totals->used_bytes += blkSize;
      totals->wasted_bytes += theBlock.extra;
    }
  }
  totals->total_physical_bytes = totals->free_bytes+totals->used_bytes+totals->wasted_bytes+totals->header_bytes;
  ASSERT(totals->used_bytes == pHeap->used_bytes);
  ASSERT(totals->current_block_count >= 1);
  ASSERT(totals->current_block_count <= pHeap->total_blocks);
  ASSERT(totals->total_physical_bytes == pHeap->total_bytes);

  Mutex_UnLock();

} /* mem_heap_get_totals */


void GetFreeblockInfo(mem_heap_type* pHeap,unsigned int num_blocks, unsigned int *buf)
{
  mem_heap_block_iterator_type heap_iter;
  mem_block_header_type the_block;
  unsigned int blk_size;
  unsigned int min_blk_size;
  unsigned int min_blk_index;
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT( num_blocks >= 1 );

  Mutex_Lock();

  memset( buf, 0, num_blocks * sizeof( int ) );
  block_iterator_init(&heap_iter);

  while ( block_iterator_next(pHeap, &heap_iter, &the_block, 1 ) )
  {
    if ( the_block.free_flag )
    {
      blk_size = GetBlockLogicalSize( &the_block );
      min_blk_size  = buf[0];
      min_blk_index = 0;
      for ( i = 1; i < num_blocks; i++ )
      {
        if ( min_blk_size > buf[i] )
        {
          min_blk_size  = buf[i];
          min_blk_index = i;
        }
      }
      if ( blk_size > min_blk_size )
      {
        buf[min_blk_index] = blk_size;
      }
    }
  }
  Mutex_UnLock();
} /* mem_heap_get_freeblock_info */


unsigned int GetChunkSize()
{
  return (unsigned int) kMinChunkSize;
} /* END mem_heap_get_chunk_size */

void block_iterator_init(mem_heap_block_iterator_type *block_iter_ptr)
{
  ASSERT(block_iter_ptr);

  block_iterator_reset(block_iter_ptr);
}

void block_iterator_reset(mem_heap_block_iterator_type *block_iter_ptr)
{
  block_iter_ptr->mBlock = NULL;
}

int block_iterator_next(mem_heap_type* pHeap, mem_heap_block_iterator_type *block_iter_ptr, mem_block_header_type *out_block_ptr, int fake_free_coalesced)
{
  int ans;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(block_iter_ptr);
  ASSERT(out_block_ptr);

  ans = 0;
  if (!block_iter_ptr->mBlock)
  {
    block_iter_ptr->mBlock = pHeap->first_block;
    if (block_iter_ptr->mBlock) {
      ans = 1;
    }
  }
  else if (!block_iter_ptr->mBlock->last_flag)
  {
    block_iter_ptr->mBlock = GetNextBlock(pHeap, block_iter_ptr->mBlock);
    ans = 1;
  }

  memset(out_block_ptr, 0, sizeof(mem_block_header_type));

  if (ans)
  {
    *out_block_ptr = *block_iter_ptr->mBlock;
    if (fake_free_coalesced && out_block_ptr->free_flag)
    {
      /* Pretend as though adjacent free blocks were coalesced */
      for (;;)
      {
        mem_block_header_type *next_block;
        if (block_iter_ptr->mBlock->last_flag) break;
        next_block = GetNextBlock(pHeap, block_iter_ptr->mBlock);
        if (!next_block->free_flag) break;
        block_iter_ptr->mBlock = next_block;
        out_block_ptr->forw_offset += block_iter_ptr->mBlock->forw_offset;
      }
    }
  }
  return ans;
} /* block_iterator_next */


void* Malloc(mem_heap_type* pHeap, size_t size)
{
  return mem_malloc(pHeap,size);
}

void Free(mem_heap_type* pHeap, void *ptr)
{
  mem_free(pHeap, ptr);
}

void MemHeap_PrintStatus(mem_heap_type* pHeap)
{
  mem_heap_totals_type totals;
  GetTotals(pHeap, &totals);

  //CallTrace(TRACE_APP_WARN, "%s : Peak(%d) Current(%d/%d)\r\n", pHeap->heap_name, totals.max_logical_used, totals.used_bytes, totals.total_physical_bytes);
}

