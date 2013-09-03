/*
 * frqdump.c
 *
 * Frequency file dumping utility.
 *
 * Frequency File Structure :::
 *  To reduce size of frequency file, (as size is very important)
 *  this frequency file uses special file structure. As this
 *  huffman coding can encode binary files, it is a little
 *  difficult to reduce frequency size. (Must save non-alphabetic
 *  characters)
 *
 * Because frequency file is not text format, you must use this utility
 * for reading frequency file. Frequency file DOES NOT save huffman tree
 * or huffman code. But this utility will read frequency file, get
 * frequency data, make huffman tree, make huffman code. So you can also
 * see huffman code for each character set, made from frequency data.
 *
 * Usage:
 *  frqdump [frq_file]
 *
 * Default frequency file: "huffman.frq"
 * 
 * You can simply type "frqdump"
 * 
 * Author: Yeom Jaehyun
 * Date: 2003/11/22
 */
#include <stdio.h>
#include "huff.h"

extern char frqfilename[256];
extern int frq[256], bitlength[256], huffcode[256], tmplen, tmpcode, originalsize;
extern unsigned char buf[BUFSIZ], wbuf[BUFSIZ];
extern heap_pointer h;
extern tnode_pointer root;

/*
 * function dump_frq
 *
 * Type of dump ::
 * Code: 0x41  Char: 'A'  Frqncy: 250 Huffman Code: 1000110
 */
void dump_frq()
{
  int i, j;
  char c;

  printf("frequency file dump utility.\nDumping file %s\n\nOriginal Size: %d\n", frqfilename, originalsize);
  printf("List of Frequency and Huffman Code. . .\n");
  for (i=0; i<256; i++) {
    if (frq[i] > 0) {
      if (i < 32) c = ' ';
      else c = (char)i;
      printf("Code: 0x%x\tChar: \'%1c\'\tFrqncy: %d\tHuffman Code: ", i, c, frq[i]);
      for (j=bitlength[i]-1; j>=0;j--)
        printf("%d", (huffcode[i] >> j) & 1);
      printf("\n");
    }
  }
}

/*
 * This utility can take argument.
 *
 * Four Steps :::
 *  1. Read frequency file.
 *  2. Make huffman tree.
 *  3. Make huffman code.
 *  4. Dump frequency and huffman code.
 */
int main(int argc, char *argv[])
{
  if (argc < 2)
    strcpy(frqfilename, DEF_FRQFILE);
  read_frqfile();       // Read frequency file.
  make_hufftree();      // Make huffman tree.
  make_huffcode(root);  // Make huffman code.
  dump_frq();           // Dump frequency and huffman code.
  return 0;
}
