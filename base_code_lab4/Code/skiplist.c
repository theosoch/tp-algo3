#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "skiplist.h"
#include "rng.h"

// 

typedef struct s_SkipListElement {
    int value;
    int level;
    struct s_SkipListElement** next_references;
    struct s_SkipListElement** back_references;
}* SkipListElement;

struct s_SkipList {
    unsigned int size;
    RNG rng;
    SkipListElement sentinel;
};

// 

/**
 * @brief Allows to get the level of a skiplist element.
 * Useful for the implementation.
 * 
 */
int skiplist_element_level(SkipListElement element) {
    return element->level;
}

/**
 * @brief Allows to get the level of a skiplist.
 * Useful for the implementation.
 * 
 */
int skiplist_level(SkipList l) {
    return skiplist_element_level(l->sentinel);
}

// 

SkipListElement skiplist_create_element(int value, int level) {
    SkipListElement element = malloc(sizeof(struct s_SkipListElement));

    // 

    element->value = value;
    element->level = level;

    // 

    element->next_references = malloc(element->level * sizeof(SkipListElement));
    element->back_references = malloc(element->level * sizeof(SkipListElement));

    // 

    return element;
}

SkipList skiplist_create(int nblevels) {
    assert(nblevels >= 1);

    SkipList l = malloc(sizeof(struct s_SkipList));

    l->size = 0;
    l->rng = rng_initialize(0);
    
    l->sentinel = skiplist_create_element(0, nblevels);

    for(int i = 0; i < nblevels; ++i) {
        l->sentinel->next_references[i] = l->sentinel;
        l->sentinel->back_references[i] = l->sentinel;
    }

    return l;
}

// 

SkipListElement skiplist_next_element(SkipListElement element, int level_index) {
    assert(0 <= level_index);
    assert(level_index < skiplist_element_level(element));
    
    return element->next_references[level_index];
}

SkipListElement skiplist_back_element(SkipListElement element, int level_index) {
    assert(0 <= level_index);
    assert(level_index < skiplist_element_level(element));
    
    return element->back_references[level_index];
}

// 

/**
 * @brief Deletes a given element of a skiplist.
 * Useful for the implementation.
 * 
 * @param element 
 */
void skiplist_delete_element(SkipListElement element) {
    free(element->next_references);
    free(element->back_references);
    free(element);
}

void skiplist_delete(SkipList d) {
    SkipListElement element = skiplist_next_element(d->sentinel, 0);
    
    while(element != d->sentinel) {
        SkipListElement element_next = skiplist_next_element(element, 0);
        skiplist_delete_element(element);
        element = element_next;
    }

    skiplist_delete_element(d->sentinel);

    free(d);
}

// 

unsigned int skiplist_size(SkipList l) {
    return l->size;
}

// 

int skiplist_ith(SkipList d, unsigned int i) {
    assert(i < skiplist_size(d));

    SkipListElement element = skiplist_next_element(d->sentinel, 0);

    while(i-- > 0) {
        element = skiplist_next_element(element, 0);
    }

    return element->value;
}

// 

void skiplist_map(SkipList d, ScanOperator f, void* user_data) {
    SkipListElement element = skiplist_next_element(d->sentinel, 0);

    while(element != d->sentinel) {
        f(element->value, user_data);
        element = skiplist_next_element(element, 0);
    }
}

// 

/**
 * @brief A functor to check if a skiplist_parkour should stop
 * and/or to do some useful operations when passing from a level to another
 * during the parkour.
 * 
 */
typedef bool (*SkipListParkourChecker)(
    int value,
    SkipListElement element,
    int level_index,
    void* userdata
);

/**
 * @brief Generic function to factorize the skiplist_parkour implementation.
 * 
 * @param d 
 * @param value 
 * @param checker 
 * @param userdata 
 * @param visit_counter 
 * @return SkipListElement 
 */
SkipListElement skiplist_parkour(
    SkipList d,
    int value,
    SkipListParkourChecker checker,
    void* userdata,
    unsigned int* visit_counter
) {
    int maxlevel = skiplist_level(d);

    bool checked = false;

    SkipListElement cursor = d->sentinel;
    if(visit_counter != NULL) *visit_counter = 1;

    for(int level_index = maxlevel-1; level_index > -1 && !checked; --level_index) {
        SkipListElement cursor_next;

        while(
            (cursor_next = skiplist_next_element(cursor, level_index)) != d->sentinel
            && cursor_next->value < value
        ) {
            cursor = cursor_next;
            if(visit_counter != NULL) (*visit_counter)++;
        }

        checked = checker(value, cursor, level_index, userdata);
    }

    return skiplist_next_element(cursor, 0);
}

// 

/**
 * @brief Used to store encountered back references
 * of an element during a skiplist_parkour.
 * 
 * @param value 
 * @param element 
 * @param level_index 
 * @param userdata 
 * @return true 
 * @return false 
 */
bool skiplist_update_checker(
    int value,
    SkipListElement element,
    int level_index,
    void* userdata
) {
    (void) value;

    SkipListElement* update = *((SkipListElement**) userdata);
    update[level_index] = element;

    return false;
}

// 

SkipList skiplist_insert(SkipList d, int value) {
    int maxlevel = skiplist_level(d);

    // Update table to store encountered future back references
    // of element to insert during the skiplist_parkour.
    SkipListElement update[maxlevel];
    // Necessary to avoid compiler error about a potential uninitialized value.
    SkipListElement* dyn_update = update;

    SkipListElement cursor = skiplist_parkour(
        d, value,
        skiplist_update_checker,
        &dyn_update, NULL
    );

    if(
        cursor == d->sentinel
        || cursor->value != value
    ) {
        int new_element_level = rng_get_value(&d->rng, maxlevel-1)+1;

        SkipListElement new_element = skiplist_create_element(
            value,
            new_element_level
        );

        for(int i = new_element_level-1; i > -1; --i) {
            new_element->next_references[i] = skiplist_next_element(update[i], i);
            skiplist_next_element(new_element, i)->back_references[i] = new_element;

            new_element->back_references[i] = update[i];
            skiplist_back_element(new_element, i)->next_references[i] = new_element;
        }

        d->size++;
    }

    return d;
}

// 

SkipList skiplist_remove(SkipList d, int value) {
    // Update table to store encountered back references
    // of searched element during the skiplist_parkour.
    SkipListElement update[skiplist_level(d)];
    // Necessary to avoid compiler error about a potential uninitialized value.
    SkipListElement* dyn_update = update;

    SkipListElement cursor = skiplist_parkour(
        d, value,
        skiplist_update_checker,
        &dyn_update, NULL
    );

    if(
        cursor != d->sentinel
        && cursor->value == value
    ) {
        for(int i = skiplist_element_level(cursor)-1; i > -1; --i) {
            update[i]->next_references[i] = skiplist_next_element(cursor, i);
            skiplist_next_element(update[i], i)->back_references[i] = update[i];
        }

        skiplist_delete_element(cursor);

        d->size--;
    }

    return d;
}

// 

/**
 * @brief Allows to acces to the necessary data
 * in the skiplist_search_found_checker.
 * 
 */
typedef struct s_SkiplistSearchCheckData {
    SkipList list;
    bool found;
} SkiplistSearchCheckData;

/**
 * @brief Used to check if searched element is find during a skiplist_parkour.
 * 
 * @param value 
 * @param cursor 
 * @param level_index 
 * @param userdata 
 * @return true 
 * @return false 
 */
bool skiplist_search_found_checker(
    int value,
    SkipListElement cursor,
    int level_index,
    void* userdata
) {
    SkiplistSearchCheckData* p_data = (SkiplistSearchCheckData*) userdata;
    
    SkipListElement cursor_next = skiplist_next_element(cursor, level_index);
    
    return (p_data->found = cursor_next != p_data->list->sentinel && cursor_next->value == value);
}

bool skiplist_search(SkipList d, int value, unsigned int *nb_operations) {
    SkiplistSearchCheckData data = {
        .list = d,
        .found = false
    };

    skiplist_parkour(d, value, skiplist_search_found_checker, &data, nb_operations);

    return data.found;
}

// 

/* ------------------------------ */

// 

/**
 * @brief Return the forward element of a given skiplist element.
 * Useful for the skiplist iterator internal implementation.
 * 
 * @param element 
 * @return SkipListElement 
 */
SkipListElement skiplist_iterator_forward(SkipListElement element) {
    return skiplist_next_element(element, 0);
}

/**
 * @brief Return the backward element of a given skiplist element.
 * Useful for the skiplist iterator internal implementation.
 * 
 * @param element 
 * @return SkipListElement 
 */
SkipListElement skiplist_iterator_backward(SkipListElement element) {
    return skiplist_back_element(element, 0);
}

// 

struct s_SkipListIterator {
    SkipList list;
    SkipListElement cursor;
    unsigned char direction;
    SkipListElement (*next)(SkipListElement element);
};

// 

SkipListIterator skiplist_iterator_create(SkipList d, unsigned char w) {
    SkipListIterator iterator = malloc(sizeof(struct s_SkipListIterator));

    iterator->list = d;
    iterator->cursor = skiplist_next_element(d->sentinel, 0);
    iterator->direction = w;
    if(w == FORWARD_ITERATOR) iterator->next = skiplist_iterator_forward;
    else if(w == BACKWARD_ITERATOR) iterator->next = skiplist_iterator_backward;

    return iterator;
}

// 

void skiplist_iterator_delete(SkipListIterator it) {
    free(it);
}

// 

SkipListIterator skiplist_iterator_begin(SkipListIterator it) {
    it->cursor = it->next(it->list->sentinel);
    return it;
}

// 

bool skiplist_iterator_end(SkipListIterator it) {
    return it->cursor == it->list->sentinel;
}

//

SkipListIterator skiplist_iterator_next(SkipListIterator it) {
    it->cursor = it->next(it->cursor);
    return it;
}

// 

int skiplist_iterator_value(SkipListIterator it) {
    assert(!skiplist_iterator_end(it));

    return it->cursor->value;
}

// 