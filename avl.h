#ifndef AVL_H
#define AVL_H

// DO NOT ADD OR REMOVE ANYTHING FROM THIS FILE

typedef enum Position {LEFT, RIGHT} Position;

typedef struct AVLNode * AVLNodePtr;

// Use only this struct definition.
typedef struct AVLNode{
    AVLNodePtr child[2]; // child[0] is left, child[1] is right.
    int key, height;
    int tree_size;
    int tree_sum;
} AVLNode;

int util_max(int x,int y);
int util_abs(int x);
// AVL Operations and Queries

// search for a node with key <x> in the and return the node if exists. Otherwise returns NULL.
AVLNodePtr avl_search(AVLNodePtr root,int x);

// insert a new node with key <x> into the subtree. If the key exists do nothing. Returns a pointer to root of the tree.
AVLNodePtr avl_insert(AVLNodePtr root,int x);

// delete the node with key <x> (if exists). Returns a pointer to the root of the tree.
AVLNodePtr avl_delete(AVLNodePtr root,int x);

// given roots of AVL trees T1 and T2 (root1 and root2 resp.), and a joint node <xnode>
// avl_merge merges the 2 trees into an avl tree.
// Returns a pointer to the root of the newly formed tree.
AVLNodePtr avl_merge(AVLNodePtr root1,AVLNodePtr root2);

// given roots of an AVL tree, and 2 keys.
// avl_LCA returns a pointer to the lowest common ancestor of x and y.
AVLNodePtr avl_LCA(AVLNodePtr root,int x,int y);

// given roots of an AVL tree, and 2 keys.
// avl_sum returns the sum of the keys in the tree, which are in [x,y].
int avl_sum(AVLNodePtr root,int x,int y);

// return a new initialized avl node with key <x>. Returns NULL if malloc fails.
AVLNodePtr new_avl_node( int x );

// Free all nodes in the tree.
void avl_delete_all (AVLNodePtr root);


#endif

