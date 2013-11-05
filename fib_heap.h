#ifndef __FIB_HEAP_H__

#include <stdlib.h>
#include <stdio.h>

typedef struct _node{
  void* data;
  int key;

  int degree,mark;
  struct _node *fc,*ns,*ps;
  struct _node *parent;

  //used in root nodes
  struct _node *next,*prev;
} _node;

typedef struct _fib_heap{
  int nodes;

  int root_count;
  _node* roots;

  _node* min_root;
} fib_heap;

fib_heap* new_heap();
void free_heap(fib_heap*);

void insert(fib_heap*,int,void*);
void* find_min(fib_heap*,int*,_node**);
void delete_min(fib_heap*);
void decrease_key(fib_heap*,_node*,int k);
void delete_node(fib_heap*,_node*);

void merge_heap(fib_heap*,fib_heap*);

int is_empty(fib_heap*);
int size(fib_heap*);

void describe(fib_heap*);

#endif
