#include "fib_heap.h"

_node* new_node(int k,void* d){
  _node* ret = (_node*)malloc(sizeof(_node));
  ret->data = d;
  ret->key = k;

  ret->degree = 0;
  ret->mark = 0;

  ret->parent = ret->fc = ret->ns = ret->ps = ret->next = ret->prev = NULL;

  return ret;
}

fib_heap* new_heap(){
  fib_heap* ret = (fib_heap*)malloc(sizeof(fib_heap));
  ret->nodes = 0;
  ret->root_count = 0;
  ret->min_root = ret->roots = NULL;

  return ret;
}

void free_node(_node* n){
  if(n){
    free_node(n->fc);
    free_node(n->ns);
    free(n);
  }
}

void free_heap(fib_heap* h){
  _node* root = h->roots;
  int i;

  for(i=0;i<h->root_count;i++){
    _node* next = root->next;
    free_node(root);

    root = next;
  }

  free(h);
}

void add_node_to_roots(fib_heap* h,_node* n){
  if(h->roots == NULL){
    h->roots = n;
    h->min_root = n;

    n->prev = n->next = n;
  } else {
    _node* last_root = h->roots->prev;

    n->next = h->roots;
    n->prev = last_root;

    h->roots->prev = n;
    last_root->next = n;

    h->roots = n;

    if(h->min_root->key > n->key)
      h->min_root = n;
  }

  h->root_count++;
}

void remove_node_from_roots(fib_heap* h,_node* n){
  if(n == h->roots){
    if(n->next == n){
      h->roots = NULL;
    } else {
      h->roots = n->next;

      n->prev->next = n->next;
      n->next->prev = n->prev;
    }
  } else {
    n->prev->next = n->next;
    n->next->prev = n->prev;
  }

  n->next = n->prev = NULL;
  h->root_count--;
}

int is_root_node(_node* n){
  return n->parent == NULL;
}

void* find_min(fib_heap* h,int* k,_node** n){
  if(is_empty(h)){
    return NULL;
  }

  if(n)
    *n = h->min_root;
  if(k)
    *k = h->min_root->key;

  return h->min_root->data;
}

void insert(fib_heap* h,int k,void* d){
  _node* new_root = new_node(k,d);
  add_node_to_roots(h,new_root);

  h->nodes += 1;
}

void merge_heap(fib_heap* h1,fib_heap* h2){
  if(is_empty(h2)){
    return;
  }

  h1->nodes += h2->nodes;
  h2->nodes = 0;

  h1->root_count += h2->root_count;
  h2->root_count = 0;

  if(is_empty(h1)){
    h1->roots = h2->roots;
    h2->roots = NULL;

    h2->min_root = NULL;
  } else {
    _node* h1_last_root = h1->roots->prev;
    _node* h2_last_root = h2->roots->prev;

    h1_last_root->next = h2->roots;
    h2->roots->prev = h1_last_root;
    h2_last_root->next = h1->roots;
    h1->roots->prev = h2_last_root;

    if(h1->min_root > h2->min_root){
      h1->min_root = h2->min_root;
    }
    h2->min_root = NULL;
  }
}

int is_empty(fib_heap* h){
  return h->nodes == 0;
}

int size(fib_heap* h){
  return h->nodes;
}

void delete_node(fib_heap* h,_node* n){
  int key;
  find_min(h,&key,NULL);
  decrease_key(h,n,key-1);
  delete_min(h);
}

void delete_min(fib_heap* h){
  int i;

  if(is_empty(h)){
    return;
  }
  h->nodes -= 1;

  remove_node_from_roots(h,h->min_root);

  _node* child = h->min_root->fc;
  while(child){
    _node* nchild = child->ns;

    add_node_to_roots(h,child);
    child->parent = NULL;
    child->ns = child->ps = NULL;

    child = nchild;
  }

  int max_degree = h->root_count;
  _node** counts = (_node**)calloc(max_degree+1,sizeof(_node*));

  int nodes_count = h->root_count;
  _node** nodes_to_check = (_node**)malloc(nodes_count*sizeof(_node*));
  _node* root = h->roots;
  for(i=0;i<nodes_count;i++){
    nodes_to_check[i] = root;

    root = root->next;
  }

  for(i=0;i<nodes_count;i++){
    root = nodes_to_check[i];
    if(!is_root_node(root)){
      continue;
    }

    if(root->degree > max_degree){
      int new_max_degree = max_degree * 2;
      int j;

      if(root->degree > new_max_degree){
        new_max_degree = root->degree + 10;
      }

      counts = realloc(counts,(new_max_degree+1)*sizeof(_node*));
      for(j=max_degree+1;j<=new_max_degree;j++){
        counts[j] = NULL;
      }
    }

    if(counts[root->degree] == NULL){
      counts[root->degree] = root;
    } else {
      while(counts[root->degree] != NULL){
        _node* root2 = counts[root->degree];
        counts[root->degree] = NULL;
        remove_node_from_roots(h,root2);
        remove_node_from_roots(h,root);

        if(root->key < root2->key){
          root2->ns = root->fc;
          if(root->fc)
            root->fc->ps = root2;
          root->fc = root2;
          root2->parent = root;

          root->degree += 1;
        } else {
          root->ns = root2->fc;
          if(root2->fc)
            root2->fc->ps = root;
          root2->fc = root;
          root2->degree += 1;
          root->parent = root2;

          root = root2;
        }
        add_node_to_roots(h,root);

        if(root->degree > max_degree){
          int new_max_degree = max_degree * 2;
          int j;

          if(root->degree > new_max_degree){
            new_max_degree = root->degree + 10;
          }

          counts = realloc(counts,(new_max_degree+1)*sizeof(_node*));
          for(j=max_degree+1;j<=new_max_degree;j++){
            counts[j] = NULL;
          }
        }
      }

      counts[root->degree] = root;
    }
  }

  if(h->roots){
    h->min_root = h->roots;
    root = h->roots;
    do{
      if(root->key < h->min_root->key)
        h->min_root = root;

      root = root->next;
    } while(root != h->roots);
  } else {
    h->min_root = NULL;
  }

  free(counts);
  free(nodes_to_check);
}

void decrease_key(fib_heap* h,_node* n,int k){
  n->key = k;

  if(n->parent == NULL){
    if(n->key < h->min_root->key){
      h->min_root = n;
    }

    return;
  }
  if(k >= n->parent->key){
    return;
  }

  _node* nparent = n->parent;
  if(n->ps == NULL){ //first child
    if(n->ns)
      n->ns->ps = NULL;
    nparent->fc = n->ns;
  } else {
    n->ps->ns = n->ns;
    if(n->ns)
      n->ns->ps = n->ps;
  }
  n->parent = NULL;

  add_node_to_roots(h,n);
  n->mark = 0;
  nparent->mark++;

  _node* root = nparent;
  _node* rparent = root->parent;
  while(root->mark > 1){
    if(rparent == NULL){
      root->mark = 0;
      break;
    }

    if(root->ps == NULL){
      rparent->fc = root->ns;
      if(root->ns)
        root->ns->ps = NULL;
    } else {
      root->ps->ns = root->ns;
      if(root->ns)
        root->ns->ps = root->ps;
    }
    root->parent = NULL;

    add_node_to_roots(h,root);
    root->mark = 0;

    rparent->mark++;
    root = rparent;
    rparent = rparent->parent;
  }
}

void print_node(_node* root,int padding){
  int i;
  for(i=0;i<padding;i++){
    printf(" ");
  }
  printf("%d\n",root->key);

  _node* child = root->fc;
  while(child){
    print_node(child,padding+2);
    child = child->ns;
  }
}

void print_tree(_node* root){
  print_node(root,0);
}

void describe(fib_heap* h){
  _node* root = h->roots;
  if(root){
    do{
      print_tree(root);

      root = root->next;
    } while(root != h->roots);
  } else {
    printf("(EMPTY)\n");
  }
}
