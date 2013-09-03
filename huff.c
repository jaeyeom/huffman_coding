/*
 * huff.c
 *
 * huff.c offers shared functions.
 *
 * This huffman coding is general purpose.
 *   - Can encode binary files.
 *   - Can encode maximum 2 Giga Bytes file. (int range)
 *   - Buffered file I/O, so encode/decode faster.
 *
 * Author: Yeom Jaehyun
 * Date 2003/11/22
 */
#include <stdio.h>
#include "huff.h"

/* global variables */
char infilename[256], binfilename[256], frqfilename[256], outfilename[256];
int frq[256], bitlength[256], huffcode[256], tmplen=0, tmpcode;
unsigned char buf[BUFSIZ], wbuf[BUFSIZ];
int originalsize=0, encodedsize=0;
heap_pointer h;
tnode_pointer root;

/*
 * function file_error
 *
 * print file error message
 */
void file_error(char *filename)
{
  fprintf(stderr, "Couldn't open the file: %s\n", filename);
}

/*
 * function make_frqfile
 * 
 * Read input file, count frequency of input file. Frequency file
 * DOES NOT saves huffman tree or huffman code.
 *
 * Frequency File Structure :::
 *  To reduce size of frequency file, (as size is very important)
 *  this frequency file uses special file structure. As this
 *  huffman coding can encode binary files, it is a little
 *  difficult to reduce frequency size. (Must save non-alphabetic
 *  characters) Start with byte 0 to byte 255 frq file saves this
 *  form first.
 *      If frequency is 0, saves 1 bit '0'.
 *      If frequency is greater than 0,
 *          If frequency is represented by 1 byte, saves '100'
 *          If frequency is represented by 2 bytes, saves '101'
 *          If frequency is represented by 3 bytes, saves '110'
 *          If Frequency is represented by 4 bytes, saves '111'
 *
 *  Afrer all headers for 0~255 was written, saves frequency for
 *  frequency is greater than 0, with minimal bytes.
 *
 * Frequency file does not use block writing, different from bin file.
 * So reading and writing frequency file is slow.
 */
void make_frqfile()
{
  FILE *inf, *frqf;  // input file, frequency file
  int readsize, i, bitnum=0, bytenum, tmp=0, tmp2;

  inf = fopen(infilename, "rb");  // open input file.
  if (inf == NULL) {
    file_error(infilename);  // file not found error
    exit(1);
  }
  while ( (readsize = fread(buf, 1, BUFSIZ, inf)) != 0 ) {
    /* read one block to buf */
    putchar('.');  // put one dot for each block
    fflush(stdout);  // flush dot output
    originalsize += readsize;  // count original size
    /* count frequency */
    for (i=0; i<readsize; i++) frq[(int)buf[i]]++;
  }
  fclose(inf);/* close input file. */
  printf(" %d bytes\n",originalsize);  // write out original size
  frqf = fopen(frqfilename, "wb");  // open frequency file
  for (i=0; i<256; i++) {  // for 0 ~ 255
    if (frq[i] == 0) {  // if frequency is 0
      bitnum++;  // increse saved bit number
      tmp = tmp << 1;  // shift left tmp variable
    }
    else {  // if frequency is >0
      bitnum += 3;  // increse saved bit number by 3
      tmp = (tmp << 1) | 1;  // shift left and set 1 to the right most bit
      bytenum = 1;
      /* count how many bytes are needed to represent frequency */ 
      while (frq[i] >> (8 * bytenum) !=0 ) bytenum++;

      /* save two bit by (needed byte-1) such as "00","01","10","11" */
      tmp = (tmp << 2) | (bytenum-1);
    }
    if (bitnum >= 8) {  // if saved bit is more than 8
      tmp2 = tmp >> (bitnum-8);
      fwrite(&tmp2, 1, 1, frqf);  // saves one byte
      bitnum -= 8;
    }
    
  }
  if (bitnum != 0) {  // write remainded bits
    tmp = tmp << (8-bitnum);
    fwrite(&tmp, 1, 1, frqf);
  }
  
  for (i=0; i<256; i++) {  // write frequency of each byte with minimal bytes
    if (frq[i] != 0) {
      bytenum = 1;
      while (frq[i] >> (8 * bytenum) != 0 ) bytenum++;
      fwrite(&frq[i], 1, bytenum, frqf);
    }
  }
  fclose(frqf);
  
}

/*
 * function read_frqfile
 * 
 * Read the frequency file. Frequency file DOES NOT saves huffman
 * tree or huffman code.
 *
 * Frequency File Structure :::
 *  To reduce size of frequency file, (as size is very important)
 *  this frequency file uses special file structure. As this
 *  huffman coding can encode binary files, it is a little
 *  difficult to reduce frequency size. (Must save non-alphabetic
 *  characters) So reading frequency file is difficult too.
 *  But I offer frqdump utility to see frequency file.
 *
 * How to Read :::
 *  Read one bit.
 *      If '0' frequency is 0, and to the next bit.
 *      If '1' frequency is not 0, read next two bits.
 *          If "00" frequency is saved by 1 byte.
 *          If "01" frequency is saved by 2 byte.
 *          If "10" frequency is saved by 3 byte.
 *          If "11" frequency is saved by 4 byte.
 * 
 * After reading all 0~255 characters, read each byte of
 * each characters.
 *
 * Frequency file does not use block writing, different from bin file.
 * So reading and writing frequency file is slow.
 */
void read_frqfile()
{
  FILE *frqf;
  int i, bitnum=0, tmp;

  frqf = fopen(frqfilename, "rb");// open frequency file
  if (frqf == NULL) {
    file_error(frqfilename);// file not found exception
    exit(1);
  }
  fread(&tmp, 1, 1, frqf);// read one byte
  bitnum=0;
  for (i=0; i<256; i++) {// for each 0~255
    if (bitnum==8) {// if read bit is 8
      bitnum=0;
      fread(&tmp, 1, 1, frqf);// read one more byte
    }
    /* read first bit */
    frq[i] = (((tmp << bitnum++) & 0x80) >> 7) & 0x1;
    if (frq[i]==1) {
      /* read bit is '1' */
      if (bitnum==8) {// if read bit is 8
        bitnum=0;
        fread(&tmp, 1, 1, frqf);// read one more byte
      }
      if (bitnum != 7) {
        frq[i] += (((tmp << bitnum) & 0xc0) >> 6) & 0x3;
        bitnum += 2;
      }
      else { // if read bit is 7
        frq[i] += (tmp & 1) << 1;// must read 1 bit and
        fread(&tmp, 1, 1, frqf);// read one byte
        frq[i] += (tmp & 0x80) >> 7;// read one more 1 bit
        bitnum = 1;
      }
    }
  }
  for (i=0; i<256; i++) {// for each 0~255
    if (frq[i]==0) continue;// if frequency is 0, don't read.
    fread(&frq[i], 1, frq[i], frqf);// read n byte 
    originalsize += frq[i];// add original size by frq[i]
  }
  fclose(frqf);
}

/*
 * function make_huffcode
 * 
 * Makes huffman code from huffman tree recursively.
 * Encoding binary files has some difficulties.
 *
 * Input: Huffman tree root node
 * 
 * Really Really Worst case :::::::
 *
 *     one '\0', two '\1's, four '\2's, eight '\3's ...
 *     2^i for '\i' for all character set.
 *     
 *     one + two < four, one + two + four < eight . . .
 *     
 *     Huffman code for '\0' is "0000000 ... 0000000", that's 255 zeros.
 *     Huffman code for '\1' is "0000000 ... 0000001", that's 254 zeros and 1 one.
 *     Huffman code for '\2' is "0000000 ... 000001" , that's 253 zeros and 1 one.
 *     
 *     So maximum size of huffman code is 255.
 *
 * So we must use array of int for huffman code.
 * (Actually I made it.....)
 * BUT..... I realized that it exceed 2 Giga byte, if huffman code is longer than 32 bits.
 * So huffcode for one character is just one integer. (T.T) I deleted all code for multi-
 * integer sized huffman code.
 *
 */
void make_huffcode(tnode_pointer tn)
{
  if (tn->left == NULL) {  // if leaf node
    bitlength[(int)tn->c] = tmplen;  // save bit length and
    huffcode[(int)tn->c] = tmpcode;  // huffman code
    return;
  }

  /* before recursion to the left */
  tmpcode = tmpcode << 1;  // add bit 0
  tmplen++;

  /* recursion to the left */
  make_huffcode(tn->left);

  /* before recursion to the right */
  tmpcode = tmpcode | 1;  // add bit 1

  /* recursion to the right */
  if (tn->right != NULL) make_huffcode(tn->right);

  /* recover values after recursions */
  tmplen--;
  tmpcode = tmpcode >> 1;
}

/*
 * function make_hufftree
 *
 * Make hufftree using heap.
 *
 * 1. For all character set, if frequency is over 0, make new tree node
 *    and put it in min heap.
 * 2. Make new tree node and get 2 items from min heap.
 * 3. Set 2 items as left and right node of the new tree node.
 * 4. Repeat number 2 and number 3 until heap contains only one item.
 * 5. Then the remained tree node is the root node of whole huffman tree.
 *
 * If only 1 character set exists set it as huffcode 0.
 * root -> left = (the only tnode), root->right = NULL.
 * ** Takes left node we can check leaf node if left node is NULL.
 *
 * If no character set exists (empty file)
 * root -> left = NULL, root -> right = NULL.
 */
void make_hufftree()
{
  int i;
  tnode_pointer tn1, tn2;
  h = new_heap();  // create new heap.
  for (i=0; i<256; i++)  // for each character set
    if (frq[i] > 0)   // if frequency is over 0, add new tnode to heap
      heap_add(h, new_tnode(frq[i], (unsigned char)i, NULL, NULL));
  
  if (h->size <= 1) {  // if heap size is not greater than 1
    root = new_tnode(0,'\0',heap_remove(h),NULL);
    /* root->left=(the only tnode), root->right=NULL
     * If no items are in heap, heap_remove(h) returns NULL
     * So both case OK.
     */
    free_heap(h);  // destroy heap and return
    return;
  }
  while (h->size > 1) {  // loop
    tn1 = heap_remove(h);  // get 2 items from heap
    tn2 = heap_remove(h);
    heap_add(h, new_tnode(tn1->frq+tn2->frq, '\0', tn1, tn2));
    // add new node that has 2 children
  }
  root = heap_remove(h);  // remainded node is root node of whole hufftree
  free_heap(h);  // destroy heap
}
