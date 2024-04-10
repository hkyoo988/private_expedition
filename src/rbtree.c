#include "rbtree.h"
#include "stdio.h"
#include <stdlib.h>

static void right_rotate(rbtree* t, node_t* n);
static void left_rotate(rbtree* t, node_t* n);
static void insert_fixup(rbtree* T, node_t* n);
static void post_order_delete(rbtree* t, node_t* n);
static void transplant(rbtree* T, node_t* x, node_t* y);
static void delete_fixup(rbtree* T, node_t* x);
static void inorder_tree(const rbtree *t, node_t* x, int *idx, key_t *arr, const size_t n);
node_t* find_succ(rbtree* t, node_t* n);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  p->nil = p->root = nil;
  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  post_order_delete(t, t->root);
  free(t->nil);
  free(t);
}

void post_order_delete(rbtree* t, node_t* n){
  if(n == t->nil){
    return;
  }
  post_order_delete(t, n->left);
  post_order_delete(t, n->right);
  free(n);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *z = (node_t*)calloc(1, sizeof(node_t));
  z->key = key;
  node_t *x = t->root;
  node_t *y = t->nil;

  while(x != t->nil){
    y = x;
    if(x->key <= z->key){
      x = x->right;
    } else {
      x = x->left;
    }
  }
  z->parent = y;
  if(y == t->nil){
    t->root = z;
  }else if(y->key > z->key){
    y->left = z;
  }else {
    y->right = z;
  }
  z->color = RBTREE_RED;
  z->left = t->nil;
  z->right = t->nil;

  insert_fixup(t, z);

  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t* p = t->root;
  while(p != t->nil){
    if(p->key < key){
      p = p->right;
    }else if(p->key > key){
      p = p->left;
    }else{
      return p;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t* n = t->root;
  while(n->left != t->nil){
    n = n->left;
  }
  return n;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t* n = t->root;
  while(n->right != t->nil){
    n = n->right;
  }
  return n;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  node_t* y = p;
  color_t y_original_color = y->color;
  node_t* x;
  if(p->left == t->nil){
    x = p->right;
    transplant(t, p, p->right);
  }else if(p->right == t->nil){
    x = p->left;
    transplant(t, p, p->left);
  }else{
    y = find_succ(t, y->right);
    y_original_color = y->color;
    x = y->right;
    if(y->parent == p){
      x->parent = y;
    }
    if(y->parent != p) {
      transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if(y_original_color == RBTREE_BLACK){
    delete_fixup(t, x);
  }
  free(p);
  return 0;
}

void insert_fixup(rbtree* T, node_t* n){
  //부모의 색깔이 빨강
  while(n->parent->color == RBTREE_RED){
    //부모의 형제 노드도 빨강
    if(n->parent->parent->left == n->parent){
      node_t* u = n->parent->parent->right;
      if(u->color == RBTREE_RED){
        n->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        //할아버지부터 다시 탐색
        n = n->parent->parent;
      }else{
        if(n->parent->right == n){
        n = n->parent;
        left_rotate(T, n);
      }
      n->parent->color = RBTREE_BLACK;
      n->parent->parent->color = RBTREE_RED;
      right_rotate(T, n->parent->parent);
      } 
    }else{
      node_t* u = n->parent->parent->left;
      if(u->color == RBTREE_RED){
        n->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        n = n->parent->parent;
      }else{
        if(n->parent->left == n){
        n = n->parent;
        right_rotate(T, n);
      }
      n->parent->color = RBTREE_BLACK;
      n->parent->parent->color = RBTREE_RED;
      left_rotate(T, n->parent->parent);
      }
    }
  }
  T->root->color = RBTREE_BLACK;
}

void right_rotate(rbtree* t, node_t* n){
  node_t* c = n->left;
  n->left = c->right;
  if(c->right != t->nil){
    c->right->parent = n;
  }
  c->parent = n->parent;
  if(n->parent == t->nil){
    t->root = c;
  }else if(n->parent->left == n){
    n->parent->left = c;
  }else{
    n->parent->right = c;
  }
  c->right = n;
  n->parent = c;
}

void left_rotate(rbtree* t, node_t* n){
  node_t* c = n->right;

  n->right = c->left;
  if(c->left != t->nil){
    c->left->parent = n;
  }
  c->parent = n->parent;
  if(n->parent == t->nil){
    t->root = c;
  }else if(n->parent->left == n){
    n->parent->left = c;
  }else{
    n->parent->right = c;
  }
  c->left = n;
  n->parent = c;
}

void transplant(rbtree* T, node_t* x, node_t* y){
  if(x->parent == T->nil){
    T->root = y;
  }else if(x->parent->left == x){
    x->parent->left = y;
  }else{
    x->parent->right = y;
  }
  y->parent = x->parent;
}

void delete_fixup(rbtree* T, node_t* x){
  while (x != T->root && x->color == RBTREE_BLACK){
    if(x == x->parent->left){
      node_t* w = x->parent->right;
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(T, x->parent);
        w = x->parent->right;
      }
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{
        if(w->right->color == RBTREE_BLACK){
        w->left->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        right_rotate(T, w);
        w = x->parent->right;
      }
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->right->color = RBTREE_BLACK;
      left_rotate(T, x->parent);
      x = T->root;
      } 
    }else{
      node_t* w = x->parent->left;
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(T, x->parent);
        w = x->parent->left;
      }
      if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{
        if(w->left->color == RBTREE_BLACK){
        w->right->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        left_rotate(T, w);
        w = x->parent->left;
      }
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->left->color = RBTREE_BLACK;
      right_rotate(T, x->parent);
      x = T->root;
      } 
    }
  }
  x->color = RBTREE_BLACK;
}

node_t* find_succ(rbtree* t, node_t* n){
  while(n->left != t->nil){
    n = n->left;
  }
  return n;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  node_t* x = t->root;
  int cnt = 0;
  int* idx = &cnt;
  inorder_tree(t, x, idx, arr, n);
  return 0;
}

void inorder_tree(const rbtree *t, node_t* x, int *idx, key_t *arr, const size_t n){
  if(x==t->nil){
    return;
  }
  inorder_tree(t, x->left, idx, arr, n);
  if(*idx<n){
    arr[(*idx)++] = x->key;
  }else{
    return;
  }
  inorder_tree(t, x->right, idx, arr, n);
  return;
}