/* This files manages blocks allocation and deallocation.
 *
 * The entry points into this file are:
 *   discard_preallocated_blocks:	Discard preallocated blocks.
 *   alloc_block:	somebody wants to allocate a block; find one.
 *   free_block:	indicate that a block is available for new allocation.
 *
 * Created:
 *   June 2010 (Evgeniy Ivanov)
 */

#include <fcntl.h>
#include "stdio.h"
#include "fs.h"
#include <string.h>
#include <stdlib.h>
#include <minix/com.h>
#include <minix/u64.h>
#include "buf.h"
#include "inode.h"
#include "super.h"
#include "const.h"
#include <MY_CONFIG.h>
  

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int stoi(char* a,int size){
    int sum=0;
    for (int i=0;i<size;i++){
        sum=sum*10+(a[i]-'0');
    }
    return sum;
}
struct extent *read_config_file()
{   
    struct extent *extents = NULL;
    int num_extents = 0;
    char a[10];
    int j=0,start_block,num_blocks;
    for(int i=0;MYCONFIG[i]!='\0';i++){
            
        if(MYCONFIG[i]!=' '&&MYCONFIG[i]!='_'){
            a[j++]=MYCONFIG[i];
            
        }else if(MYCONFIG[i]==' '){

            a[j]='\0';
            start_block=stoi(a,j);
            j=0;
        }else if (MYCONFIG[i]=='_'){
            
            num_extents++;
            a[j]='\0';
            num_blocks=stoi(a,j);
            j=0;
            extents = realloc(extents, sizeof(struct extent) * num_extents);
            extents[num_extents - 1].start_block = start_block;
            extents[num_extents - 1].num_blocks = num_blocks; 
        }
    }
    for(int i=0;i<num_extents;i++){
        extents[i].num_extents=num_extents;
    }
      return extents;
 
}

static block_t alloc_block_bit(struct super_block *sp, block_t origin,
	struct inode *rip, size_t extent_size);

/*===========================================================================*
 *                      discard_preallocated_blocks                          *
 *===========================================================================*/
void discard_preallocated_blocks(struct inode *rip)
{
/* When called for rip, discard (free) blocks preallocated for rip,
 * otherwise discard all preallocated blocks.
 * Normally it should be called in following situations:
 * 1. File is closed.
 * 2. File is truncated.
 * 3. Non-sequential write.
 * 4. inode is "unloaded" from the memory.
 * 5. No free blocks left (discard all preallocated blocks).
 */
  int i;

  if (rip) {
	rip->i_prealloc_count = rip->i_prealloc_index = 0;
	for (i = 0; i < EXT2_PREALLOC_BLOCKS; i++) {
		if (rip->i_prealloc_blocks[i] != NO_BLOCK) {
			free_block(rip->i_sp, rip->i_prealloc_blocks[i]);
			rip->i_prealloc_blocks[i] = NO_BLOCK;
		}
	}
	return;
  }

  /* Discard all allocated blocks.
   * Probably there are just few blocks on the disc, so forbid preallocation.*/
  for(rip = &inode[0]; rip < &inode[NR_INODES]; rip++) {
	rip->i_prealloc_count = rip->i_prealloc_index = 0;
	rip->i_preallocation = 0; /* forbid preallocation */
	for (i = 0; i < EXT2_PREALLOC_BLOCKS; i++) {
		if (rip->i_prealloc_blocks[i] != NO_BLOCK) {
			free_block(rip->i_sp, rip->i_prealloc_blocks[i]);
			rip->i_prealloc_blocks[i] = NO_BLOCK;
		}
	}
  }
}


/*===========================================================================*
 *                              alloc_block                                  *
 *===========================================================================*/
block_t alloc_block(struct inode *rip, block_t block, size_t extent_size)
{
  printf("ya mama");
  block_t goal;
  block_t b;
  struct super_block *sp = rip->i_sp;
  block_t start_block = NO_BLOCK;  // start block of the allocated extent
  size_t blocks_allocated = 0;  // number of blocks allocated in the extent

  if (sp->s_rd_only)
  {
    panic("can't alloc block on read-only filesys.");
  }

  // Check for free blocks. First time discard preallocation,
  // next time return NO_BLOCK
  if (!opt.use_reserved_blocks &&
      sp->s_free_blocks_count <= sp->s_r_blocks_count)
  {
    discard_preallocated_blocks(NULL);
  }
  else if (sp->s_free_blocks_count <= EXT2_PREALLOC_BLOCKS)
  {
    discard_preallocated_blocks(NULL);
  }

  if (!opt.use_reserved_blocks &&
      sp->s_free_blocks_count <= sp->s_r_blocks_count)
  {
    return (NO_BLOCK);
  }
  else if (sp->s_free_blocks_count == 0)
  {
    return (NO_BLOCK);
  }

  if (block != NO_BLOCK)
  {
    goal = block;
    if (rip->i_preallocation && rip->i_prealloc_count > 0)
    {
      // check if goal is preallocated
      b = rip->i_prealloc_blocks[rip->i_prealloc_index];
      if (block == b || (block + 1) == b)
      {
        // use preallocated block
        rip->i_prealloc_blocks[rip->i_prealloc_index] = NO_BLOCK;
        rip->i_prealloc_count--;
        rip->i_prealloc_index++;
        if (rip->i_prealloc_index >= EXT2_PREALLOC_BLOCKS)
        {
          rip->i_prealloc_index = 0;
          ASSERT(rip->i_prealloc_count == 0);
        }
        rip->i_bsearch = b;
        return b;
      }
      else
      {
       rip->i_preallocation = 0;
discard_preallocated_blocks(rip);
}
}
}
      else
    {
        int group = (rip->i_num - 1) / sp->s_inodes_per_group;
        goal = sp->s_blocks_per_group * group + sp->s_first_data_block;
    }

      if (rip->i_preallocation && rip->i_prealloc_count)
    {
        ext2_debug("There're preallocated blocks, but they're neither used or freed!");
    }
      printf("Block allocated not user extent \n");

      struct extent *extents = read_config_file();

  // Allocate a block from the block bitmap.
  block = alloc_block_bit(sp, goal, rip, extents[0].num_blocks);
  if (block == NO_BLOCK)
  {
      // No free block available.
      return NO_BLOCK;
  }

  // Update the search position in the block bitmap.
  rip->i_bsearch = block;

  // Check if the block we just allocated is within any of the user-defined extents.
  struct extent *curr = extents;

	for (int i = 0; i < extents[0].num_extents; i++) {
    if (block >= curr[i].start_block && block < (curr[i].start_block + curr[i].num_blocks)) {
        start_block = curr[i].start_block;
		blocks_allocated = curr[i].num_blocks;
        break;
    }
}

  if (blocks_allocated == 0)
  {
      // The block is not within a user-defined extent.
      // Allocate more blocks if needed to form an extent of the desired size.
      while (blocks_allocated < extent_size)
      {
          block = alloc_block_bit(sp, block + 1, rip, extents[0].num_blocks);
          if (block == NO_BLOCK)
          {
              // No more free blocks available.
              break;
          }
          blocks_allocated++;
      }
  }

  // Return the start block of the allocated extent.
  return start_block;

}
static void check_block_number(block_t block, struct super_block *sp,
	struct group_desc *gd);

/*===========================================================================*
 *                              alloc_block_bit                              *
 *===========================================================================*/
static block_t alloc_block_bit(struct super_block *sp, block_t goal, struct inode *rip, size_t extent_size)
{
  block_t block = NO_BLOCK;	/* allocated block */
  int group;
  char update_bsearch = FALSE;
  int i;
  int word;
  word = ((goal - sp->s_first_data_block) % sp->s_blocks_per_group) / FS_BITCHUNK_BITS;

  if (goal >= sp->s_blocks_count ||
      (goal < sp->s_first_data_block && goal != 0)) {
	goal = sp->s_bsearch;
  }

  if (goal <= sp->s_bsearch) {
	/* No reason to search in a place with no free blocks */
	goal = sp->s_bsearch;
	update_bsearch = TRUE;
  }
 printf("Bit allocated \n");
  /* Read the configuration file and parse the contents into a data structure. */
  struct extent *extents = read_config_file();

  /* Try to allocate block at any group starting from the goal's group.
   * First time goal's group is checked from the word=goal, after all
   * groups checked, it's checked again from word=0, that's why "i <=".
   */
  group = (goal - sp->s_first_data_block) / sp->s_blocks_per_group;
  for (i = 0; i <= sp->s_groups_count; i++, group++) {
	struct group_desc *gd;

	if (group >= sp->s_groups_count)
		group = 0;

	gd = get_group_desc(group);
	if (gd == NULL)
		panic("can't get group_desc to alloc block");

	if (gd->free_blocks_count == 0)
		continue;

    /* Iterate over the extents in the linked list and check if the group has a user-defined extent that can be used. */
    for (int i = 0; i < extents[0].num_extents; i++) {
  if (extents[i].num_blocks >= extent_size) {
    /* Allocate a block from the user-defined extent */
    block = extents[i].start_block;
    extents[i].start_block += extent_size;
    extents[i].num_blocks -= extent_size;
    gd->free_blocks_count -= extent_size;
    sp->s_free_blocks_count -= extent_size;
    return block;
  }
}
	/* Otherwise, try to allocate a block from the free block bitmap */
    struct buf *bp = get_block(sp->s_dev, gd->block_bitmap, NORMAL);
	int bit = setbit(b_bitmap(bp), sp->s_blocks_per_group, word);
	if (bit != -1) {
		block = bit + sp->s_first_data_block +
				group * sp->s_blocks_per_group;
		check_block_number(block, sp, gd);
		lmfs_markdirty(bp);
		put_block(bp, MAP_BLOCK);
		gd->free_blocks_count--;
		sp->s_free_blocks_count--;
		return block;
	}

	put_block(bp, MAP_BLOCK);
  }

  return NO_BLOCK;
  printf("Bit allocated from user-extent \n");
}
/*===========================================================================*
 *                        free_block	                                     *
 *===========================================================================*/
void free_block(struct super_block *sp, block_t block)
{
    /* Return a block by turning off its bitmap bit. */
    int group;          /* group number of block */
    struct group_desc *gd;

    if (sp->s_rd_only)
        panic("can't free bit on read-only filesys.");

    if (block >= sp->s_blocks_count ||
        block < sp->s_first_data_block)
        panic("trying to free block %d beyond blocks scope.", block);

    /* At first search group, to which block belongs to */
    group = (block - sp->s_first_data_block) / sp->s_blocks_per_group;

    gd = get_group_desc(group);
    if (gd == NULL)
        panic("can't get group_desc to alloc block");

    /* We might be buggy (No way! :P), so check if we deallocate
    * data block, but not control (system) block.
    * This should never happen.
    */
    if (block == gd->inode_bitmap || block == gd->block_bitmap
        || (block >= gd->inode_table
            && block < (gd->inode_table + sp->s_itb_per_group))) {
        ext2_debug("ext2: freeing non-data block %d\n", block);
        panic("trying to deallocate system/control block, hardly poke author.");
    }

    /* Read the extent configuration file */
    struct extent *extents = read_config_file();
    if (!extents) {
        /* No user-defined extents, use the block bitmap */
        struct buf *bp;
        bp = get_block(sp->s_dev, gd->block_bitmap, NORMAL);
        if (unsetbit(b_bitmap(bp), block))
            panic("Tried to free unused block", block);

        lmfs_markdirty(bp);
        put_block(bp, MAP_BLOCK);

        gd->free_blocks_count++;
        sp->s_free_blocks_count++;

        group_descriptors_dirty = 1;

        if (block < sp->s_bsearch)
            sp->s_bsearch = block;
    } else {
        /* User-defined extents, search through the array */
       for (int i = 0; i < extents[0].num_extents; i++) {
  if (block >= extents[i].start_block && block <= extents[i].start_block + extents[i].num_blocks - 1) {
    /* Block belongs to this extent, decrement the free block count */
    extents[i].num_blocks--;
    gd->free_blocks_count--;
    sp->s_free_blocks_count--;
    break;
  }
}
    }
}


static void check_block_number(block_t block, struct super_block *sp,
				struct group_desc *gd)
{

  /* Check if we allocated a data block, but not control (system) block.
   * Only major bug can cause us to allocate wrong block. If it happens,
   * we panic (and don't bloat filesystem's bitmap).
   */
  if (block == gd->inode_bitmap || block == gd->block_bitmap ||
      (block >= gd->inode_table
       && block < (gd->inode_table + sp->s_itb_per_group))) {
	ext2_debug("ext2: allocating non-data block %d\n", block);
	panic("ext2: block allocator tryed to return \
		system/control block, poke author.\n");
  }

  if (block >= sp->s_blocks_count) {
	panic("ext2: allocator returned blocknum greater, than total number of blocks.\n");
  }
}



