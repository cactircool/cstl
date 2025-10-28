#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "utility.h"

struct _RBTreeNode {
	char *data;
	bool black; // false = red
	struct _RBTreeNode *left, *right;
};

enum _RBTreeNodeError {
	RBTN_ERR_SUCCESS,
	RBTN_ERR_OOM,
};

enum _RBTreeNodeError _RBTreeNode_create(struct _RBTreeNode *node, bool black, void *data, size_t size);

void _RBTreeNode_recursive_invalidate(struct _RBTreeNode *node, void (*deletor)(void *));

bool _RBTreeNode_black(struct _RBTreeNode *node);

void _RBTreeNode_color_black(struct _RBTreeNode *node);

typedef struct _RBTreeNode *TreeSetIterator;

typedef struct TreeSet {
	TreeSetIterator _root;
	size_t size;
	int (*_comparator)(void *, void *);
	void (*_deletor)(void *);
	const size_t _member_size;
} TreeSet;

typedef enum {
	TS_ERR_SUCCESS = 0,

	TS_ERR_OOM,
	TS_ERR_INVALID_ITERATOR,
	TS_ERR_INVALID_PARENT_ITERATOR,
} TreeSetError;

typedef Pair(TreeSetIterator iterator, bool inserted) TSEmplacePair;

void TreeSet_invalidate(TreeSet *ts);
void TreeSet_custom_invalidate(TreeSet *ts, void (*deletor)(void *));

TreeSet TreeSet_init(int (*comparator)(void *, void *), size_t member_size);
void TreeSet_create(TreeSet *ts, int (*comparator)(void *, void *), size_t member_size);
TreeSet TreeSet_custom_init(int (*comparator)(void *, void *), void (*deletor)(void *), size_t member_size);
void TreeSet_custom_create(TreeSet *ts, int (*comparator)(void *, void *), void (*deletor)(void *), size_t member_size);

void _TreeSet_left_rotate(TreeSet *ts, TreeSetIterator b, TreeSetIterator parent);
void _TreeSet_right_rotate(TreeSet *ts, TreeSetIterator b, TreeSetIterator parent);

bool _TreeSet_bst_insert(TreeSet *ts, void *data, int (*comparator)(void *, void *), TreeSetIterator *stack, unsigned int *size);
TSEmplacePair _TreeSet_bst_emplace(TreeSet *ts, void *data, int (*comparator)(void *, void *), TreeSetIterator *stack, unsigned int *size);
void _TreeSet_insert_rebalance(TreeSet *ts, TreeSetIterator *stack, unsigned int size);

size_t TreeSet_size(TreeSet *ts);
bool TreeSet_insert(TreeSet *ts, void *data);
bool TreeSet_custom_insert(TreeSet *ts, void *data, int (*comparator)(void *, void *));
TSEmplacePair TreeSet_emplace(TreeSet *ts, void *data);
TSEmplacePair TreeSet_custom_emplace(TreeSet *ts, void *data, int (*comparator)(void *, void *));
bool TreeSet_remove(TreeSet *ts, void *data);
bool TreeSet_custom_remove(TreeSet *ts, void *data, int (*comparator)(void *, void *));
TreeSetError _TreeSet_bst_erase_parent(TreeSet *ts, TreeSetIterator it, TreeSetIterator parent);
void _TreeSet_erase_rebalance(TreeSet *ts, TreeSetIterator it, TreeSetIterator parent, TreeSetIterator *stack, unsigned int size);
TreeSetError TreeSet_erase(TreeSet *ts, TreeSetIterator it);

TreeSetIterator TreeSet_find(TreeSet *ts, void *data);
TreeSetIterator TreeSet_custom_find(TreeSet *ts, void *data, int (*comparator)(void *, void *));

bool TreeSet_contains(TreeSet *ts, void *data);
bool TreeSet_custom_contains(TreeSet *ts, void *data, int (*comparator)(void *, void *));

TreeSetIterator TreeSet_upper_bound(TreeSet *ts, void *data);
TreeSetIterator TreeSet_custom_upper_bound(TreeSet *ts, void *data, int (*comparator)(void *, void *));

TreeSetIterator TreeSet_lower_bound(TreeSet *ts, void *data);
TreeSetIterator TreeSet_custom_lower_bound(TreeSet *ts, void *data, int (*comparator)(void *, void *));
