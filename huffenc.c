/*
 * huffenc.c
 *
 * Utility for encoding huffman code.
 *
 * Usage:
 *   huffenc [input_file] [bin_file] [frq_file]
 * 
 * Default input_file = "huffman.in"
 * Default bin_file = "huffman.bin"
 * Defailt frq_file = "huffman.frq"
 *
 * You can simply type "huffenc".
 *
 * Author: Yeom Jaehyun
 * Date: 2003/11/22
 */
#include <stdio.h>
#include "huff.h"

extern char infilename[256], binfilename[256], frqfilename[256];
extern int frq[256], bitlength[256], huffcode[256], tmplen, tmpcode;
extern unsigned char buf[BUFSIZ], wbuf[BUFSIZ];
extern int originalsize, encodedsize;
extern heap_pointer h;
extern tnode_pointer root;

/*
 * function writebinfile()
 *
 * Write bin file with huffman encoding.
 * Block read/write for faster file I/O.
 */
void writebinfile()
{
  FILE *binf;
  FILE *inf;
  unsigned char tmp=0;
  int i, j, tmpsaved=0, readsize, writesize=0;
  inf = fopen(infilename, "rb");
  if (inf == NULL) {
    file_error(infilename);
    exit(1);
  }
  binf = fopen(binfilename, "wb");
  while ( (readsize = fread(buf, 1, BUFSIZ, inf)) != 0 ) {  // for each block
    for (i=0; i<readsize; i++) {   // for each read byte
      for (j=bitlength[(int)buf[i]]-1; j>=0; j--) {  // for each bits
        tmpsaved++;
        // add bits
        tmp = (tmp << 1) | ((huffcode[(int)buf[i]] >> j) & 1);
        // if 8 bits saved
        if (tmpsaved == 8) {
          // save the byte and go to next byte
          wbuf[writesize++] = tmp;
          tmp = 0;
          tmpsaved = 0;
          if (writesize==BUFSIZ) {   // if buffer full
            fwrite(wbuf, 1, BUFSIZ, binf);   // write buffer
            putchar('.');   // put one point for each block
            fflush(stdout);
            writesize = 0;
            encodedsize += BUFSIZ;
          }
        }
      }
    }
  }

  if (tmpsaved != 0) wbuf[writesize++] = tmp << (8-tmpsaved);// save remainded bits
  fwrite(wbuf, 1, writesize, binf);// save remainded bytes
  encodedsize += writesize;
  putchar('.');
  printf(" %d bytes(%3.1f%%)\n",encodedsize,(double)encodedsize/originalsize*100);
  fclose(inf);
  fclose(binf);
}

/*
 * main function
 *
 * If there are no arguments, use the default values.
 *
 * Four steps :::
 *   1. make frequency file
 *   2. make huffman tree
 *   3. make huffman code
 *   4. write encoded bin file
 */
int main(int argc, char *argv[])
{
  if (argc < 2)
    strcpy(infilename,DEF_INFILE);
  else
    strcpy(infilename,argv[1]);
  if (argc < 3)
    strcpy(binfilename,DEF_BINFILE);
  else
    strcpy(binfilename,argv[2]);
  if (argc < 4)
    strcpy(frqfilename,DEF_FRQFILE);
  else
    strcpy(frqfilename,argv[3]);

  make_frqfile();       // make frequency file
  make_hufftree();      // make huffman tree
  make_huffcode(root);  // make huffman code
  writebinfile();       // write encoded bin file
  return 0;
}
