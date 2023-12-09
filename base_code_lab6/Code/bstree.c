#include "bstree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

void bstree_remove_node(ptrBinarySearchTree *t, ptrBinarySearchTree current);

ptrBinarySearchTree fixredblack_insert(ptrBinarySearchTree x);

ptrBinarySearchTree fixredblack_remove(ptrBinarySearchTree p, ptrBinarySearchTree x);

/*------------------------  BSTreeType  -----------------------------*/

typedef enum {red, black} NodeColor;

struct _bstree {
    BinarySearchTree *parent;
    BinarySearchTree *left;
    BinarySearchTree *right;
    NodeColor color;
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
    
    t->color = red;
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

    // 

    BinarySearchTree* stop = fixredblack_insert(*p_cursor);

    if(bstree_empty(bstree_parent(stop)))
        *t = stop;
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

    // 

    NodeColor from_color = from->color;

    from->color = to->color;
    to->color = from_color;

    // 

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

    // 

    BinarySearchTree* substitute = current_child;

    if(current->color == black) {
        if(
            bstree_empty(substitute)
            || substitute->color == black
        ) {
            BinarySearchTree* subtreeroot = fixredblack_remove(bstree_parent(current), substitute);
            
            if(bstree_parent(subtreeroot) == NULL)
                *t = subtreeroot;
        } else
            substitute->color = black;
    }

    // 

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

/* ---------------------- RBTSpecific ----------------------*/

// 

ptrBinarySearchTree grandparent(ptrBinarySearchTree n) {
    if(bstree_empty(n)) return NULL;

    BinarySearchTree* n_parent = bstree_parent(n);
    if(bstree_empty(n_parent)) return NULL;

    return bstree_parent(n_parent);
}

// 

/**
 * @brief Return the brother of a given node by admitting
 * that the given parent node is effectively its parent.
 * 
 * @param n 
 * @return BinarySearchTree* 
 */
BinarySearchTree* brother_by_parent(BinarySearchTree* parent, BinarySearchTree* n) {
    if(bstree_empty(parent)) return NULL;
    
    if(n == bstree_left(parent))
        return bstree_right(parent);
    else if(n == bstree_right(parent))
        return bstree_left(parent);
    else
        return NULL;
}

/**
 * @brief Return the brother of a given node.
 * 
 * @param n 
 * @return BinarySearchTree* 
 */
BinarySearchTree* brother(BinarySearchTree* n) {
    if(bstree_empty(n)) return NULL;
    return brother_by_parent(bstree_parent(n), n);
}

// 

ptrBinarySearchTree uncle(ptrBinarySearchTree n) {
    if(bstree_empty(n)) return NULL;
    return brother(bstree_parent(n));
}

// 

void leftrotate(BinarySearchTree *x) {
    BinarySearchTree* y = bstree_right(x);

    if(bstree_empty(y)) return;

    BinarySearchTree* beta = bstree_left(y);

    // 
    
    BinarySearchTree* x_parent = bstree_parent(x);

    y->parent = x_parent;
    if(!bstree_empty(x_parent)) {
        if(x == bstree_left(x_parent))
            x_parent->left = y;
        else if(x == bstree_right(x_parent))
            x_parent->right = y;
    }

    x->parent = y;
    y->left = x;

    x->right = beta;
    if(!bstree_empty(beta)) beta->parent = x;
}

void rightrotate(BinarySearchTree *y) {
    BinarySearchTree* x = bstree_left(y);

    if(bstree_empty(x)) return;

    BinarySearchTree* beta = bstree_right(x);

    // 

    BinarySearchTree* y_parent = bstree_parent(y);

    x->parent = y_parent;
    if(!bstree_empty(y_parent)) {
        if(y == bstree_left(y_parent))
            y_parent->left = x;
        else if(y == bstree_right(y_parent))
            y_parent->right = x;
    }

    y->parent = x;
    x->right = y;

    y->left = beta;
    if(!bstree_empty(beta)) beta->parent = y;
}

// 

ptrBinarySearchTree fixredblack_insert_case2(ptrBinarySearchTree x) {
    BinarySearchTree* p = bstree_parent(x);
    BinarySearchTree* pp = bstree_parent(p);

    if(p == bstree_left(pp)) {
        if(x == bstree_right(p))
            leftrotate(p);

        rightrotate(pp);
    }
    else {
        if(x == bstree_left(p))
            rightrotate(p);

        leftrotate(pp);
    }

    BinarySearchTree* pp_parent = bstree_parent(pp);

    pp_parent->color = black;
    pp->color = red;

    return pp_parent;
}

ptrBinarySearchTree fixredblack_insert_case1(ptrBinarySearchTree x) {
    BinarySearchTree* f = uncle(x);

    if(!bstree_empty(f) && f->color == red) {
        BinarySearchTree* p = bstree_parent(x);
        BinarySearchTree* pp = bstree_parent(p);

        p->color = f->color = black;
        pp->color = red;

        return fixredblack_insert(pp);
    }
    else
        return fixredblack_insert_case2(x);
}

ptrBinarySearchTree fixredblack_insert_case0(ptrBinarySearchTree x) {
    BinarySearchTree* p = bstree_parent(x);

    if(bstree_empty(bstree_parent(p)))
        p->color = black;
    else
        return fixredblack_insert_case1(x);
    return x;
}

ptrBinarySearchTree fixredblack_insert(ptrBinarySearchTree x) {
    BinarySearchTree* p = bstree_parent(x);
    
    if(bstree_empty(p))
        x->color = black;
    else if(p->color == red)
        return fixredblack_insert_case0(x);
    
    return x;
}

// 

ptrBinarySearchTree fixredblack_remove_case1(ptrBinarySearchTree p, ptrBinarySearchTree x) {
    BinarySearchTree* f = brother_by_parent(p, x);
    if(bstree_empty(f)) return x;

    BinarySearchTree* f_left = bstree_left(f);
    BinarySearchTree* f_right = bstree_right(f);

    // 
    
    if(
        (bstree_empty(f_left) || f_left->color == black)
        && (bstree_empty(f_right) || f_right->color == black)
    ) {
        f->color = red;

        if(p->color == red) {
            p->color = black;
            return p;
        }
        
        return fixredblack_remove(bstree_parent(p), p);
    }
    else {
        BinarySearchTree* f_a = f_right;
        BinarySearchTree* f_b = f_left;

        void (*a_rotate)(BinarySearchTree* n) = leftrotate;
        void (*b_rotate)(BinarySearchTree* n) = rightrotate;

        if(x == bstree_right(p)) {
            f_a = f_left; f_b = f_right;
            a_rotate = rightrotate; b_rotate = leftrotate;
        }

        if(!bstree_empty(f_a) && f_a->color == red) {
            a_rotate(p);
            
            f->color = p->color;
            p->color = f_a->color = black;

            return f;
        }
        else {
            f_b->color = black;
            f->color = red;

            b_rotate(f);
            a_rotate(p);

            f->color = p->color;

            return bstree_parent(f);
        }
    }
}

ptrBinarySearchTree fixredblack_remove_case2(ptrBinarySearchTree p, ptrBinarySearchTree x) {
    BinarySearchTree* f;

    if(x == bstree_left(p)) {
        f = bstree_right(p);
        leftrotate(p);
    }
    else {
        f = bstree_left(p);
        rightrotate(p);
    }

    p->color = red;
    f->color = black;

    fixredblack_remove_case1(p, x);

    return f;
}

ptrBinarySearchTree fixredblack_remove(ptrBinarySearchTree p, ptrBinarySearchTree x) {
    if(bstree_empty(p)) return x;
    
    BinarySearchTree* f = brother_by_parent(p, x);

    if(f->color == black) return fixredblack_remove_case1(p, x);
    else return fixredblack_remove_case2(p, x);
}

// 

#define NODE_RED_BORDER_COLOR "#AA0000"
#define NODE_RED_BACKGROUND_COLOR "#FF0000"
#define NODE_RED_FONT_COLOR "#FFFFFF"

#define NODE_BLACK_BORDER_COLOR "#000000"
#define NODE_BLACK_BACKGROUND_COLOR "#666666"
#define NODE_BLACK_FONT_COLOR "#FFFFFF"

void printNode(const BinarySearchTree *n, void *out) {
    printf("%d ", bstree_root(n));
    
    fprintf(
        out,
        "\tn%d [label=\"{{<parent>}|%d|{<left>|<right>}}\", color=\"%s\", style=filled, fillcolor=\"%s\", fontcolor=\"%s\", fontname=\"Arial\"];\n",
        bstree_root(n),
        bstree_root(n),
        n->color == red ? NODE_RED_BORDER_COLOR : NODE_BLACK_BORDER_COLOR,
        n->color == red ? NODE_RED_BACKGROUND_COLOR : NODE_BLACK_BACKGROUND_COLOR,
        n->color == red ? NODE_RED_FONT_COLOR : NODE_BLACK_FONT_COLOR
    );

    if (bstree_left(n)) {
        fprintf(
            out,
            "\tn%d:left:c -> n%d:parent:c [headclip=false, tailclip=false]\n",
            bstree_root(n),
            bstree_root(bstree_left(n))
        );
    }
    else {
        fprintf(
            out,
            "\tlnil%d [label=\"NIL\", color=\"%s\", style=filled, fillcolor=\"%s\", fontcolor=\"%s\", fontname=\"Arial\"];\n",
            bstree_root(n),
            NODE_BLACK_BORDER_COLOR,
            NODE_BLACK_BACKGROUND_COLOR,
            NODE_BLACK_FONT_COLOR
        );
        fprintf(
            out,
            "\tn%d:left:c -> lnil%d:n [headclip=false, tailclip=false]\n",
            bstree_root(n),
            bstree_root(n)
        );
    }
    if (bstree_right(n)) {
        fprintf(
            out,
            "\tn%d:right:c -> n%d:parent:c [headclip=false, tailclip=false]\n",
            bstree_root(n),
            bstree_root(bstree_right(n))
        );
    }
    else {
        fprintf(
            out,
            "\trnil%d [label=\"NIL\", color=\"%s\", style=filled, fillcolor=\"%s\", fontcolor=\"%s\", fontname=\"Arial\"];\n",
            bstree_root(n),
            NODE_BLACK_BORDER_COLOR,
            NODE_BLACK_BACKGROUND_COLOR,
            NODE_BLACK_FONT_COLOR
        );
        fprintf(
            out,
            "\tn%d:right:c -> rnil%d:n [headclip=false, tailclip=false]\n",
            bstree_root(n),
            bstree_root(n)
        );
    }
}

void rbtree_export_dot(const BinarySearchTree *t, FILE *file) {
    fprintf(file , "digraph RedBlackTree {\n\tgraph[ranksep=0.5];\n\tnode[shape=record];\n\n");
    bstree_iterative_depth_infix(t, printNode , file);
    fprintf(file , "\n}\n");
}

void testrotateleft(BinarySearchTree *t) {
    leftrotate(t);
}

void testrotateright(BinarySearchTree *t) {
    rightrotate(t);
}