#include <stdio.h>
#include <stdlib.h>
#include "../src/btree.h"
/*
 *
 */

int intcmp(void* a, void* b) {
  if(*(int*)a==*(int*)b) return 0;
  if(*(int*)a<*(int*)b) return -1;
  else return 1;
}

void printPage(BTree* bt, int offset) {
  BTreePage* p = BTreeGetPageLoad(bt, offset);
  int c;
  if(p->type==0) c = bt->keySize+BTREE_POINTER_SIZE;
  else c = bt->keySize+bt->dataSize;
  if(p->type==1) {
    printf("d%i(%i): p%i, ", offset, p->parent, BTreeGetPointer(bt,p,0));
    int i;
    for(i=0; i<p->n; i++) printf("k%i d%i, ", *(int*)(&p->buf[c*i+BTREE_POINTER_SIZE]), *(int*)(&p->buf[c*(i+1)]));
    printf("\n");
  }
  else {
    printf("k%i(%i): p%i, ", offset, p->parent, BTreeGetPointer(bt,p,0));
    int i;
    for(i=0; i<p->n; i++) printf("k%i p%i, ", *(int*)(&p->buf[c*i+BTREE_POINTER_SIZE]), BTreeGetPointer(bt,p,i+1));
    printf("\n");
    for(i=0; i<=p->n; i++) printPage(bt, BTreeGetPointer(bt,p,i));
  }
}

int print(BTree* bt) {
  printPage(bt, bt->root);
}

#define KEY int
#define DATA int

int main(int argc, char** argv) {
  BTree* bt = BTreeCreate("btreetest.db", intcmp, 32, sizeof(KEY), sizeof(DATA));
  bt->pagesOpenedMax=5;
  printf("%i,%i\n", bt->_keyN, bt->_dataN);
//   int* a = (int*) malloc(sizeof(int)); *a = 1;
//   int* b = (int*) malloc(sizeof(int)); *b = 2;
//   int* c = (int*) malloc(sizeof(int)); *c = 3;
//   int* d = (int*) malloc(sizeof(int)); *d = 4;
//   BTreeInsert(bt, a, b);
//   BTreeInsert(bt, c, d);
//   BTreeInsert(bt, b, a);
//   BTreeInsert(bt, d, c);

  int n=100;
  int i;
  for(i=n-1; i>0; i-=3) {
    KEY* a = (KEY*) malloc(sizeof(KEY)); *a = i;
    DATA* b = (DATA*) malloc(sizeof(DATA)); *b = i+1000;
    BTreeInsert(bt, a, b);
    printf("INSERT: %i\n", *b);
    print(bt);
  }
  for(i=1; i<n; i+=3) {
    KEY* a = (KEY*) malloc(sizeof(KEY)); *a = i;
    DATA* b = (DATA*) malloc(sizeof(DATA)); *b = i+1000;
    BTreeInsert(bt, a, b);
    printf("INSERT: %i\n", *b);
    print(bt);
  }
  for(i=2; i<n; i+=3) {
    KEY* a = (KEY*) malloc(sizeof(KEY)); *a = i;
    DATA* b = (DATA*) malloc(sizeof(DATA)); *b = i+1000;
    BTreeInsert(bt, a, b);
    printf("INSERT: %i\n", *b);
    print(bt);
  }
/*
  for(i=n-1; i>0; i--) {
    int* a = (int*) malloc(sizeof(int)); *a = i;
    int* x = (int*) BTreeSearchDelete(bt,a,1);
    assert(x!=NULL);
    printf("%i\n", *x);
    print(bt);
  }

  int j;
  void** r;
  for(j=0; j<100; j++) {
  printf("POP: ");
  r = BTreePopN(bt,n/2);
  for(i=0; i<n/2; i++) printf("%f,", *((DATA*)r[i]));
  printf("\n");
  //print(bt);
  for(i=1; i<=n/2; i++) {
    KEY* a = (KEY*) malloc(sizeof(KEY)); *a = i;
    DATA* b = (DATA*) malloc(sizeof(DATA)); *b = i+1000;
    BTreeInsert(bt, a, b);
    printf("INSERT: %f\n", *b);
    //print(bt);
  }
  BTreeCheck(bt);
  }
  printf("POP\n");
  r = BTreePopN(bt,n/2);
  for(i=0; i<n/2; i++) printf("%i,", *((DATA*)r[i]));
  printf("\n");
  print(bt);
 */
 return (EXIT_SUCCESS);
}

