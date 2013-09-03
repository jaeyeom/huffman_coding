/*
 * heap.h
 *
 * Header file for heap.c
 * Heap was simply implemented by array.
 *
 * Author: Yeom Jaehyun
 * Date: 2003/11/22
 */

#define HEAP_SIZE 256// Maximum size of heap.

/*
 * tnode_pointer
 *
 * Huffman tree node. Contains one character c, and its frequency,
 * left and right node. Only leaf node has appropriate character c,
 * non-leaf trees has NULL character.
 */
typedef struct tnode *tnode_pointer;
typedef struct tnode {
  unsigned char c;
  int frq;// frequency
  tnode_pointer left, right;
} tnode_struct;

/*
 * heap_pointer
 *
 * Array based heap. Contains its size and array of tnodes.
 */
typedef struct heap *heap_pointer;
typedef struct heap {
  int size;
  tnode_pointer node[HEAP_SIZE];
} heap_struct;

/* Functions heap.c offers */
extern tnode_pointer new_tnode(int frq, unsigned char c, tnode_pointer left, tnode_pointer right);
extern void free_tnode(tnode_pointer tn);
extern heap_pointer new_heap();
extern void free_heap(heap_pointer h);
extern void swap(void *a, void *b);
extern void heap_add(heap_pointer h, tnode_pointer n);
extern tnode_pointer heap_remove(heap_pointer h);
