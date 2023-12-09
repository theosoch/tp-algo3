/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)
 
 Implantation du TAD List vu en cours.
 */
/*-----------------------------------------------------------------*/
#include <stdio.h>

#include <time.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

typedef struct s_LinkedElement {
	int value;
	struct s_LinkedElement *previous;
	struct s_LinkedElement *next;
} LinkedElement;

/* Use of a sentinel for implementing the list :
 The sentinel is a LinkedElement * whose next pointer refer always to the head of the list and previous pointer to the tail of the list
 */
struct s_List {
	LinkedElement *sentinel;
	int size;
};


/*-----------------------------------------------------------------*/

List *list_create() {
	List* l = malloc(sizeof(struct s_List));

	l->sentinel = malloc(sizeof(struct s_LinkedElement));
	
	l->sentinel->next = l->sentinel;
	l->sentinel->previous = l->sentinel;
	l->sentinel->value = 0;

	l->size = 0;

	// 

	return l;
}

/*-----------------------------------------------------------------*/

List *list_push_back(List *l, int v) {
	LinkedElement* element = malloc(sizeof(struct s_LinkedElement));
	LinkedElement* last_element = l->sentinel->previous;

	element->value = v;

	element->previous = last_element;
	last_element->next = element;

	element->next = l->sentinel;
	l->sentinel->previous = element;

	l->size++;

	// 

	return l;
}

/*-----------------------------------------------------------------*/

void list_delete(ptrList *l) {
	while(!list_is_empty(*l)) {
		list_pop_back(*l);
	}

	free((*l)->sentinel);
	free(*l);

	*l = NULL;
}

/*-----------------------------------------------------------------*/

List *list_push_front(List *l, int v) {
	LinkedElement* element = malloc(sizeof(struct s_LinkedElement));
	LinkedElement* first_element = l->sentinel->next;

	element->value = v;

	element->next = first_element;
	first_element->previous = element;

	element->previous = l->sentinel;
	l->sentinel->next = element;

	l->size++;

	// 

	return l;
}

/*-----------------------------------------------------------------*/

int list_front(List *l) {
	assert(!list_is_empty(l));
	return l->sentinel->next->value;
}

/*-----------------------------------------------------------------*/

int list_back(List *l) {
	assert(!list_is_empty(l));
	return l->sentinel->previous->value;
}

/*-----------------------------------------------------------------*/

List *list_pop_front(List *l) {
	assert(!list_is_empty(l));

	// 

	LinkedElement* first_element = l->sentinel->next;

	first_element->next->previous = first_element->previous;
	first_element->previous->next = first_element->next;

	free(first_element);

	l->size--;

	// 

	return l;
}

/*-----------------------------------------------------------------*/

List *list_pop_back(List *l){
	assert(!list_is_empty(l));

	// 

	LinkedElement* last_element = l->sentinel->previous;

	last_element->previous->next = last_element->next;
	last_element->next->previous = last_element->previous;

	free(last_element);

	l->size--;

	// 

	return l;
}

/*-----------------------------------------------------------------*/

List *list_insert_at(List *l, int p, int v) {
	assert(0 <= p && p <= list_size(l));

	// 

	LinkedElement* previous_element = l->sentinel;

	for(int i = p; i > 0; --i)
		previous_element = previous_element->next;

	LinkedElement* element = malloc(sizeof(struct s_LinkedElement));
	element->value = v;

	element->previous = previous_element;
	element->next = previous_element->next;

	previous_element->next = element;
	element->next->previous = element;

	l->size++;

	// 

	return l;
}

/*-----------------------------------------------------------------*/

List *list_remove_at(List *l, int p) {
	assert(0 <= p && p < list_size(l));

	// 

	LinkedElement* element_to_remove = l->sentinel->next;

	for(int i = p; i > 0; --i)
		element_to_remove = element_to_remove->next;

	element_to_remove->previous->next = element_to_remove->next;
	element_to_remove->next->previous = element_to_remove->previous;

	free(element_to_remove);

	l->size--;

	// 
		
	return l;
}

/*-----------------------------------------------------------------*/

int list_at(List *l, int p) {
	assert(0 <= p && p < l->size);

	// 

	LinkedElement* element = l->sentinel->next;

	for(int i = p; i > 0; --i)
		element = element->next;

	// 

	return element->value;
}

/*-----------------------------------------------------------------*/

bool list_is_empty(List *l) {
	return l->sentinel->next == l->sentinel && l->sentinel->previous == l->sentinel;
}

/*-----------------------------------------------------------------*/

int list_size(List *l) {
	return l->size;
}

/*-----------------------------------------------------------------*/

List * list_map(List *l, SimpleFunctor f) {
	LinkedElement* element = l->sentinel->next;

	while(element != l->sentinel) {
		element->value = f(element->value);
		element = element->next;
	}

	// 

	return l;
}


List *list_reduce(List *l, ReduceFunctor f, void *userData) {
	LinkedElement* element = l->sentinel->next;

	while(element != l->sentinel) {
		f(element->value, userData);
		element = element->next;
	}

	return l;
}

/*-----------------------------------------------------------------*/

struct s_SubList {
	LinkedElement* first;
	LinkedElement* last;
};

typedef struct s_SubList SubList;

// 

/**
 * @brief Prints content of a list in the given out-stream.
 * 
 * @param sl 
 */
void fprint_list(FILE* outstream, List* l) {
	int lsize = list_size(l);

	fprintf(outstream, "[");
	for(int i = 0; i < lsize-1; ++i) {
		fprintf(outstream, "%d, ", list_at(l, i));
	}

	if(lsize != 0) fprintf(outstream, "%d", list_at(l, lsize-1));
	fprintf(outstream, "]\n");
}

// 

/**
 * @brief Prints content of a list in stdout.
 * 
 * @param l 
 */
void print_list(List* l) {
	fprint_list(stdout, l);
}

// 

/**
 * @brief Prints content of a sublist in the given out-stream.
 * 
 * @param sl 
 */
void fprint_sublist(FILE* outstream, SubList* sl) {
	LinkedElement* cursor = sl->first;

	fprintf(outstream, "[");
	while(cursor != NULL) {
		if(cursor->next == NULL) fprintf(outstream, "%d", cursor->value);
		else fprintf(outstream, "%d, ", cursor->value);
		cursor = cursor->next;
	}
	fprintf(outstream, "]\n");
}

// 

/**
 * @brief Prints content of a sublist in the stdout.
 * 
 * @param sl 
 */
void print_sublist(SubList* sl) {
	fprint_sublist(stdout, sl);
}

// 

/**
 * @brief Unlink elements from a given list to link them to
 * a new sublist and return it.
 * 
 * @param l 
 * @return SubList 
 */
SubList extract_sublist(List* l) {
	SubList result;

	result.first = l->sentinel->next == l->sentinel ? NULL : l->sentinel->next;
	result.last = l->sentinel->previous == l->sentinel ? NULL : l->sentinel->previous;

	if(result.first != NULL) result.first->previous = NULL;
	if(result.last != NULL) result.last->next = NULL;

	l->sentinel->next = l->sentinel;
	l->sentinel->previous = l->sentinel;

	return result;
}

// 

/**
 * @brief Reconstruct the linking of a given list from a sublist.
 * 
 * @param l 
 * @param sl 
 * @return List* 
 */
List* reconstruct_list_linking_from_sublist(List* l, SubList* sl) {
	l->sentinel->next = sl->first == NULL ? l->sentinel : sl->first;
	l->sentinel->next->previous = l->sentinel;

	l->sentinel->previous = sl->last == NULL ? l->sentinel : sl->last;
	l->sentinel->previous->next = l->sentinel;

	return l;
}

// 

/**
 * @brief Complementary structure used to store result
 * of the function reconstruct_sublists_from_split
 * 
 */
typedef struct s_SubListSplit {
	SubList left;
	SubList right;
} SubListSplit;

/**
 * @brief Can reconstrct two independent sublists from the result
 * of the function list_split.
 * 
 * @param list_split_result 
 * @return SubListSplit 
 */
SubListSplit reconstruct_sublists_from_split(SubList list_split_result) {
	SubListSplit result;

	result.left.first = NULL;
	result.left.last = NULL;

	result.right.first = NULL;
	result.right.last = NULL;

	if(list_split_result.first != NULL) {
		result.left.last = list_split_result.first;

		result.left.first = result.left.last;
		while(result.left.first != NULL && result.left.first->previous != NULL)
			result.left.first = result.left.first->previous;
	}

	if(list_split_result.last != NULL) {
		result.right.first = list_split_result.last;

		result.right.last = result.right.first;
		while(result.right.last != NULL && result.right.last->next != NULL)
			result.right.last = result.right.last->next;
	}

	return result;
}

// 

/**
 * @brief Free memory used by a sublist.
 * 
 * @param sl The sublist to free.
 */
void delete_sublist(SubList* sl) {
	LinkedElement* cursor = sl->first;

	while(cursor != NULL) {
		LinkedElement* cursor_next = cursor->next;
		free(cursor);
		cursor = cursor_next;
	}

	sl->first = NULL;
	sl->last = NULL;
}

// 

SubList list_split(SubList sl) {
	SubList result;

	result.first = NULL;
	result.last = NULL;

	// 

	LinkedElement* forward = sl.first;
	LinkedElement* backward = sl.last;

	while(
		(forward != NULL && backward != NULL)
		&& forward != backward
		&& forward->previous != backward
		&& backward->next != forward
	) {
		forward = forward->next;
		backward = backward->previous;
	}
	
	if(forward != NULL && backward != NULL) {
		if(forward == backward)
			forward = forward->next;

		result.first = backward;
		if(result.first != NULL) result.first->next = NULL;
		result.last = forward;
		if(result.last != NULL) result.last->previous = NULL;
	}

	// 

	return result;
}

// 

SubList list_merge(SubList leftlist, SubList rightlist, OrderFunctor f) {
	LinkedElement* left_cursor = leftlist.first;
	LinkedElement* right_cursor = rightlist.first;

	if(left_cursor == NULL) return rightlist;
	if(right_cursor == NULL) return leftlist;
	
	SubList merged = {
		.first = NULL,
		.last = NULL
	};

	// 

	LinkedElement* tmp = malloc(sizeof(struct s_LinkedElement));

	tmp->previous = NULL;
	tmp->next = NULL;
	tmp->value = 0;

	// 

	LinkedElement* merged_cursor = tmp;

	while(left_cursor != NULL || right_cursor != NULL) {
		LinkedElement** p_element_to_merge = NULL;
		LinkedElement* element_to_merge_next = NULL;

		if(left_cursor == NULL) {
			p_element_to_merge = &right_cursor;
		}
		else if(right_cursor == NULL) {
			p_element_to_merge = &left_cursor;
		}
		else if(f(left_cursor->value, right_cursor->value)) {
			p_element_to_merge = &left_cursor;
		}
		else {
			p_element_to_merge = &right_cursor;
		}

		merged_cursor->next = *p_element_to_merge;
		(*p_element_to_merge)->previous = merged_cursor;
		
		element_to_merge_next = (*p_element_to_merge)->next;
		if(element_to_merge_next != NULL) element_to_merge_next->previous = NULL;
		(*p_element_to_merge)->next = NULL;

		*p_element_to_merge = element_to_merge_next;

		merged_cursor = merged_cursor->next;
	}

	merged.first = tmp->next;
	merged.last = merged_cursor;

	free(tmp);

	return merged;
}

// 

SubList list_mergesort(SubList sl, OrderFunctor f) {
	if(sl.first != sl.last) {
		SubListSplit sublists = reconstruct_sublists_from_split(list_split(sl));

		SubList sl_merged_list_left = list_mergesort(sublists.left, f);
		SubList sl_merged_list_right = list_mergesort(sublists.right, f);

		SubList sl_merged = list_merge(sl_merged_list_left, sl_merged_list_right, f);

		return sl_merged;
	}

	return sl;
}

// 

List *list_sort(List *l, OrderFunctor f) {
	SubList sl_sorted = list_mergesort(extract_sublist(l), f);
	return reconstruct_list_linking_from_sublist(l, &sl_sorted);
}

// 

/* +---------------------+
 * | COMPLEMENTARY TESTS |
 * +---------------------+
 */

// 

/**
 * @brief An OrderFunctor to use to sort lists.
 * 
 * @param current 
 * @param next 
 * @return true 
 * @return false 
 */
bool test_comparison_1(int current, int next) { return current < next; }

/**
 * @brief The OrderFunctor to use to check if a list is
 * correctly sorted by the relation of the OrderFunctor test_comparison_1
 * 
 * @param current 
 * @param next 
 * @return true 
 * @return false 
 */
bool test_comparison_2(int current, int next) { return current <= next; }

// 

/**
 * @brief Test if a l is sorted in a specified order.
 * 
 * @param l The list to test
 * @param f The function representing the order relation
 * @return true 
 * @return false 
 */
bool test_is_sorted(List* l, OrderFunctor f) {
	if(list_size(l) > 1) {
		bool success;

		for(
			int i = 0;
			(
				i < list_size(l)-1
				&& ( success = f(list_at(l, i), list_at(l, i+1)) )
			);
			++i
		);

		return success;
	}

	return true;
}

// 

#define PART1_TEST_COUNT 5
#define PART1_MAX_L_SIZE 10

#define PART3_TEST_COUNT 10000
#define PART3_LIST_TO_SORT_MAX_SIZE 20

/**
 * @brief Some tests on the merge-sort implementation.
 * 
 */
void list_tests() {
	srand(time(NULL));

	// 

	printf("\x1b[1m-=< PART 1\n\x1b[0m");

	// 

	for(int test_i = 0; test_i < PART1_TEST_COUNT; ++test_i) {
		printf("\x1b[1m-=| 1.%d\n\x1b[0m", test_i+1);

		// 

		List* l = list_create();

		printf("# Initial list l content :\n");
		print_list(l);

		int l_size;
		switch (test_i) {
			case 0: l_size = 0; break;
			case 1: l_size = 1; break;
			default:
				l_size = ((rand() % (PART1_MAX_L_SIZE)) + 1);
				if(l_size % 2 == 0 && (test_i+1) % 2 == 1)
					l_size += l_size < PART1_MAX_L_SIZE ? 1 : -1;
				break;
		}

		for(int i = 0; i < l_size; ++i) {
			list_push_back(l, i+1);
		}

		printf("# List l content after filling :\n");
		print_list(l);

		SubList sl = extract_sublist(l);
		
		printf("# Extracted sublist sl content :\n");
		print_sublist(&sl);

		printf("# Check if sublist members (first & last) are NULL :\n");
		printf("%d ; %d \n", sl.first == NULL, sl.last == NULL);

		SubList list_split_result = list_split(sl);

		printf("# Element value from the `first` member of list_split result :\n");
		if(list_split_result.first != NULL)
			printf("| `%d`\n", list_split_result.first->value);
		else printf("| <none>\n");

		printf("# Element value from the `last` member of list_split result :\n");
		if(list_split_result.last != NULL)
			printf("| `%d`\n", list_split_result.last->value);
		else printf("| <none>\n");

		SubListSplit sublists = reconstruct_sublists_from_split(list_split_result);
		
		printf("# Content of left sublist reconstructed from list_split result :\n");
		print_sublist(&sublists.left);
		printf("# Content of right sublist reconstructed from list_split result :\n");
		print_sublist(&sublists.right);

		SubList merged = list_merge(sublists.left, sublists.right, test_comparison_1);

		printf("# Sublist merged content :\n");
		print_sublist(&merged);

		reconstruct_list_linking_from_sublist(l, &merged);

		printf("# List l content after linking reconstruction from sublist merged :\n");
		print_list(l);

		list_delete(&l);
	}

	// 

	printf("\x1b[1m-=< PART 2\n\x1b[0m");

	// 

	List* l1 = list_create();
	List* l2 = list_create();

	printf("# Initial list l1 content :\n");
	print_list(l1);
	printf("# Initial list l2 content :\n");
	print_list(l2);

	for(int i = 0; i < 4; ++i) {
		list_push_back(l1, 2*i);
		list_push_back(l2, 2*i+1);
	}

	printf("# List l1 content after filling :\n");
	print_list(l1);
	printf("# List l2 content after filling :\n");
	print_list(l2);

	SubList sl1 = extract_sublist(l1);
	SubList sl2 = extract_sublist(l2);

	list_delete(&l1);
	list_delete(&l2);

	printf("# Extracted sublist sl1 content :\n");
	print_sublist(&sl1);
	printf("# Extracted sublist sl2 content :\n");
	print_sublist(&sl2);

	SubList slmerged = list_merge(sl1, sl2, test_comparison_1);

	printf("# Sublist sl_merged content :\n");
	print_sublist(&slmerged);

	delete_sublist(&slmerged);

	// 

	printf("\x1b[1m-=< PART 3\n\x1b[0m");
	
	// 

	bool failed = false;
	
	// 

	printf("\x1b[1m-=| 3.1\n\x1b[0m");

	// 

	for(int test_i = 0; test_i < PART3_TEST_COUNT && !failed; ++test_i) {
		List* list_to_sort = list_create();
		List* list_to_sort_copy = list_create();

		int list_to_sort_size = rand() % (PART3_LIST_TO_SORT_MAX_SIZE+1);

		for(int i = 0; i < list_to_sort_size; ++i)
			list_push_back(list_to_sort, rand() % 20);

		for(int i = 0; i < list_size(list_to_sort); ++i)
			list_push_back(list_to_sort_copy, list_at(list_to_sort, i));

		SubList sl_list_to_sort = list_mergesort(
			extract_sublist(list_to_sort),
			test_comparison_1
		);

		reconstruct_list_linking_from_sublist(list_to_sort, &sl_list_to_sort);

		if((failed = !test_is_sorted(list_to_sort, test_comparison_2))) {
			print_list(list_to_sort_copy);
			print_list(list_to_sort);
		}

		list_delete(&list_to_sort);
		list_delete(&list_to_sort_copy);
	}

	if(!failed) printf("No sort error.\n");
	else printf("An error happend in list merge-sort.\n");

	// 

	printf("\x1b[1m-=| 3.2\n\x1b[0m");

	// 

	for(int test_i = 0; test_i < PART3_TEST_COUNT && !failed; ++test_i) {
		List* list_to_sort = list_create();
		List* list_to_sort_copy = list_create();

		int list_to_sort_size = rand() % (PART3_LIST_TO_SORT_MAX_SIZE+1);

		for(int i = 0; i < list_to_sort_size; ++i)
			list_push_back(list_to_sort, rand() % 20);

		for(int i = 0; i < list_size(list_to_sort); ++i)
			list_push_back(list_to_sort_copy, list_at(list_to_sort, i));

		list_sort(list_to_sort, test_comparison_1);

		if((failed = !test_is_sorted(list_to_sort, test_comparison_2))) {
			print_list(list_to_sort_copy);
			print_list(list_to_sort);
		}

		list_delete(&list_to_sort);
		list_delete(&list_to_sort_copy);
	}

	if(!failed) printf("No sort error.\n");
	else printf("An error happend in list merge-sort.\n");
}

// 