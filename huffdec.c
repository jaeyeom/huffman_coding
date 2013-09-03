/*
 * huffdec.c
 *
 * Utility for encoding huffman code.
 *
 * Usage:
 * huffdec [output_file] [bin_file] [frq_file]
 *
 * Default output_file = "huffman.out"
 * Default bin_file = "huffman.bin"
 * Defailt frq_file = "huffman.frq"
 *
 * You can type simply "huffdec".
 *
 * Author: Yeom Jaehyun
 * Date: 2003/11/22
 */
#include <stdio.h>
#include "huff.h"

extern char outfilename[256], binfilename[256], frqfilename[256];
extern int frq[256], bitlength[256], tmplen, tmpcode[9], originalsize;
extern unsigned char buf[BUFSIZ], wbuf[BUFSIZ];
extern heap_pointer h;
extern tnode_pointer root;

/*
 * function writeoutfile
 *
 * Write out file with huffman decoding.
 * Block read/write for faster file I/O.
 */
void writeoutfile()
{
  FILE *binf;
  FILE *outf;
  unsigned char tmp=0;
  int tmploaded=0, readsize=0, writesize=0, encodedsize, remainedsize=originalsize;
  tnode_pointer tn;

  outf = fopen(outfilename, "wb");
  binf = fopen(binfilename, "rb");
  if (binf == NULL) {  // file not found error
    file_error(binfilename);
    exit(1);
  }
  encodedsize = fread(buf, 1, BUFSIZ, binf);  // read one block
  tmp=buf[readsize++];
  while (remainedsize > 0) {
    tn = root;
    while(tn->left != NULL) {  // if not leaf node
      if ((tmp & (0x80 >> tmploaded))==0)  // read next bit
        tn = tn->left;   // go to left child if bit is 0
      else
        tn = tn->right;  // go to right child if bit is 1
      tmploaded++;
      if (tmploaded==8) {  // if eight bit was read
        tmploaded=0;
        tmp=buf[readsize++];  // take new byte
        if (readsize==BUFSIZ) {  // if nothing more read in buffer
          encodedsize += fread(buf, 1, BUFSIZ, binf);  // load one block
          readsize=0;
        }
      }
    }
    // if leaf node
    wbuf[writesize++]=tn->c;  // put the character
    if (writesize==BUFSIZ) {  // if buffer full
      fwrite(wbuf, 1, BUFSIZ, outf);  // write out one block
      writesize=0;
    }
    // decrease remained byte size
    remainedsize--;
  }
  fwrite(wbuf, 1, writesize, outf);  // file write for remained bytes
  fclose(outf);
  fclose(binf);
  printf("%d bytes(%3.1f) -> %d bytes\n", encodedsize, (double)encodedsize/originalsize*100, originalsize);
}

/*
 * main function
 *
 * This utility can take arguments.
 *
 * Three Steps :::
 *   1. Read frequency file.
 *   2. Make huffman tree.
 *   3. Write output file.
 */
int main(int argc, char *argv[])
{
  if (argc < 2)
    strcpy(outfilename,DEF_OUTFILE);
  else
    strcpy(outfilename,argv[1]);
  if (argc < 3)
    strcpy(binfilename,DEF_BINFILE);
  else
    strcpy(binfilename,argv[2]);
  if (argc < 4)
    strcpy(frqfilename,DEF_FRQFILE);
  else
    strcpy(frqfilename,argv[3]);
  read_frqfile();   // Read frequency file.
  make_hufftree();  // Make huffman tree.
  writeoutfile();   // Write output file.
  return 0;
}
