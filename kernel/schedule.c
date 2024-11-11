#include <types.h>
#include <param.h>
#include <spinlock.h>
#include <macro.h>
#include <list.h>
#include <proc.h>
#include <defs.h>
#include <schedule.h>

#define NEW_NODE(p) ({           \
        struct rbNode *node = kalloc(sizeof(struct rbNode), RB_MODE);    \
        ASSERT_INFO(node != NULL, "rbNode alloc fault");      \
        node->p = p;            \
        node->color = RED;          \
        node->left = NIL;        \
        node->right = NIL;   \
        node;                \
      })
#define DROP_NDOE(node)   ({\
        node->left = NULL;    \
        node->right = NULL;     \
        node->p = NULL;       \
        kfree(node, RB_MODE);      \
      })
#define sibling(node)   ({\
          struct rbNode *ret;   \
          if (node->parent == NIL)  \
            ret = NIL;      \
          else if (node->parent->left == node)  \
            ret = node->parent->right;    \
          else        \
            ret = node->parent->left;   \
          ret;    \
        })
#define set_sibling_red(node)   \
        struct rbNode *tmp = sibling(node); \
        if (tmp != NIL)     \
          tmp->color = RED;
#define has_red_child(node)   \
        (node->left != NIL && node->left->color == RED)   \
          ||  (node->right != NIL && node->right->color == RED)

static struct rbNode *rbRoot;
static struct rbNode *NIL;

static void left_rotate(struct rbNode *node) {
  struct rbNode *rnode = node->right;
  node->right = rnode->left;
  if (rnode->left != NIL) {
    rnode->left->parent = node;
  }
  if (node->parent == NULL) {
    rbRoot = rnode;
  } else if (node->parent->left == node) {
    node->parent->left = rnode;
  } else {
    node->parent->right = rnode;
  }

  node->parent = rnode;
  rnode->left = node;
}

static void right_rotate(struct rbNode *node) {
  struct rbNode *lnode = node->left;
  node->left = lnode->right;
  if (lnode->right != NIL) {
    lnode->right->parent = node;
  }

  if (node->parent == NULL) {
    rbRoot = lnode;
  } else if (node->parent->left == node) {
    node->parent->left = lnode;
  } else if (node->parent->right == node) {
    node->parent->right = lnode;
  }

  lnode->right = node;
  node->parent = lnode;
}

void fixed_insert(struct rbNode *node) {
  struct rbNode *unode = NULL;
  while (node != rbRoot && node->parent->color == RED) {
    if (node->parent == node->parent->parent->left) {
      unode = node->parent->parent->right;
      if (unode->color == RED) {
        unode->color = BLACK;
        node->parent->color = BLACK;
        unode->parent->color = RED;
        node = unode->parent;
      } else {
        if (node == node->parent->right) {
          node = node->parent;
          left_rotate(node);
        }
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        right_rotate(node->parent->parent);
      }
    } else {
      unode = node->parent->parent->right;
      if (unode->color == RED) {
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        unode->color = BLACK;
        node = node->parent->parent;
      } else {
        if (node == node->parent->left) {
          node = node->parent;
          right_rotate(node);
        }
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        left_rotate(node->parent->parent);
      }
    }
  }
  rbRoot->color = BLACK;
}

static void fixed_drop(struct rbNode *node) {
  while (node != rbRoot) {
    struct rbNode *sibling = sibling(node);
    struct rbNode *parent = node->parent;

    if (sibling == NIL) {
      node = parent;
      continue;
    } else {
      if (sibling->color == RED) {
        parent->color = RED;
        sibling->color = BLACK;
        if (sibling == sibling->parent->left)
          right_rotate(sibling);
        else
          left_rotate(sibling);
        continue;
      } else {
        if (has_red_child(sibling)) {
          if (sibling->left != NIL && sibling->left->color == RED) {
            if (sibling == sibling->parent->left) {
              sibling->left->color = sibling->color;
              sibling->color = parent->color;
              right_rotate(parent);
            } else {
              sibling->left->color = parent->color;
              right_rotate(sibling);
              left_rotate(parent);
            }
          } else {
            if (sibling == sibling->parent->left) {
              sibling->right->color = parent->color;
              left_rotate(sibling);
              right_rotate(parent);
            } else {
              sibling->right->color = sibling->color;
              sibling->color = parent->color;
              left_rotate(parent);
            }
          }
          parent->color = BLACK;
          break;
        } else {
          sibling->color = RED;
          if (parent->color == BLACK) {
            node = parent;
            continue;
          } else {
            parent->color = BLACK;
            break;
          }
        }
      }
    }
  }
}

static struct rbNode *search_heaper(struct proc *p) {
  struct rbNode *node = rbRoot;
  struct rbNode *ret = NULL;
  while (node != NIL) {
    if (node->p == p) {
      ret = node;
      break;
    } else if (node->p->vruntime > p->vruntime) {
      node = node->left;
      continue;
    } else {
      node = node->right;
      continue;
    }
  }
  return ret;
}

// 中序遍历-> 中 左 右
// 所以对于删除给定节点, 实际需要删除的节点
// 应该是从当前节点开始循环查找左子树
// 直到该节点的左节点是 NIL
// 然后交换两个节点的数据
// 在完成删除即可

static struct rbNode *successor(struct rbNode *node) {
  while (node->left != NIL)
    node = node->left;
  return node;
}

static void swap_node_info(struct rbNode *x, struct rbNode *y) {
  struct proc *p = x->p;
  x->p = y->p;
  y->p = p;
}

static struct rbNode *replace(struct rbNode *node) {
  if (node->left == NIL && node->right == NIL) {
    return NIL;
  } else if (node->left != NIL) {
    return node->left;
  } else if (node->right != NIL) {
    return node->right;
  } else {
    return successor(node);
  }
}

static void rb_drop_node(struct rbNode *node) {
  struct rbNode *replace_node = replace(node);
  bool double_black = ((replace_node == NIL || replace_node->color == BLACK) && (node->color == BLACK));
  struct rbNode *parent = node->parent;

  if (replace_node == NIL) {
    if (node == rbRoot) {
      node = NIL;
    } else {
      if (double_black) {
        fixed_drop(node);
      } else {
        set_sibling_red(node);
      }
      if (node == parent->left)
        parent->left = NIL;
      else
        parent->right = NIL;
    }
    DROP_NDOE(node);
    return;
  }

  if (node->left == NIL || node->right == NIL) {
    if (node == rbRoot) {
      node->p = replace_node->p;
      node->left = NIL;
      node->right = NIL;
      DROP_NDOE(replace_node);
    } else {
      if (node == parent->left)
        parent->left = replace_node;
      else
        parent->right = replace_node;
      DROP_NDOE(node);
      if (double_black) {
        fixed_drop(replace_node);
      } else {
        replace_node->color = BLACK;
      }
    }
    return;
  }
  swap_node_info(node, replace_node);
  rb_drop_node(replace_node);
}

void rb_push_back(struct proc *p) {
  struct rbNode *node = NEW_NODE(p);
  struct rbNode *parent = NULL;
  struct rbNode *current = rbRoot;

  while (current != NIL) {
    parent = current;
    if (node->p->vruntime > current->p->vruntime) {
      current = current->right;
    } else {
      current = current->left;
    }
  }

  node->parent = parent;
  if (parent == NULL)
    rbRoot = node;
  else if (parent->p->vruntime > node->p->vruntime)
    parent->left = node;
  else
    parent->right = node;

  if (node->parent == NULL) {
    node->color = BLACK;
    return;
  }
  if (node->parent->parent == NULL)
    return;

  fixed_insert(node);
}

void rb_pop_front(struct proc *p) {
  rb_drop_node(search_heaper(p));
}

void rb_init() {
  NIL = kalloc(sizeof (struct rbNode), RB_MODE);
  NIL->left = NIL;
  NIL->right = NIL;
  NIL->color = BLACK;
  NIL->p = NULL;
  NIL->parent = NULL;
  rbRoot = NIL;
}

void  yield() {
  // TODO
  return;
}

void schedule(void) {
  asm volatile("wfi");
}
