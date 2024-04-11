#include "rbtree.h"
#include "stdio.h"
#include <stdlib.h>
// 우회 회전 함수 정의
static void right_rotate(rbtree* t, node_t* n);
static void left_rotate(rbtree* t, node_t* n);

// 삽입 및 삭제 시 조정 함수 정의
static void insert_fixup(rbtree* T, node_t* n);
static void delete_fixup(rbtree* T, node_t* x);

// 후위 순회 삭제 및 대체 함수 정의
static void post_order_delete(rbtree* t, node_t* n);
static void transplant(rbtree* T, node_t* x, node_t* y);

// 중위 순회 함수 정의
static void inorder_tree(const rbtree *t, node_t* x, int *idx, key_t *arr, const size_t n);

// 선행자 찾기 함수 정의
static node_t* find_successor(rbtree* t, node_t* n);

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
  // 삽입할 노드
  node_t *new_node = (node_t*)calloc(1, sizeof(node_t));
  new_node->key = key;
  node_t *x = t->root;
  node_t *y = t->nil;

  // 삽입할 위치 탐색
  while(x != t->nil){
    y = x;
    if(x->key <= new_node->key){
      x = x->right;
    } else {
      x = x->left;
    }
  }
  // x = 삽입할 위치, y = 부모
  new_node->parent = y;
  if(y == t->nil){
    t->root = new_node;
  }else if(y->key > new_node->key){
    y->left = new_node;
  }else {
    y->right = new_node;
  }
  new_node->color = RBTREE_RED;
  new_node->left = t->nil;
  new_node->right = t->nil;

  // 삽입 후 RB_TREE 조정
  insert_fixup(t, new_node);

  return new_node;
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
  // 삭제할 노드의 자식이 없거나 오른쪽만 있을 때
  if(p->left == t->nil){
    x = p->right;
    transplant(t, p, p->right);
  // 삭제할 노드의 자식이 왼쪽만 있을 때
  }else if(p->right == t->nil){
    x = p->left;
    transplant(t, p, p->left);
  // 삭제할 노드의 자식이 둘 다 있을 때
  }else{
    // 후계자 탐색
    y = find_successor(t, y->right);
    y_original_color = y->color;
    x = y->right;
    // 후계자가 삭제할 노드의 자식이면
    if(y->parent == p){
      x->parent = y;
    }
    // 후계자가 삭제할 노드의 자식이 아니면 삭제할 노드로 y를 이동
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
  // 자식이 1개 이하면 삭제된 노드의 색깔, 2개면 후계자의 색깔이 블랙이면 트리 조정
  if(y_original_color == RBTREE_BLACK){
    delete_fixup(t, x);
  }
  free(p);
  return 0;
}

void insert_fixup(rbtree* T, node_t* n){
  //부모의 색깔이 빨강
  while(n->parent->color == RBTREE_RED){
    // 왼쪽 트리이면
    if(n->parent->parent->left == n->parent){
      node_t* u = n->parent->parent->right;
      //부모의 형제 노드도 빨강
      if(u->color == RBTREE_RED){
        n->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        //할아버지부터 다시 탐색
        n = n->parent->parent;
      // 부모의 형제 노드가 검정
      }else{
        // 꺾여있으면 좌회전으로 조정
        if(n->parent->right == n){
        n = n->parent;
        left_rotate(T, n);
      }
      // 우회전으로 트리 조정
      n->parent->color = RBTREE_BLACK;
      n->parent->parent->color = RBTREE_RED;
      right_rotate(T, n->parent->parent);
      }
    // 오른쪽 트리이면 
    }else{
      node_t* u = n->parent->parent->left;
      //부모의 형제 노드도 빨강
      if(u->color == RBTREE_RED){
        n->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        //할아버지부터 다시 탐색
        n = n->parent->parent;
      // 부모의 형제 노드가 검정
      }else{
        if(n->parent->left == n){
        n = n->parent;
        // 꺾여있으면 우회전으로 조정
        right_rotate(T, n);
      }
      // 좌회전으로 트리 조정
      n->parent->color = RBTREE_BLACK;
      n->parent->parent->color = RBTREE_RED;
      left_rotate(T, n->parent->parent);
      }
    }
  }
  T->root->color = RBTREE_BLACK;
}

void right_rotate(rbtree* t, node_t* n){
  node_t* child = n->left;
  n->left = child->right;
  if(child->right != t->nil){
    child->right->parent = n;
  }
  child->parent = n->parent;
  if(n->parent == t->nil){
    t->root = child;
  }else if(n->parent->left == n){
    n->parent->left = child;
  }else{
    n->parent->right = child;
  }
  child->right = n;
  n->parent = child;
}

void left_rotate(rbtree* t, node_t* n){
  node_t* child = n->right;

  n->right = child->left;
  if(child->left != t->nil){
    child->left->parent = n;
  }
  child->parent = n->parent;
  if(n->parent == t->nil){
    t->root = child;
  }else if(n->parent->left == n){
    n->parent->left = child;
  }else{
    n->parent->right = child;
  }
  child->left = n;
  n->parent = child;
}

// X노드와 Y노드의 위치를 바꾸는 함수
void transplant(rbtree* T, node_t* x, node_t* y){
  // X가 루트면 Y는 루트
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
  // x가 루트가 아니고, x가 더블 블랙일 때
  while (x != T->root && x->color == RBTREE_BLACK){
    // x가 왼쪽 자식
    if(x == x->parent->left){
      node_t* w = x->parent->right;
      // case1: x의 형제의 색깔이 빨강이면 색깔을 교환하고 회전 -> case 2,3,4로 이동
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(T, x->parent);
        w = x->parent->right;
      }
      // case2: 형제의 색깔이 블랙이고 자식이 모두 블랙일 때
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{
        // case 3: 형제의 왼쪽 자녀가 빨강이고 오른쪽 자녀가 블랙일 때 -> case 4로 이동
        if(w->right->color == RBTREE_BLACK){
        w->left->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        right_rotate(T, w);
        w = x->parent->right;
      }
      // case 4:형제의 오른쪽 자녀가 빨강일 때
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->right->color = RBTREE_BLACK;
      left_rotate(T, x->parent);
      x = T->root;
      } 
    // x가 왼쪽 자식
    }else{
      node_t* w = x->parent->left;
      // case1: x의 형제의 색깔이 빨강이면 색깔을 교환하고 회전 -> case 2,3,4로 이동
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(T, x->parent);
        w = x->parent->left;
      }
      // case2: 형제의 색깔이 블랙이고 자식이 모두 블랙일 때
      if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{
        // case 3: 형제의 왼쪽 자녀가 빨강이고 오른쪽 자녀가 블랙일 때 -> case 4로 이동
        if(w->left->color == RBTREE_BLACK){
        w->right->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        left_rotate(T, w);
        w = x->parent->left;
      }
      // case 4:형제의 오른쪽 자녀가 빨강일 때
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->left->color = RBTREE_BLACK;
      right_rotate(T, x->parent);
      x = T->root;
      } 
    }
  }
  // red and black이면 black으로 바꿔줌
  x->color = RBTREE_BLACK;
}

//후계자 탐색
node_t* find_successor(rbtree* t, node_t* n){
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

// 중위 순회로 배열 삽입
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
