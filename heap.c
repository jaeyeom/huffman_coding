/*
 * heap.c
 *
 * heap.c offers array based heap operations.
 *
 * Author: Yeom Jaehyun
 * Date: 2003/11/22
 */

/* include files */
#include <stdlib.h>  // stdlib for malloc(), free()
#include <stdio.h>
#include "heap.h"

#define PARENT (i-1)/2
#define LEFT_CHILD i*2+1
#define RIGHT_CHILD i*2+2

/*
 * function new_tnode
 *
 * Allocate memory for a new tnode and initialize the node.
 * 
 * Arguments:
 *      int frq - Frequency data of new tree node.
 *      unsigned char c - character data of new tree node.
 *          Non-leaf tree node has NULL character.
 *      tnode_pointer left, right - left and right node of the node.
 *          Bit 0 to the left node, bit 1 to the right node.
 *
 * Returns:
 *      Pointer of newly allocated tree node.
 *      NULL if failed to allocate.
 */
tnode_pointer new_tnode(int frq, unsigned char c, tnode_pointer left, tnode_pointer right)
{
  tnode_pointer tp = malloc(sizeof(tnode_struct));  // allocates
  if (tp == NULL) return tp;  // if fails, return NULL.
  tp->frq = frq;
  tp->c = c;
  tp->left = left;
  tp->right = right;
  return tp;
}

/*
 * function free_tnode.
 *
 * Deallocate all child tree node from tn.
 * And deallocate tn.
 *
 * Arguments:
 *      tnode_pointer tn - Free child node recursively from tn,
 *      and free itself.
 */
void free_tnode(tnode_pointer tn)
{
  if (tn->left != NULL) free_tnode(tn->left);
  if (tn->right != NULL) free_tnode(tn->right);
  free(tn);
}

/*
 * function new_heap
 *
 * Allocate memory for new heap.
 * 
 * Returns:
 *      Pointer of newly allocated heap.
 */
heap_pointer new_heap()
{
  heap_pointer hep = malloc(sizeof(heap_struct));
  hep->size = 0;
  return hep;
}

/*
 * function free_heap
 *
 * Deallocate memory for heap.
 * Free all tree nodes in the heap. ( heap[i] that 0<=i<size )
 */
void free_heap(heap_pointer h)
{
  int i;
  for (i=0; i < h->size; i++)
    free_tnode(h->node[i]);
  free(h);
}

/*
 * function heap_add
 *
 * Add tnode to heap.
 *  1. Add new node at the end of the heap.
 *  2. Percolate up from the added node.
 * 
 * How to Percolate Up:
 *  1. Comapare (i)th and PARENT node frequency.
 *  2. If PARENT node frequency is bigger, swap them.
 *  3. Change i to its PARENT.
 */
void heap_add(heap_pointer h, tnode_pointer n)
{
  int i = h->size;
  tnode_pointer tmp;
  if (h->size >= HEAP_SIZE) {  // heap overflow
    fprintf(stderr, "Heap full error!\n");
    return;
  }
  h->node[h->size++] = n;  // Add new node at the end of the heap
  while (i>0) {  // percolate up until root
    if (h->node[i]->frq < h->node[PARENT]->frq) {  // if PARENT is bigger
      tmp = h->node[i];
      h->node[i] = h->node[PARENT];  // swap
      h->node[PARENT] = tmp;
      i = PARENT;  // change i to PARENT
    }
    else break;
  }
}

/*
 * function heap_remove
 *
 * Remove the minimum element in heap.
 *  1. Remove the root element of heap. (node[0])
 *  2. Move the last node to root.
 *  3. Percolate down from the root.
 *
 * How to percolate down:
 *  1. Compare LEFT_CHILD and RIGHT_CHILD, and set 'smaller' to smaller one.
 *  2. Compare (i)th and (smaller)th node frequency.
 *  3. if (smaller)th node is smaller, swap them.
 *  4. Change i to smaller.
 * 
 * Returns:
 *      Pointer of the minimum frequency tree node.
 */
tnode_pointer heap_remove(heap_pointer h)
{
  int i=0;
  tnode_pointer tp = h->node[0];
  tnode_pointer tmp;
  int smaller;
  if (h->size<=0) return NULL;  // Heap empty returns NULL
  h->node[0] = h->node[--h->size];  // Remove the root element of heap
    // And move the last node to root.
  while (LEFT_CHILD < h->size) {  // Percolate down until leaf node
    if (RIGHT_CHILD == h->size) {  // If single child
      if (h->node[LEFT_CHILD]->frq < h->node[i]->frq) {  // if child is smaller
        tmp = h->node[LEFT_CHILD];
        h->node[LEFT_CHILD] = h->node[i];  // change them
        h->node[i] = tmp;
      }
      break;
    }
    else {  // If double child
      smaller = (h->node[LEFT_CHILD]->frq < h->node[RIGHT_CHILD]->frq?LEFT_CHILD:RIGHT_CHILD);
      if (h->node[i]->frq > h->node[smaller]->frq) {  // if smaller child is smaller than i
        tmp = h->node[i];
        h->node[i] = h->node[smaller];  // change i and smaller
        h->node[smaller] = tmp;
        i=smaller;
      }
      else break;
    }
  }
  return tp;
}
