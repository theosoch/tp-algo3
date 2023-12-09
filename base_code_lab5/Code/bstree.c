#include "bstree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

void bstree_remove_node(ptrBinarySearchTree *t, ptrBinarySearchTree current);

/*------------------------  BSTreeType  -----------------------------*/

struct _bstree {
    BinarySearchTree *parent;
    BinarySearchTree *left;
    BinarySearchTree *right;
    int root;
};

/*------------------------  BaseBSTree  -----------------------------*/

BinarySearchTree *bstree_create() {
    return NULL;
}

/* This constructor is private so that we can maintain the oredring invariant on
 * nodes. The only way to add nodes to the tree is with the bstree_add function
 * that ensures the invariant.
 */
BinarySearchTree *bstree_cons(BinarySearchTree *left, BinarySearchTree *right, int root) {
    BinarySearchTree *t = malloc(sizeof(struct _bstree));
    
    t->parent = NULL;
    t->left = left;
    t->right = right;
    
    if (t->left != NULL) t->left->parent = t;
    if (t->right != NULL) t->right->parent = t;
    
    t->root = root;
    
    return t;
}

void bstree_delete_free_node(const BinarySearchTree* node, void* userData) {
    (void) userData;
    free((void*) node);
}

void bstree_delete(ptrBinarySearchTree *t) {
    bstree_depth_postfix(*t, bstree_delete_free_node, NULL);
    *t = NULL;

    // while(!bstree_empty(*t))
    //     bstree_remove_node(t, *t);
}

bool bstree_empty(const BinarySearchTree *t) {
    return t == NULL;
}

int bstree_root(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->root;
}

BinarySearchTree *bstree_left(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->left;
}

BinarySearchTree *bstree_right(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->right;
}

BinarySearchTree *bstree_parent(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->parent;
}

// 

/*
+------------+
| Extensions |
+------------+
*/

/**
 * @brief Search a node by its value in a given BinarySearchTree and return it.
 * 
 * @param t 
 * @param v 
 * @return BinarySearchTree* 
 */
BinarySearchTree* bstree_search_node(const BinarySearchTree *t, int v) {
    BinarySearchTree* cursor = (BinarySearchTree*) t;

    bool found = false;

    while(!bstree_empty(cursor) && !found) {
        int cursor_root = bstree_root(cursor);

        if(v < cursor_root)
            cursor = bstree_left(cursor);
        else if(cursor_root < v)
            cursor = bstree_right(cursor);
        else
            found = true;
    }

    return found ? cursor : NULL;
}

// 

typedef enum {
    left, right
} NearestDirection;

/**
 * @brief Search the nearest next node in a given direction (left or right).
 * 
 * @param x 
 * @param direction 
 * @return BinarySearchTree* 
 */
BinarySearchTree* bstree_nearest(const BinarySearchTree* x, NearestDirection direction) {
    BinarySearchTree* (*next)(const BinarySearchTree* node) = NULL;
    BinarySearchTree* (*previous)(const BinarySearchTree* node) = NULL;
    switch(direction) {
        case left:
            next = bstree_left;
            previous = bstree_right;
            break;
        case right:
            next = bstree_right;
            previous = bstree_left;
            break;
        default: break;
    }

    assert(next != NULL);
    assert(previous != NULL);
    
    // 

    BinarySearchTree* cursor = next(x);
    BinarySearchTree* cursor_next;

    if(!bstree_empty(cursor)) {
        while(!bstree_empty(cursor_next = previous(cursor)))
            cursor = cursor_next;
    }
    else {
        cursor_next = (BinarySearchTree*) x;
        cursor = bstree_parent(x);

        while(!bstree_empty(cursor) && cursor_next == next(cursor)) {
            cursor_next = cursor;
            cursor = bstree_parent(cursor);
        }
    }

    return cursor;
}

// 

/*------------------------  BSTreeDictionary  -----------------------------*/

/* Obligation de passer l'arbre par référence pour pouvoir le modifier */
void bstree_add(ptrBinarySearchTree *t, int v) {
    ptrBinarySearchTree* p_cursor = t;
    ptrBinarySearchTree cursor_parent = NULL;

    bool already_exists = false;

	while(!bstree_empty(*p_cursor) && !already_exists) {
        cursor_parent = *p_cursor;

        int cursor_root = bstree_root(*p_cursor);

        if(v < cursor_root)
            p_cursor = &(*p_cursor)->left;
        else if(cursor_root < v)
            p_cursor = &(*p_cursor)->right;
        else
            already_exists = true;
    }

    if(!already_exists) {
        *p_cursor = bstree_cons(bstree_create(), bstree_create(), v);
        (*p_cursor)->parent = cursor_parent;
    }
}

bool bstree_search(const BinarySearchTree *t, int v) {
    return bstree_search_node(t, v) != NULL;
}

BinarySearchTree *bstree_successor(const BinarySearchTree *x) {
    return bstree_nearest(x, right);
}

BinarySearchTree *bstree_predecessor(const BinarySearchTree *x) {
    return bstree_nearest(x, left);
}

void bstree_swap_nodes(ptrBinarySearchTree *tree, ptrBinarySearchTree from, ptrBinarySearchTree to) {
    assert(!bstree_empty(from));
    assert(!bstree_empty(to));

    // 

    if(from == to) return;

    BinarySearchTree* from_parent = bstree_parent(from);
    BinarySearchTree* to_parent = bstree_parent(to);

    from->parent = to_parent;
    to->parent = from_parent;
    
    from_parent = bstree_parent(from);
    to_parent = bstree_parent(to);

    if(!bstree_empty(from_parent)) {
        if(to == bstree_left(from_parent))
            from_parent->left = from;
        else if(to == bstree_right(from_parent))
            from_parent->right = from;
    }

    if(!bstree_empty(to_parent)) {
        if(from == bstree_left(to_parent))
            to_parent->left = to;
        else if(from == bstree_right(to_parent))
            to_parent->right = to;
    }

    // 

    BinarySearchTree* from_left = bstree_left(from);
    BinarySearchTree* from_right = bstree_right(from);

    BinarySearchTree* to_left = bstree_left(to);
    BinarySearchTree* to_right = bstree_right(to);
    
    from->left = to_left;
    from->right = to_right;

    to->left = from_left;
    to->right = from_right;

    from_left = bstree_left(from);
    from_right = bstree_right(from);

    to_left = bstree_left(to);
    to_right = bstree_right(to);

    if(!bstree_empty(from_left)) from_left->parent = from;
    if(!bstree_empty(from_right)) from_right->parent = from;
    if(!bstree_empty(to_left)) to_left->parent = to;
    if(!bstree_empty(to_right)) to_right->parent = to;

    // 

    if(*tree == from) *tree = to;
    else if(*tree == to) *tree = from;
}

// t -> the tree to remove from, current -> the node to remove
void bstree_remove_node(ptrBinarySearchTree *t, ptrBinarySearchTree current) {
    assert(!bstree_empty(current));

    // 

    if(
        !bstree_empty(bstree_left(current))
        && !bstree_empty(bstree_right(current))
    )
        bstree_swap_nodes(t, current, bstree_successor(current));
        // bstree_swap_nodes(t, current, bstree_predecessor(current));

    BinarySearchTree* current_parent = bstree_parent(current);
    BinarySearchTree* current_left = bstree_left(current);
    BinarySearchTree* current_right = bstree_right(current);

    BinarySearchTree* current_child = current_left;
    if(bstree_empty(current_left))
        current_child = current_right;

    if(!bstree_empty(current_child))
        current_child->parent = current_parent;

    if(!bstree_empty(current_parent)) {
        if(current == bstree_left(current_parent))
            current_parent->left = current_child;
        if(current == bstree_right(current_parent))
            current_parent->right = current_child;
    }
    else
        *t = current_child;

    free(current);
}

void bstree_remove(ptrBinarySearchTree *t, int v) {
    BinarySearchTree* to_remove = bstree_search_node(*t, v);

    if(to_remove != NULL) {
        bstree_remove_node(t, to_remove);
    }
}

/*------------------------  BSTreeVisitors  -----------------------------*/

void bstree_depth_prefix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    if(bstree_empty(t)) return;

    f(t, userData);
    bstree_depth_prefix(bstree_left(t), f, userData);
    bstree_depth_prefix(bstree_right(t), f, userData);
}

void bstree_depth_infix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    if(bstree_empty(t)) return;

    bstree_depth_infix(bstree_left(t), f, userData);
    f(t, userData);
    bstree_depth_infix(bstree_right(t), f, userData);
}

void bstree_depth_postfix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    if(bstree_empty(t)) return;

    bstree_depth_postfix(bstree_left(t), f, userData);
    bstree_depth_postfix(bstree_right(t), f, userData);
    f(t, userData);
}

void bstree_iterative_depth_infix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    BinarySearchTree* cursor = (BinarySearchTree*) t;
    BinarySearchTree* cursor_previous = NULL;
    BinarySearchTree* cursor_next;

    BinarySearchTree* cursor_parent;
    BinarySearchTree* cursor_left;
    BinarySearchTree* cursor_right;

    while(!bstree_empty(cursor)) {
        cursor_parent = bstree_parent(cursor);
        cursor_left = bstree_left(cursor);
        cursor_right = bstree_right(cursor);

        cursor_next = bstree_empty(cursor_right) ? cursor_parent : cursor_right;

        if(cursor_previous == cursor_parent) {
            if(bstree_empty(cursor_left))
                f(cursor, userData);
            else
                cursor_next = cursor_left;
        }
        else if(cursor_previous == cursor_left)
            f(cursor, userData);
        else if(cursor_previous == cursor_right)
            cursor_next = cursor_parent;

        cursor_previous = cursor;
        cursor = cursor_next;
    }
}

void bstree_iterative_breadth_prefix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    Queue* q = createQueue();
    queuePush(q, t);

    while(!queueEmpty(q)) {
        const BinarySearchTree* node = (const BinarySearchTree*) queueTop(q);
        q = queuePop(q);

        if(!bstree_empty(node)) {
            f(node, userData);

            queuePush(q, bstree_left(node));
            queuePush(q, bstree_right(node));
        }
    }

    deleteQueue(&q);
}

/*------------------------  BSTreeIterator  -----------------------------*/

// 

/*
+------------+
| Extensions |
+------------+
*/

typedef enum {
    minimum, maximum
} Extremum;

const BinarySearchTree *goto_extremum(const BinarySearchTree *e, Extremum extremum) {
    BinarySearchTree* (*next)(const BinarySearchTree* node) = NULL;
    switch(extremum) {
        case minimum:
            next = bstree_left;
            break;
        case maximum:
            next = bstree_right;
            break;
        default: break;
    }

    assert(next != NULL);

    BinarySearchTree* cursor = (BinarySearchTree*) e;
	BinarySearchTree* cursor_next;

    if(!bstree_empty(cursor)) {
        while(!bstree_empty(cursor_next = next(cursor)))
            cursor = cursor_next;
    }

	return cursor;
}

// 

struct _BSTreeIterator {
    /* the collection the iterator is attached to */
    const BinarySearchTree *collection;
    /* the first element according to the iterator direction */
    const BinarySearchTree *(*begin)(const BinarySearchTree *);
    /* the current element pointed by the iterator */
    const BinarySearchTree *current;
    /* function that goes to the next element according to the iterator direction */
    BinarySearchTree *(*next)(const BinarySearchTree *);
};

/* minimum element of the collection */
const BinarySearchTree *goto_min(const BinarySearchTree *e) {
	return goto_extremum(e, minimum);
}

/* maximum element of the collection */
const BinarySearchTree *goto_max(const BinarySearchTree *e) {
	return goto_extremum(e, maximum);
}

/* constructor */
BSTreeIterator *bstree_iterator_create(const BinarySearchTree *collection, IteratorDirection direction) {
	BSTreeIterator* iterator = malloc(sizeof(BSTreeIterator));

    iterator->collection = collection;

    iterator->begin = NULL;
    iterator->next = NULL;

    switch(direction) {
        case forward:
            iterator->begin = goto_min;
            iterator->next = bstree_successor; 
            break;
        case backward:
            iterator->begin = goto_max;
            iterator->next = bstree_predecessor; 
            break;
        default: break;
    }

    assert(iterator->begin != NULL);
    assert(iterator->next != NULL);

    iterator = bstree_iterator_begin(iterator);

	return iterator;
}

/* destructor */
void bstree_iterator_delete(ptrBSTreeIterator *i) {
    free(*i);
    *i = NULL;
}

BSTreeIterator *bstree_iterator_begin(BSTreeIterator *i) {
    i->current = i->begin(i->collection);
    return i;
}

bool bstree_iterator_end(const BSTreeIterator *i) {
    return i->current == NULL;
}

BSTreeIterator *bstree_iterator_next(BSTreeIterator *i) {
    i->current = i->next(i->current);
    return i;
}

const BinarySearchTree *bstree_iterator_value(const BSTreeIterator *i) {
    return i->current;
}