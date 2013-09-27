#include "fib_heap.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>

int count_node(_node* n){
  if(n == NULL) return 0;

  int ret = 1 + count_node(n->ns);
  ret += count_node(n->fc);

  return ret;
}

int count_nodes_in_heap(fib_heap* h){
  if(h->roots == NULL) return 0;

  _node* root = h->roots;
  int ret = 0;

  do{
    ret += count_node(root);
    root = root->next;
  }while(root != h->roots);

  return ret;
}

void check_node_heap_property(_node* n){
  if(n == NULL) return;
  _node* child = n->fc;
  while(child){
    assert(child->parent == n);
    assert(child->key >= n->key);

    check_node_heap_property(child);

    if(child->ns){
      assert(child->ns->ps == child);
    }
    if(child->ps){
      assert(child->ps->ns == child);
    }

    child = child->ns;
  }
}

void check_node_degree(_node* n){
  if(n == NULL) return;

  int degree = 0;
  _node* child = n->fc;
  while(child){
    degree++;
    check_node_degree(child);

    child = child->ns;
  }

  assert(degree == n->degree);
}

void check_heap(fib_heap* h){
  assert(h->nodes == count_nodes_in_heap(h));
  _node* root;

  if(h->roots == NULL){
    assert(h->root_count == 0);
    assert(h->min_root == NULL);
  } else {
    int rc = 0;
    root = h->roots;

    do{
      rc++;
      assert(h->min_root->key <= root->key);

      root = root->next;
    } while(root != h->roots);

    assert(rc == h->root_count);
  }

  //check for general heap property and degree
  if(h->roots != NULL){
    root = h->roots;
    do{
      check_node_heap_property(root);
      check_node_degree(root);

      root = root->next;
    } while(root != h->roots);
  }
}

void check_heap_after_delete_min(fib_heap* h){
  check_heap(h);

  //check that no two trees have the same degree
  int maxdegree = 0;
  _node* root = h->roots;
  do{
    if(root->degree > maxdegree)
      maxdegree = root->degree;

    root = root->next;
  } while(root != h->roots);

  int* count = (int*)calloc(maxdegree+1,sizeof(int));

  root = h->roots;
  do{
    assert(count[root->degree] == 0);
    count[root->degree]++;

    root = root->next;
  } while(root != h->roots);
}

int main(void){
  fib_heap* h = new_heap();
  int i,j;

  srand(time(NULL));

  for(i=0;i<10;i++){
    for(j=1;j<=4;j++){
      printf("insert and describe\n");
      int r = rand()%100 + 1;
      insert(h,r,(void*)r);
      check_heap(h);
      describe(h);
      printf("[%d]\n",size(h));
    }

    printf("delete_min and describe\n");
    int key;

    for(j=0;j<3;j++){
      if(find_min(h,&key,NULL)){
        printf("%d\n",key);
        delete_min(h);
        check_heap_after_delete_min(h);
        printf("tree\n");
        describe(h);
        printf("[%d]\n",size(h));
      }
    }

    printf("\n\n");
  }
}
