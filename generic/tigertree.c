/* (PD) 2003 The Bitzi Corporation
 *
 * Copyright (C) 2001 Bitzi (aka Bitcollider) Inc. & Gordon Mohr
 * Released into the public domain by same; permission is explicitly
 * granted to copy, modify, and use freely.
 *
 * THE WORK IS PROVIDED "AS IS," AND COMES WITH ABSOLUTELY NO WARRANTY,
 * EXPRESS OR IMPLIED, TO THE EXTENT PERMITTED BY APPLICABLE LAW,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Please see file COPYING or http://bitzi.com/publicdomain 
 * for more info.
 *
 * tigertree.c - Implementation of the TigerTree algorithm
 *
 * Patterned after sha.c by A.M. Kuchling and others.
 *
 * To use:
 *    (1) allocate a TT_CONTEXT in your own code;
 *    (2) tt_init(ttctx);
 *    (3) tt_update(ttctx, buffer, length); as many times as necessary
 *    (4) tt_digest(ttctx,resultptr);
 *
 * NOTE: The TigerTree hash value cannot be calculated using a
 * constant amount of memory; rather, the memory required grows
 * with the (binary log of the) size of input. (Roughly, one more 
 * interim value must be remembered for each doubling of the 
 * input size.) This code reserves a counter and stack for input 
 * up to about 2^72 bytes in length. PASSING IN LONGER INPUT WILL 
 * LEAD TO A BUFFER OVERRUN AND UNDEFINED RESULTS. Of course,
 * that would be over 4.7 trillion gigabytes of data, so problems
 * are unlikely in practice anytime soon. :)
 *
 * Requires the tiger() function as defined in the reference
 * implementation provided by the creators of the Tiger
 * algorithm. See
 *
 *    http://www.cs.technion.ac.il/~biham/Reports/Tiger/
 *
 * $Id$
 *
 */

#include <string.h>
#include "tigertree.h"

/* Initialize the tigertree context */
void tt_init(TT_CONTEXT *ctx)
{
  ctx->count = 0;
  ctx->leaf[0] = 0; // flag for leaf  calculation -- never changed
  ctx->node[0] = 1; // flag for inner node calculation -- never changed
  ctx->block = ctx->leaf + 1 ; // working area for blocks
  ctx->index = 0;   // partial block pointer/block length
  ctx->top = ctx->nodes;
}

static void tt_compose(TT_CONTEXT *ctx) {
  byte *node = ctx->top - NODESIZE;
  memmove((ctx->node)+1,node,NODESIZE); // copy to scratch area
  tiger((word64*)(ctx->node),(word64)(NODESIZE+1),(word64*)(ctx->top)); // combine two nodes
  tiger_to_canonical((byte *)ctx->top);
  memmove(node,ctx->top,TIGERSIZE);           // move up result
  ctx->top -= TIGERSIZE;                      // update top ptr
}

static void tt_block(TT_CONTEXT *ctx)
{
  word64 b;

  tiger((word64*)ctx->leaf,(word64)ctx->index+1,(word64*)ctx->top);
  tiger_to_canonical((byte *)ctx->top);
  ctx->top += TIGERSIZE;
  ++ctx->count;
  b = ctx->count;
  while(b == ((b >> 1)<<1)) { // while evenly divisible by 2...
    tt_compose(ctx);
    b = b >> 1;
  }
}

void tt_update(TT_CONTEXT *ctx, const byte *buffer, word32 len)
{

  if (ctx->index)
  { /* Try to fill partial block */
	  unsigned left = BLOCKSIZE - ctx->index;
	  if (len < left)
		{
		memcpy(ctx->block + ctx->index, buffer, len);
		ctx->index += len;
		return; /* Finished */
		}
	  else
		{
		memcpy(ctx->block + ctx->index, buffer, left);
		ctx->index = BLOCKSIZE;
		tt_block(ctx);
		buffer += left;
		len -= left;
		}
  }

  while (len >= BLOCKSIZE)
	{
	memcpy(ctx->block, buffer, BLOCKSIZE);
	ctx->index = BLOCKSIZE;
	tt_block(ctx);
	buffer += BLOCKSIZE;
	len -= BLOCKSIZE;
	}
  if ((ctx->index = len))     /* This assignment is intended */
	{
	/* Buffer leftovers */
	memcpy(ctx->block, buffer, len);
	}
}

// no need to call this directly; tt_digest calls it for you
static void tt_final(TT_CONTEXT *ctx)
{
  // do last partial block, unless index is 1 (empty leaf)
  // AND we're past the first block
  if((ctx->index>0)||(ctx->top==ctx->nodes))
    tt_block(ctx);
}

void tt_digest(TT_CONTEXT *ctx, byte *s)
{
  tt_final(ctx);
  while( (ctx->top-TIGERSIZE) > ctx->nodes ) {
    tt_compose(ctx);
  }
  memcpy(s,ctx->nodes,TIGERSIZE);
}

// this code untested; use at own risk
void tt_copy(TT_CONTEXT *dest, TT_CONTEXT *src)
{
  int i;
  dest->count = src->count;
  for(i=0; i < BLOCKSIZE; i++)
    dest->block[i] = src->block[i];
  dest->index = src->index;
  for(i=0; i < STACKSIZE; i++)
    dest->nodes[i] = src->nodes[i];
  dest->top = src->top;
}

