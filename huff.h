/*
 * huff.h
 *
 * Header file for huff.c
 *
 * Author: Yeom Jaehyun
 * Date: 2003/11/22
 */

/* Default file names. */
#define DEF_INFILE "huffman.in"
#define DEF_OUTFILE "huffman.out"
#define DEF_BINFILE "huffman.bin"
#define DEF_FRQFILE "huffman.frq"

/* must include heap.h because make_huffcode takes tnode_pointer. */
#include "heap.h"

/* Functions huff.c offers */
extern void make_frqfile();
extern void read_frqfile();
extern void make_huffcode(tnode_pointer tn);
extern void make_hufftree();
extern void file_error(char *filename);
