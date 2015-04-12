


#ifndef _T3_DLIST_
#define _T3_DLIST_

typedef struct dlist {
    void **data;
    int size;
    int __capacity__;
    int __lc__;
} dlist;

int dlist_add(dlist *d, void *data);
dlist *dlist_new();

#define lc(l) ((l)->__lc__)
#define dget(l, i) (((l)->data)[(i)])
#define size(l) ((l)->size)
#define cap(l) ((l)->__capacity__)
#define each(list, ptr) for (lc(list)=0, ptr=(size(list)==0)?NULL:dget(list, lc(list)); lc(list)<size(list); ptr=(dget(list, ++lc(list))))


#endif
