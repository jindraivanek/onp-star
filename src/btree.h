#ifndef _ONP_BTREE_H
#define _ONP_BTREE_H

#include "common.h"

/* BTree - jednoduchy B* strom. Redundantni - vsechna data v listech, jinak pouze klice. Klice se mohou opakovat. Listy odkazuji na sveho praveho bratra.
 *
 */

//#define BTREE_LOG_VERBOSE 1
#define BTreeLog(number,cmd) {if(number<=BTREE_LOG_VERBOSE) cmd;}

#define ARRAY_COEF 2

//Nafukovaci pole pointeru, ktere umoznuje mit prazdna mista
typedef struct Array {
  int n;
  int alloc; //alokovanych polozek
  void** data;
} Array;


#define BTREE_POINTER int
#define BTREE_POINTER_SIZE sizeof(BTREE_POINTER)
#define BPS BTREE_POINTER_SIZE //zkratka, pouzivam to az prilis casto

typedef int (*BTreeCompareFunction)(void*,void*);

typedef struct BTree {
  FILE* f;
  BTreeCompareFunction cmpf;
  Array* pages;
  int pagesOpened, pagesChanged, pagesOpenedMax;
  int count;
  //vsechny velikosti v bajtech
  int pageSize;
  int keySize;
  int dataSize;
  BTREE_POINTER root; //offset korenoveho uzlu (nemusi byt na zacatku souboru - insert muze vytvorit novy koren
  int leftmost; //odkaz na nejlevejsi list
  int _keyN; //velikost uzlu pro klice (pocet dat. polozek) = pageSize/(keySize+BPS)
  int _dataN; //velikost datoveho uzlu (listu), zda jsou klic a data a jeden odkaz na praveho bratra
  int _pageHeaderSize;
  int _keyItemSize;
  int _dataItemSize;
} BTree;

typedef struct BTreePage {
  int id; //index v BTree->pages, zaroven offset na ulozeni buf
  int n; //pocet polozek
  BTREE_POINTER parent; //offset rodice
  int type; //0=klic, 1=data
  char* buf;
  int change; //doslo ke zmene ve stance, ktera vyzaduje ulozeni
  //samotne data stranky: (p=pointer, p_s=pointer na souseda, k=klic, d=data)
  //typ==0: p_0 k_1 p_1 k_2 ... k_n p_n
  //typ==1: p_s k_1 d_1 k_2 d_2 ... k_n d_n
} BTreePage;

Array* ArrayCreate(int alloc);
void ArrayRealloc(Array* a);
int ArrayInsert(Array* a, void* x);
void ArraySet(Array* a, int i, void* x);
void* ArrayGet(Array* a, int i);
void ArrayFree(Array* a);
BTreePage* BTreeGetPage(BTree* bt, int id);
int BTreePageGetN(BTree* bt, BTreePage* p);
int BTreePageItemSize(BTree* bt, BTreePage* p);
BTREE_POINTER BTreeGetPointer(BTree* bt, BTreePage* p, int index);
int BTreePageSave(BTree* bt, BTreePage* p);
BTreePage* BTreePageLoad(BTree* bt, BTreePage* p);
BTreePage* BTreeGetPageLoad(BTree* bt, int id);
int BTreePageCreate(BTree* bt, char type, int parent);
void BTreePageDestroy(BTree* bt, BTreePage* p);
void* BTreePageDelete(BTree* bt, int id, int index);
int BTreePageKeySearch(BTree* bt, int id, void* key);
void* BTreePageDataSearch(BTree* bt, int id, void* key, int del);
int BTreePageInsert(BTree* bt, int id, void* key, void* data, int pointer);
int BTreePageSplit(BTree* bt, int id, void* key, void* data, int pointer, int index);
int BTreePageLend(BTree* bt, BTreePage* par, BTreePage* l, BTreePage* r, int index);
int BTreePageJoin(BTree* bt, BTreePage* par, BTreePage* l, BTreePage* r, int index);
int BTreePageAfterDelete(BTree* bt, int id);
void BTreeCommit(BTree* bt);
BTree* BTreeCreate(char* filename, BTreeCompareFunction cmpf, int pageSize, int keySize, int dataSize, int memMax);
void BTreeDestroy(BTree* bt);
int BTreeCheck(BTree* bt, int root);
int BTreeGetCount(BTree* bt);
void BTreeInsert(BTree* bt, void* key, void* data);
void* BTreeSearchDelete(BTree* bt, void* key, int del);
void* BTreeSearch(BTree* bt, void* key);
void** BTreePopN(BTree* bt, int n);

#endif
