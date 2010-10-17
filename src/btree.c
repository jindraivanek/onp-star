#include "btree.h"

int divup(int a, int b) { if(a%b>0) return a/b+1; else return a/b; }

Array* ArrayCreate(int alloc) {
  Array* a = malloc(sizeof(Array));
  a->n=0;
  a->data = malloc(sizeof(void*)*alloc);
  memset(a->data, 0, sizeof(void*)*alloc);
  a->alloc = alloc;
  return a;
}

void ArrayRealloc(Array* a) {
  int old = a->alloc;
  while(a->n >= a->alloc) {
    a->alloc = a->alloc*ARRAY_COEF;
    a->data = realloc(a->data, sizeof(void*)*a->alloc);
    memset(&a->data[old], 0, sizeof(void*)*(a->alloc-old));
  }
  while(a->n+1 < a->alloc/ARRAY_COEF) {
    a->alloc = a->alloc/ARRAY_COEF;
    a->data = realloc(a->data, sizeof(void*)*a->alloc);
  }
}

int ArrayInsert(Array* a, void* x) {
  assert(a->n<a->alloc);
  int i=0; while(a->data[i]!=NULL) i++;
  a->data[i] = x;
  if(i==a->n) a->n++;
  ArrayRealloc(a);
  return i;
}

void ArraySet(Array* a, int i, void* x) {
  if(i>=a->n) { a->n=i+1; ArrayRealloc(a); }
  if(x==NULL && i==a->n-1) { a->n--; ArrayRealloc(a); }
  a->data[i] = x;
}

void* ArrayGet(Array* a, int i) {
  if(i>=a->n) return NULL;
  return a->data[i];
}

void ArrayFree(Array* a) {
  free(a->data);
  free(a);
}

BTreePage* BTreeGetPage(BTree* bt, int id) {
  return (BTreePage*) ArrayGet(bt->pages, id);
}

int BTreePageGetN(BTree* bt, BTreePage* p) {
  if(p->type==0) return bt->_keyN;
  else return bt->_dataN;
}

int BTreePageItemSize(BTree* bt, BTreePage* p) {
  if(p->type==0) return bt->_keyItemSize;
  else return bt->_dataItemSize;
}

void BTreePageSetChange(BTree* bt, BTreePage* p) {
  assert(p);
  if(p->change==0) bt->pagesChanged++;
  p->change=1;
}

int BTreePageFindKeyIndex(BTree* bt, BTreePage* p, void* key) {
  assert(p->buf);
  int i=0;
  int c = BTreePageItemSize(bt, p);
  while(i<p->n && bt->cmpf(key, &p->buf[BPS+c*i])==1) i++;
  return i;
}

BTREE_POINTER BTreeGetPointer(BTree* bt, BTreePage* p, int index) {
  assert(p->type==0 || index==0); //u listu ma smysl pouze index 0
  BTREE_POINTER r;
  int c = BTreePageItemSize(bt, p);
  memcpy(&r, &p->buf[c*index], BPS);
  return r;
}

int BTreePageGetIndexInParent(BTree* bt, BTreePage* p, BTreePage* par) {
  assert(p->parent==par->id);
  int index=0;
  while(index<=par->n && BTreeGetPointer(bt, par, index)!=p->id) index++;
  assert(index<=par->n);
  return index;
}

void BTreeDebugPrintPage(BTree* bt, int offset) {
  BTreePage* p = BTreeGetPageLoad(bt, offset);
  if(p->type==1) {
  }
  else {
    printf("k%i(%i): p%i, ", offset, p->parent, BTreeGetPointer(bt,p,0));
    int i;
    for(i=0; i<p->n; i++) printf("p%i, ", BTreeGetPointer(bt,p,i+1));
    printf("\n");
    for(i=0; i<=p->n; i++) BTreeDebugPrintPage(bt, BTreeGetPointer(bt,p,i));
  }
}

int BTreePageSave(BTree* bt, BTreePage* p) {
  if(p->buf) {
    fseek(bt->f, (BTREE_POINTER)p->id*bt->pageSize, SEEK_SET);
    assert(fwrite(p->buf, bt->pageSize, 1, bt->f)==1);
    free(p->buf); p->buf=NULL;
    return 1;
  }
  return -1;
}

BTreePage* BTreePageLoad(BTree* bt, BTreePage* p) {
  if(p->buf==NULL) {
    fseek(bt->f, (BTREE_POINTER)p->id*bt->pageSize, SEEK_SET);
    p->buf = malloc(bt->pageSize);
    assert(fread(p->buf, bt->pageSize, 1, bt->f)==1);
    bt->pagesOpened++;
  }
  return p;
}

BTreePage* BTreeGetPageLoad(BTree* bt, int id) {
  BTreePage* r = (BTreePage*) ArrayGet(bt->pages, id);
  assert(r);
  if(r->buf==NULL) BTreePageLoad(bt, r);
  return r;
}

int BTreePageCreate(BTree* bt, char type, int parent) {
  BTreePage* p = malloc(sizeof(BTreePage));
  p->n=0;
  p->type = type;
  p->parent = parent;
  p->buf = malloc(bt->pageSize); memset(p->buf, 0, bt->pageSize);
  BTREE_POINTER x = -1;
  if(type==1) memcpy(&p->buf[0], &x, BPS);
  p->id = ArrayInsert(bt->pages, (void*) p);
  p->change=0;
  BTreePageSetChange(bt,p);
  return p->id;
}

void BTreePageDestroy(BTree* bt, BTreePage* p) {
  #ifdef BTREE_LOG_VERBOSE 
    BTreeLog(2, printf("BTREE PAGE DESTROY: %i\n", p->id));
  #endif
  if(p->buf) {
    if(p->change==1) bt->pagesChanged--;
    bt->pagesOpened--;
    free(p->buf);
  }
  ArraySet(bt->pages, p->id, NULL);
  free(p);
}

//smaze data na urcitem indexu (bud klic+data, nebo klic+pointer)
//vrati data, prip. klic
void* BTreePageDelete(BTree* bt, int id, int index) {
  BTreePage* p = BTreeGetPage(bt, id);
  if(index>=p->n) return NULL;
  if(p->buf==NULL) BTreePageLoad(bt, p);
  int c = BTreePageItemSize(bt,p);
  void* ret;
  if(p->type==0) {
    ret = malloc(bt->keySize);
    memcpy(ret, &p->buf[c*index+BPS], bt->keySize);
  } else {
    ret = malloc(bt->dataSize);
    memcpy(ret, &p->buf[c*index+BPS+bt->keySize], bt->dataSize);
  }
  memmove(&p->buf[c*index+BPS], &p->buf[c*(index+1)+BPS], (p->n-index-1)*c);
  p->n--;
  BTreePageSetChange(bt,p);
  return ret;
}

//pokusi se najit dany klic ve strance. Vrati odkaz na stranku, kde by se melo pokracovat.
int BTreePageKeySearch(BTree* bt, int id, void* key) {
  BTreePage* p = BTreeGetPageLoad(bt, id);
  assert(p->type==0);
  int i=BTreePageFindKeyIndex(bt,p,key);
  return BTreeGetPointer(bt, p, i);
}

//pokusi se najit data s danym klicem
void* BTreePageDataSearch(BTree* bt, int id, void* key, int del) {
  BTreePage* p = BTreeGetPage(bt, id);
  assert(p->type==1);
  if(p->buf==NULL) BTreePageLoad(bt, p);
  int c = BTreePageItemSize(bt,p);
  int i=BTreePageFindKeyIndex(bt,p,key);
  if(del==1) return BTreePageDelete(bt, id, i);
  else {
    void* x = malloc(bt->dataSize);
    memcpy(x, &p->buf[BPS+c*i+bt->keySize], bt->dataSize);
    return x;
  }
}

//vlozi data do stranky. Pokud je pointer>=0 vlozi i pointer za data. Pokud data==NULL data nevklada.
int BTreePageInsertAt(BTree* bt, int id, void* key, void* data, int pointer, int index) {
  BTreePage* p = BTreeGetPage(bt, id);
  assert((p->type==0 && key!=NULL && data==NULL && pointer>=0) ||
          (p->type==1 && key!=NULL && data!=NULL && pointer<0) ||
          (key==NULL && data==NULL && pointer>=0)); //pouze povolene pouziti
  if((key!=NULL || data !=NULL) && ((p->type==0 && p->n==bt->_keyN) || (p->type==1 && p->n==bt->_dataN))) return -1;
  if(p->buf==NULL) BTreePageLoad(bt, p);
  int c = BTreePageItemSize(bt,p);
  int x;
  if(index==-1) x=0;
  else {
    x=c*index+BPS;
    if(index<p->n) memmove(&p->buf[x+c], &p->buf[x], (p->n-index)*c);
  }

  if(key!=NULL) { memcpy(&p->buf[x], key, bt->keySize); x+=bt->keySize; }
  if(data!=NULL) { memcpy(&p->buf[x], data, bt->dataSize); x+=bt->dataSize; }
  if(pointer>=0) { memcpy(&p->buf[x], &pointer, BPS); }
  if(key!=NULL || data!=NULL) p->n++;
  BTreePageSetChange(bt,p);
  return 1;
}

int BTreePageInsert(BTree* bt, int id, void* key, void* data, int pointer) {
  BTreePage* p = BTreeGetPageLoad(bt, id);
  int i;
  if(key==NULL && data==NULL) i=-1;
  else i=BTreePageFindKeyIndex(bt,p,key);
  return BTreePageInsertAt(bt,id,key,data,pointer,i);
}


//Rozdeli plnou stranku na dve, vytvori ji souseda, vlozi do nich data z puvodni stranky a dalsi prvek.
//Pokud je otec take plny zavola se rekurzivne.
int BTreePageSplit(BTree* bt, int id, void* key, void* data, int pointer, int i) {
  BTreePage* p = BTreeGetPageLoad(bt, id);
  assert((p->type==0 && key!=NULL && data==NULL && pointer>=0) ||
          (p->type==1 && key!=NULL && data!=NULL && pointer<0));
  #ifdef BTREE_LOG_VERBOSE 
    BTreeLog(2, printf("BTREE SPLIT: %i, type %i\n", id, p->type));
  #endif
  if(p->parent==-1) {
    int oldroot = bt->root;
    bt->root = BTreePageCreate(bt, 0, -1); //novy root
    p->parent = bt->root;
    BTreePageInsert(bt, bt->root, NULL, NULL, oldroot);
  }
  int n_id = BTreePageCreate(bt, p->type, p->parent);
  BTreePage* n = BTreeGetPage(bt, n_id);
  int c = BTreePageItemSize(bt,p);
  if(i<0) i=BTreePageFindKeyIndex(bt,p,key); //na kterou pozici by patril vkladany prvek
  int min = divup(p->n,2);
  int d=min; if(i<d) d--; //mez pro prvky v p, ktere tam maji zustat; pokud jde o data, i median tam musi zustat, nahoru zkopiruju jeho klic
  int u=d; if(p->type==0 && i!=min) u++; //od ktereho prvku v p je zkopiruju do n
  int m=u-1; //pozice medianu
  //pokud d==u -> i je median
  memcpy(&n->buf[BPS], &p->buf[c*u+BPS], c*(p->n-u)); n->n=p->n-u; //zkopirovani prvku do n
  void* mKey;
  if(i!=(min-p->type)) {
    mKey = malloc(bt->keySize); //klic medianu
    memcpy(mKey, &p->buf[c*m+BPS], bt->keySize);
    assert(memcmp(mKey, &p->buf[c*m+BPS], bt->keySize)==0);
    if(p->type==0) memcpy(&n->buf[0], &p->buf[c*(m+1)], BPS); //odkaz za medianem musi byt prvnim odkazem nove stranky
  } else {
    mKey=key;
    if(p->type==0) memcpy(&n->buf[0], &pointer, BPS);
  }
  p->n=d; //defacto vymazani prvku na pozici d a dalsi
  if(i<min) BTreePageInsertAt(bt, id, key, data, pointer, i);
  if(i>min || (p->type==1 && i==min)) BTreePageInsertAt(bt, n_id, key, data, pointer, i-u);
  if(p->type==1) {
    int o2 = BTreeGetPointer(bt, p, 0); //mela p nejakeho bratra?
    if(o2!=-1) BTreePageInsert(bt, n_id, NULL, NULL, o2);
    BTreePageInsert(bt, id, NULL, NULL, n_id); //ulozeni odkazu na bratra p
  }
  else {
    for(i=0; i<=n->n; i++) {
      int ox = BTreeGetPointer(bt, n, i);
      BTreeGetPage(bt, ox)->parent = n_id;
    }
  }
  BTreePageSetChange(bt,p);
  assert(p->type==0 || n->id == BTreeGetPointer(bt, p, 0));
  BTreePage* par = BTreeGetPageLoad(bt, p->parent);
  int index = BTreePageGetIndexInParent(bt, p, par);
  if(BTreePageInsertAt(bt, p->parent, mKey, NULL, n_id, index)==-1)
    BTreePageSplit(bt, p->parent, mKey, NULL, n_id, index);
  assert(p->type==0 || n->id == BTreeGetPointer(bt, p, 0));
  if(mKey!=key) free(mKey);
  return 1;
}

int BTreePageLend(BTree* bt, BTreePage* par, BTreePage* l, BTreePage* r, int index) {
  assert(l->type==r->type);
  int c = BTreePageItemSize(bt, l);
  int k = BTreePageItemSize(bt, par);
  int min = divup(BTreePageGetN(bt,r), 2);
  assert(l->id == BTreeGetPointer(bt, par, index) && r->id == BTreeGetPointer(bt, par, index+1));
  assert(l->type==0 || r->id == BTreeGetPointer(bt, l, 0));
  #ifdef BTREE_LOG_VERBOSE 
    BTreeLog(2, printf("BTREE LEND: %i<-%i, type %i\n", l->id, r->id, l->type));
  #endif
  if(r->n > min) {
    if(r->type==1) {
      //prvek ze zacatku r se zkopiruje na konec l
      memcpy(&l->buf[BPS+c*l->n], &r->buf[BPS], c); l->n++;
    }
    else {
      //prvek z otce na konec l, za nej prvni odkaz z r
      memcpy(&l->buf[BPS+c*l->n], &par->buf[BPS+k*index], bt->keySize);
      memcpy(&l->buf[c*(l->n+1)], &r->buf[0], bt->keySize);
      l->n++;
      BTreeGetPage(bt, BTreeGetPointer(bt, r, 0))->parent=l->id;
    }
    //spravne nastaveni klice v otci
    memcpy(&par->buf[BPS+k*index], &r->buf[BPS], bt->keySize);
    if(r->type==1) free(BTreePageDelete(bt, r->id, 0));
    else {
      //smaze prvni odkaz a nasledujici prvek (odkaz je na konci l a prvek v otci)
      memmove(&r->buf[0], &r->buf[c], c*(r->n-1)+BPS);
      r->n--;
    }
  }
  if(l->n > min) {
    if(l->type==1) {
      //prvek z konce l se zkopiruje na zacatek r
      BTreePageInsertAt(bt, r->id, &l->buf[BPS+c*(l->n-1)], &l->buf[BPS+c*(l->n-1)+bt->keySize], -1, 0);
      l->n--;
      //spravne nastaveni klice v otci
      memcpy(&par->buf[BPS+k*index], &l->buf[BPS+c*(l->n-1)], bt->keySize);
    }
    else {
      //prvek z otce na zacatek r, pred nej posledni odkaz z l
      BTreePageInsertAt(bt, r->id, &par->buf[BPS+k*index], NULL, BTreeGetPointer(bt, r, 0), 0);
      memcpy(&r->buf[0], &l->buf[c*l->n], BPS);
      memcpy(&par->buf[BPS+k*index], &l->buf[BPS+c*(l->n-1)], bt->keySize);
      l->n--;
      BTreeGetPage(bt, BTreeGetPointer(bt, r, 0))->parent=r->id;
    }
    
  }
  BTreePageSetChange(bt,l); BTreePageSetChange(bt,r); BTreePageSetChange(bt,par);
  assert(l->type==0 || r->id == BTreeGetPointer(bt, l, 0));
  return 1;
}

int BTreePageJoin(BTree* bt, BTreePage* par, BTreePage* l, BTreePage* r, int index) {
  assert(l->n+r->n <= BTreePageGetN(bt,l));
  assert(index < par->n);
  int c = BTreePageItemSize(bt, l);
  #ifdef BTREE_LOG_VERBOSE 
    BTreeLog(2,printf("BTREE JOIN: %i<-%i, type %i\n", l->id, r->id, l->type));
  #endif
    
  assert(l->type==0 || r->id == BTreeGetPointer(bt, l, 0));
  if(l->type==0) {
    //nejprve pridat klic nejpravejsiho listu podstromu l (pokud se tam nevejde poslat nahoru nejpravejsi prvek r)
    BTreePage* s = l;
    while(s->type==0) s = BTreeGetPageLoad(bt, BTreeGetPointer(bt, s, s->n));
    if(l->n+r->n==BTreePageGetN(bt,l)) {
      return 1; 
    }
    memcpy(&l->buf[BPS+c*l->n], &s->buf[BPS+BTreePageItemSize(bt,s)*(s->n-1)], bt->keySize);
    memcpy(&l->buf[c*(l->n+1)], &r->buf[0], BPS);
    l->n++;
    int i; for(i=0; i<=r->n; i++) {
      s = BTreeGetPage(bt, BTreeGetPointer(bt, r, i));
      s->parent = l->id;
    }
  }
  //zkopiruje obsah r na konec l
  memcpy(&l->buf[BPS+c*l->n], &r->buf[BPS], c*r->n);
  l->n+=r->n;
  //pokud jde o listy, prehraj odkaz na souseda
  if(l->type==1) memcpy(&l->buf[0], &r->buf[0], BPS);
  //nyni smazat prvek v otci s odkazem na r
  BTreePageDestroy(bt, r);
  BTreePageSetChange(bt,l);
  free(BTreePageDelete(bt, par->id, index));
  if(l->type==0) {
    int i; for(i=0; i<=l->n; i++)
      assert(BTreeGetPage(bt, BTreeGetPointer(bt, l, i))->parent==l->id);
  }
  return 1;
}

int BTreePageAfterDelete(BTree* bt, int id) {
  BTreePage* p = BTreeGetPageLoad(bt, id);
  int min = divup(BTreePageGetN(bt,p), 2);
  assert(p->n < min);
  if(p->parent<0) return 1;
  BTreePage* par = BTreeGetPageLoad(bt, p->parent);
  int index=BTreePageGetIndexInParent(bt, p, par);
  int r=-1;
  BTreePage* sp = NULL; BTreePage* sl = NULL;
  if(index<par->n) {
    sp = BTreeGetPageLoad(bt, BTreeGetPointer(bt, par, index+1));
    if(sp->n > min) r=BTreePageLend(bt, par, p, sp, index);
  }
  if(r!=-1 && index>0) {
    sl = BTreeGetPageLoad(bt, BTreeGetPointer(bt, par, index-1));
    if(sl->n > min) r=BTreePageLend(bt, par, sl, p, index-1);
  }
  if(r!=1 && (sp!=NULL || sl!=NULL)) {
    if(sp!=NULL) r=BTreePageJoin(bt, par, p, sp, index);
    else { r=BTreePageJoin(bt, par, sl, p, index-1); p=sl; }

  }
  if(par->parent>=0 && par->n < divup(bt->_keyN,2))
    r=BTreePageAfterDelete(bt, p->parent);
  return r;
}

void BTreeCommit(BTree* bt) {
  int i; BTreePage* p;
  //kdyz volam tuto funkci, uz nepotrebuju mit v pameti zadne stranky, ktere se nezmenily
  if(bt->pagesOpened > bt->pagesOpenedMax && bt->pagesChanged < bt->pagesOpenedMax) {
    for(i=0; i<bt->pages->n; i++) {
      p = BTreeGetPage(bt, i);
      if(p && p->buf!=NULL) {
        if(p->change==0) { free(p->buf); p->buf=NULL; }
      }
    }
    bt->pagesOpened = bt->pagesChanged;
  }
  if(bt->pagesChanged>=bt->pagesOpenedMax) {
    #ifdef BTREE_LOG_VERBOSE
      BTreeLog(1,printf("BTREE COMMIT\n"));
    #endif
    for(i=0; i<bt->pages->n; i++) {
      p = BTreeGetPage(bt, i);
      if(p && p->buf!=NULL && p->change==1) { BTreePageSave(bt, p); p->change=0; }
    }
    fflush(bt->f);
    bt->pagesChanged = 0; bt->pagesOpened = 0;
  }
}

BTree* BTreeCreate(char* filename, BTreeCompareFunction cmpf, int pageSize, int keySize, int dataSize, int memMax) {
  BTree* bt = malloc(sizeof(BTree));
  bt->f = fopen(filename, "w+b");
  exitstr((bt->f),"Error: Nelze vytvorit B-strom!");
  bt->cmpf = cmpf;
  bt->pageSize = pageSize;
  bt->keySize = keySize;
  bt->dataSize = dataSize;
  bt->_keyN = (bt->pageSize-BPS)/(keySize+BPS);
  bt->_dataN = (bt->pageSize-BPS)/(keySize+dataSize);
  bt->_keyItemSize = bt->keySize + BPS;
  bt->_dataItemSize = bt->dataSize + keySize;
  bt->pages = ArrayCreate(2);
  bt->pagesOpened = 0; bt->pagesChanged = 0; 
  bt->pagesOpenedMax = memMax/(1024*1024)/bt->pageSize; //memMax je v MB
  bt->count = 0;
  bt->root = BTreePageCreate(bt, 0, -1);
  bt->leftmost = BTreePageCreate(bt, 1, bt->root);
  BTreePageInsert(bt, bt->root, NULL, NULL, bt->leftmost);
  BTreeCommit(bt);
  return bt;
}

void BTreeDestroy(BTree* bt) {
  int i; BTreePage* p;
  for(i=0; i<bt->pages->n; i++) {
    p = BTreeGetPage(bt, i);
    if(p) BTreePageDestroy(bt, p);
  }
  ArrayFree(bt->pages);
  fclose(bt->f);
  free(bt);
}

//debugovaci funkce
int BTreeCheck(BTree* bt, int root) {
  #ifdef BTREE_LOG_VERBOSE 
    BTreeLog(4,BTreeDebugPrintPage(bt, bt->root));
    BTreeLog(2,printf("BTREE CHECK: "));
  #endif
  int i;
  int* pages = malloc(sizeof(int)*bt->pages->n);
  for(i=0; i<bt->pages->n; i++) pages[i]=-1;
  int* z = malloc(sizeof(int)*bt->pages->n);
  z[0] = root;
  for(i=1; i<bt->pages->n; i++) z[i]=-1;
  int z_n = 1;
  //z, z_n .. zasobnik
  //pruchod do hloubky
  BTreePage* p; BTreePage* par; int nextLeaf=-1; int parInd=-1;
  void* lastValue = malloc(bt->keySize);
  while(z_n>0) {
    p = BTreeGetPageLoad(bt,z[z_n-1]); z_n--;
    if(p->parent>=0) { 
      par = BTreeGetPageLoad(bt,p->parent);
      parInd = BTreePageGetIndexInParent(bt, p, par);
      int c = BTreePageItemSize(bt,p);
      int cp = BTreePageItemSize(bt,par);
      if((parInd>0 && bt->cmpf(&p->buf[BPS], &par->buf[BPS+cp*(parInd-1)])==-1) ||
         (parInd<par->n && bt->cmpf(&p->buf[BPS+c*(p->n-1)], &par->buf[BPS+cp*parInd])==1)
        ) {
        printf("%i", p->id); BTreeDebugPrintPage(bt, bt->root);
        assert(0);
      }
    }
    #ifdef BTREE_LOG_VERBOSE
      BTreeLog(4,printf("%i ", p->id));
    #endif
    assertcmd(pages[p->id]==-1,
      {printf("%i,%i->%i",pages[p->id],p->parent,p->id); BTreeDebugPrintPage(bt, bt->root);}); //na zadnou stranku nesmi byt dvojity odkaz
    pages[p->id]=p->parent;
    if(p->type==1) {
      if(nextLeaf!=-1) 
        if(bt->cmpf(lastValue, &p->buf[BPS])==1) {
          printf("%i", p->id);
          BTreeDebugPrintPage(bt, bt->root);
          assert(0);
        }
      assert(nextLeaf==-1 || p->id==nextLeaf);
      nextLeaf = BTreeGetPointer(bt, p, 0);
    }
    for(i=0; i<p->n-1; i++) {
      int c = BTreePageItemSize(bt,p);
      if(bt->cmpf(&p->buf[BPS+c*i], &p->buf[BPS+c*(i+1)])==1) {
        printf("%i, %i", i, p->id);
        assert(0);
      }
      if(p->type==1) memcpy(lastValue, &p->buf[BPS+c*(p->n-1)], bt->keySize);
    }
     if(p->type==0) {
      
      for(i=p->n; i>=0; i--) {
        int n = BTreeGetPointer(bt, p, i);
        if(i==0 && n==-1 && p->parent==-1) break; //prazdny root
        z[z_n]=n; z_n++;
        assert(BTreeGetPage(bt, n)->parent==p->id);
      }
    }
  }
  if(root==bt->root)
    for(i=0; i<bt->pages->n; i++) assert(BTreeGetPage(bt,i)==NULL || i==bt->root || pages[i]>=0); //na vsechny stranky existuje odkaz?

  #ifdef BTREE_LOG_VERBOSE
    BTreeLog(2, printf("\n"));
  #endif
  free(pages); free(z); free(lastValue);
  return 1;
}

int BTreeGetCount(BTree* bt) {
  return bt->count;
}

void BTreeInsert(BTree* bt, void* key, void* data) {
  BTreePage* p = BTreeGetPage(bt, bt->root);
  int id=bt->root;
  while(p->type==0) {
    id = BTreePageKeySearch(bt, id, key);
    p = BTreeGetPage(bt, id);
  }
   #ifdef BTREE_LOG_VERBOSE
    BTreeLog(3, printf("BTREE INSERT %i\n", id));
  #endif
  if(BTreePageInsert(bt, id, key, data, -1)==-1) BTreePageSplit(bt, id, key, data, -1, -1);
  bt->count++;
  BTreeCommit(bt);
}

void* BTreeSearchDelete(BTree* bt, void* key, int del) {
  BTreePage* p = BTreeGetPage(bt, bt->root);
  int id=bt->root;
  while(p->type==0) {
    id = BTreePageKeySearch(bt, id, key);
    p = BTreeGetPage(bt, id);
  }
  void* x = BTreePageDataSearch(bt, id, key, del);
  if(del==1) {
    #ifdef BTREE_LOG_VERBOSE 
    BTreeLog(3, printf("BTREE DELETE\n"));
    #endif
    if(p->n < divup(bt->_dataN,2)) BTreePageAfterDelete(bt, id);
    bt->count--;
  }
  BTreeCommit(bt);
  return x;
}

void* BTreeSearch(BTree* bt, void* key) {
  return BTreeSearchDelete(bt, key, 0);
}

void** BTreePopN(BTree* bt, int n) {
  void** r = malloc(sizeof(void*)*n);
  memset(r, 0, sizeof(void*)*n);
  BTreePage* p = BTreeGetPageLoad(bt,bt->leftmost);
  assert(p->type==1);
  int i;
  if(n>0) {
    for(i=0; i<n; i++) {
      r[i] = BTreePageDelete(bt, p->id, 0);
      if(p->n < divup(bt->_dataN,2)) { BTreePageAfterDelete(bt, p->id); BTreeCommit(bt); }
      bt->count--;
    }
  }
  BTreeCommit(bt);
  bt->leftmost = p->id;
  return r;
}
