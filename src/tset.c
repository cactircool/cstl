#include "tset.h"

#include <bits/types/struct_itimerspec.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

enum _RBTreeNodeError _RBTreeNode_create(struct _RBTreeNode *node, bool black, void *data, size_t size) {
	node->black = black;
	node->left = node->right = NULL;
	node->data = (char *) malloc(size);
	if (!node->data)
		return RBTN_ERR_OOM;
	memcpy(node->data, data, size);
	return RBTN_ERR_SUCCESS;
}

void _RBTreeNode_recursive_invalidate(struct _RBTreeNode *node, void (*deletor)(void *)) {
	deletor(node->data);
	free(node);
	if (node->left) _RBTreeNode_recursive_invalidate(node->left, deletor);
	if (node->right) _RBTreeNode_recursive_invalidate(node->right, deletor);
}

bool _RBTreeNode_black(struct _RBTreeNode *node) {
	return !node || node->black;
}

void _RBTreeNode_color_black(struct _RBTreeNode *node) {
	if (node) node->black = true;
}

TreeSet TreeSet_init(int (*comparator)(void *, void *), size_t member_size) {
	return TreeSet_custom_init(comparator, free, member_size);
}
void TreeSet_create(TreeSet *ts, int (*comparator)(void *, void *), size_t member_size) {
	TreeSet_custom_create(ts, comparator, free, member_size);
}

void TreeSet_invalidate(TreeSet *ts) {
	TreeSet_custom_invalidate(ts, ts->_deletor);
}

void TreeSet_custom_invalidate(TreeSet *ts, void (*deletor)(void *)) {
	_RBTreeNode_recursive_invalidate(ts->_root, deletor);
}

TreeSet TreeSet_custom_init(int (*comparator)(void *, void *), void (*deletor)(void *), size_t member_size) {
	TreeSet ts = {
		._member_size = member_size,
	};
	TreeSet_custom_create(&ts, comparator, deletor, member_size);
	return ts;
}
void TreeSet_custom_create(TreeSet *ts, int (*comparator)(void *, void *), void (*deletor)(void *), size_t member_size) {
	*((size_t *) &ts->_member_size) = member_size;
	ts->size = 0;
	ts->_comparator = comparator;
	ts->_deletor = deletor;
	ts->_root = NULL;
}

void _TreeSet_left_rotate(TreeSet *ts, TreeSetIterator b, TreeSetIterator parent) {
	if (!b || !b->right) return;

	TreeSetIterator c = b->right;
	if (!parent)
		ts->_root = c;
	else if (b == parent->right)
		parent->right = c;
	else
		parent->left = c;

	b->right = c->left;
	c->left = b;
}
void _TreeSet_right_rotate(TreeSet *ts, TreeSetIterator b, TreeSetIterator parent) {
	if (!b || !b->left) return;

	TreeSetIterator c = b->left;
	if (!parent)
		ts->_root = c;
	else if (b == parent->right)
		parent->right = c;
	else
		parent->left = c;

	b->left = c->right;
	c->right = b;
}

bool _TreeSet_bst_insert(TreeSet *ts, void *data, int (*comparator)(void *, void *), TreeSetIterator *stack, unsigned int *size) {
	TreeSetIterator ptr = ts->_root;
	stack[(*size)++] = ptr;
	if (!ptr) {
		ts->_root = (TreeSetIterator) malloc(sizeof(*ptr));
		_RBTreeNode_create(ptr, true, data, ts->_member_size);
		return true;
	}

	while (ptr) {
		int result = comparator(ptr->data, data);
		if (result == 0)
			return false;

		// data > ptr->data
		if (result < 0) {
			stack[(*size)++] = ptr->right;
			if (!ptr->right) {
				ptr->right = (TreeSetIterator) malloc(sizeof(*(ptr->right)));
				_RBTreeNode_create(ptr->right, false, data, ts->_member_size);
				return true;
			}
			ptr = ptr->right;
		}

		// data < ptr->data
		else {
			stack[(*size)++] = ptr->left;
			if (!ptr->left) {
				ptr->left = (TreeSetIterator) malloc(sizeof(*(ptr->left)));
				_RBTreeNode_create(ptr->left, false, data, ts->_member_size);
				return true;
			}
			ptr = ptr->left;
		}
	}
	return false;
}
TSEmplacePair _TreeSet_bst_emplace(TreeSet *ts, void *data, int (*comparator)(void *, void *), TreeSetIterator *stack, unsigned int *size) {
	TreeSetIterator ptr = ts->_root;
	stack[(*size)++] = ptr;
	if (!ptr) {
		ts->_root = (TreeSetIterator) malloc(sizeof(*ptr));
		_RBTreeNode_create(ptr, true, data, ts->_member_size);
		return (TSEmplacePair) {
			.iterator = ptr,
			.inserted = true,
		};
	}

	while (ptr) {
		int result = comparator(ptr->data, data);
		if (result == 0)
			return (TSEmplacePair) {
				.iterator = ptr,
				.inserted = false,
			};

		// data > ptr->data
		if (result < 0) {
			stack[(*size)++] = ptr->right;
			if (!ptr->right) {
				ptr->right = (TreeSetIterator) malloc(sizeof(*(ptr->right)));
				_RBTreeNode_create(ptr->right, false, data, ts->_member_size);
				return (TSEmplacePair) {
					.iterator = ptr->right,
					.inserted = true,
				};
			}
			ptr = ptr->right;
		}

		// data < ptr->data
		else {
			stack[(*size)++] = ptr->left;
			if (!ptr->left) {
				ptr->left = (TreeSetIterator) malloc(sizeof(*(ptr->left)));
				_RBTreeNode_create(ptr->left, false, data, ts->_member_size);
				return (TSEmplacePair) {
					.iterator = ptr->left,
					.inserted = true,
				};
			}
			ptr = ptr->left;
		}
	}

	return (TSEmplacePair) {
		.iterator = NULL,
		.inserted = false,
	};
}
void _TreeSet_insert_rebalance(TreeSet *ts, TreeSetIterator *stack, unsigned int size) {
	for (unsigned int i = size - 1; i > 1; --i) {
		TreeSetIterator node = stack[i], parent = stack[i - 1], gp = stack[i - 2];
		if (_RBTreeNode_black(parent)) break;

		TreeSetIterator uncle = parent == gp->left ? gp->right : gp->left;

		// Red uncle = recolor
		if (!_RBTreeNode_black(uncle)) {
			_RBTreeNode_color_black(parent);
			_RBTreeNode_color_black(uncle);
			gp->black = false;
			continue;
		}

		// Black uncle = rotations
		TreeSetIterator ggp = (i > 2) ? stack[i - 3] : NULL;
		bool gp_color = gp->black;
		if (parent == gp->left) {
			if (node == parent->right) {
				_TreeSet_left_rotate(ts, parent, gp);
				parent = node;
			}
			_TreeSet_right_rotate(ts, gp, ggp);
		}
		else {
			if (node == parent->left) {
				_TreeSet_right_rotate(ts, parent, gp);
				parent = node;
			}
			_TreeSet_left_rotate(ts, gp, ggp);
		}

		gp->black = parent->black;
		parent->black = gp_color;
		break;
	}

	ts->_root->black = true;
}

size_t TreeSet_size(TreeSet *ts) {
	return ts->size;
}
bool TreeSet_insert(TreeSet *ts, void *data) {
	return TreeSet_custom_insert(ts, data, ts->_comparator);
}
bool TreeSet_custom_insert(TreeSet *ts, void *data, int (*comparator)(void *, void *)) {
	TreeSetIterator stack[65];
	unsigned int size = 0;
	bool inserted = _TreeSet_bst_insert(ts, data, comparator, stack, &size);
	if (inserted) {
		_TreeSet_insert_rebalance(ts, stack, size);
		++ts->size;
	}
	return inserted;
}
TSEmplacePair TreeSet_emplace(TreeSet *ts, void *data) {
	return TreeSet_custom_emplace(ts, data, ts->_comparator);
}
TSEmplacePair TreeSet_custom_emplace(TreeSet *ts, void *data, int (*comparator)(void *, void *)) {
	TreeSetIterator stack[65];
	unsigned int size = 0;
	TSEmplacePair pair = _TreeSet_bst_emplace(ts, data, comparator, stack, &size);
	if (pair.inserted) {
		_TreeSet_insert_rebalance(ts, stack, size);
		++ts->size;
	}
	return pair;
}
bool TreeSet_remove(TreeSet *ts, void *data) {
	return TreeSet_custom_remove(ts, data, ts->_comparator);
}
bool TreeSet_custom_remove(TreeSet *ts, void *data, int (*comparator)(void *, void *)) {
	TreeSetIterator stack[65];
	unsigned int size = 0;
	TreeSetIterator ptr = ts->_root, parent = NULL;
	while (ptr) {
		stack[size++] = ptr;

		int result = comparator(data, ptr->data);
		if (result == 0) {
			TreeSetError result = _TreeSet_bst_erase_parent(ts, ptr, parent);
			if (result != TS_ERR_SUCCESS) return false;
			_TreeSet_erase_rebalance(ts, ptr, parent, stack, size);
			free(ptr);
			return true;
		}

		parent = ptr;
		if (result < 0)
			ptr = ptr->left;
		else
			ptr = ptr->right;
	}
	return false;
}
TreeSetError _TreeSet_bst_erase_parent(TreeSet *ts, TreeSetIterator it, TreeSetIterator parent) {
	if (!it)
		return TS_ERR_SUCCESS;

	if (!it->left && !it->right) {
		if (!parent)
			ts->_root = NULL;
		else if (parent->left == it)
			parent->left = NULL;
		else if (parent->right == it)
			parent->right = NULL;
		else
			return TS_ERR_INVALID_PARENT_ITERATOR;
		return TS_ERR_SUCCESS;
	}

	if (!it->left) {
		// right is valid
		if (!parent)
			ts->_root = it->right;
		else if (parent->left == it)
			parent->left = it->right;
		else if (parent->right == it)
			parent->right = it->right;
		else
			return TS_ERR_INVALID_PARENT_ITERATOR;
		return TS_ERR_SUCCESS;
	}

	if (!it->right) {
		// left is valid
		if (!parent)
			ts->_root = it->left;
		else if (parent->left == it)
			parent->left = it->left;
		else if (parent->right == it)
			parent->right = it->left;
		else
			return TS_ERR_INVALID_PARENT_ITERATOR;
		return TS_ERR_SUCCESS;
	}

	TreeSetIterator successor = it->right, successor_parent = it;
	while (successor->left) {
		successor_parent = successor;
		successor = successor->left;
	}

	if (successor_parent == it)
		successor_parent->right = successor->right;
	else
		successor_parent->left = successor->right;

	successor->left = it->left;
	successor->right = it->right;

	if (!parent)
		ts->_root = successor;
	if (parent->left == it)
		parent->left = successor;
	else if (parent->right == it)
		parent->right = successor;
	else
		return TS_ERR_INVALID_PARENT_ITERATOR;
	return TS_ERR_SUCCESS;
}
void _TreeSet_erase_rebalance(TreeSet *ts, TreeSetIterator it, TreeSetIterator parent, TreeSetIterator *stack, unsigned int size) {
	if (!_RBTreeNode_black(it) && !it->left && !it->right)
		return;

	while (size > 2) {
		TreeSetIterator ptr = stack[size - 1], parent = stack[size - 2], gp = stack[size - 3];
		TreeSetIterator sibling = (parent->left == ptr) ? parent->right : parent->left;

		// Case 1: sibling is red
		if (!_RBTreeNode_black(sibling)) {
			sibling->black = true;
			parent->black = false;
			if (parent->left == ptr)
				_TreeSet_left_rotate(ts, parent, gp);
			else
				_TreeSet_right_rotate(ts, parent, gp);
			sibling = (parent->left == ptr) ? parent->right : parent->left;
		}

		// Case 2: sibling is black and both children are black
		if (!sibling || (_RBTreeNode_black(sibling) && _RBTreeNode_black(sibling->left) && _RBTreeNode_black(sibling->right))) {
			if (sibling) sibling->black = true;
			ptr = parent;
			--size;
			continue;
		}

		// Case 3 + 4: at least 1 red child
		if (parent->left == ptr) {
			if (_RBTreeNode_black(sibling->right)) {
				_RBTreeNode_color_black(sibling->left);
				sibling->black = false;
				_TreeSet_right_rotate(ts, sibling, parent);
				sibling = parent->right;
			}

			sibling->black = parent->black;
			parent->black = true;
			_RBTreeNode_color_black(sibling->right);
			_TreeSet_left_rotate(ts, parent, gp);
		} else {
			if (_RBTreeNode_black(sibling->left)) {
				_RBTreeNode_color_black(sibling->right);
				sibling->black = false;
				_TreeSet_left_rotate(ts, sibling, parent);
				sibling = parent->left;
			}

			sibling->black = parent->black;
			parent->black = true;
			_RBTreeNode_color_black(sibling->left);
			_TreeSet_right_rotate(ts, parent, gp);
		}

		break;
	}

	if (ts->_root) ts->_root->black = true;
}
TreeSetError TreeSet_erase(TreeSet *ts, TreeSetIterator it) {
	TreeSetIterator stack[65];
	unsigned int size = 0;
	TreeSetIterator ptr = ts->_root, parent = NULL;
	while (ptr) {
		stack[size++] = ptr;

		int result = ts->_comparator(it->data, ptr->data);
		if (result == 0) {
			TreeSetError result = _TreeSet_bst_erase_parent(ts, ptr, parent);
			if (result != TS_ERR_SUCCESS) return result;
			_TreeSet_erase_rebalance(ts, ptr, parent, stack, size);
			free(ptr);
			return TS_ERR_SUCCESS;
		}

		parent = ptr;
		if (result < 0)
			ptr = ptr->left;
		else
			ptr = ptr->right;
	}
	return TS_ERR_INVALID_ITERATOR;
}

TreeSetIterator TreeSet_find(TreeSet *ts, void *data) {
	return TreeSet_custom_find(ts, data, ts->_comparator);
}
TreeSetIterator TreeSet_custom_find(TreeSet *ts, void *data, int (*comparator)(void *, void *)) {
	TreeSetIterator ptr = ts->_root;
	while (ptr) {
		int result = comparator(data, ptr->data);
		if (result == 0)
			return ptr;
		if (result < 0)
			ptr = ptr->left;
		else
			ptr = ptr->right;
	}
	return NULL;
}

bool TreeSet_contains(TreeSet *ts, void *data) {
	return TreeSet_find(ts, data) != NULL;
}
bool TreeSet_custom_contains(TreeSet *ts, void *data, int (*comparator)(void *, void *)) {
	return TreeSet_custom_find(ts, data, comparator) != NULL;
}

TreeSetIterator TreeSet_upper_bound(TreeSet *ts, void *data) {
	return TreeSet_custom_upper_bound(ts, data, ts->_comparator);
}
TreeSetIterator TreeSet_custom_upper_bound(TreeSet *ts, void *data, int (*comparator)(void *, void *)) {
	TreeSetIterator ptr = ts->_root;
	TreeSetIterator candidate = NULL;
	while (ptr) {
		int result = comparator(data, ptr->data);
		if (result < 0) {
			candidate = ptr;
			ptr = ptr->right;
		} else {
			ptr = ptr->left;
		}
	}
	return candidate;
}

TreeSetIterator TreeSet_lower_bound(TreeSet *ts, void *data) {
	return TreeSet_custom_lower_bound(ts, data, ts->_comparator);
}
TreeSetIterator TreeSet_custom_lower_bound(TreeSet *ts, void *data, int (*comparator)(void *, void *)) {
	TreeSetIterator ptr = ts->_root;
	TreeSetIterator candidate = NULL;
	while (ptr) {
		int result = comparator(data, ptr->data);
		if (result < 0) {
			candidate = ptr;
			ptr = ptr->left;
		} else {
			ptr = ptr->right;
		}
	}
	return candidate;
}
