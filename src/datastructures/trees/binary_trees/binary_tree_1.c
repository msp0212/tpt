#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct tree_node {
	int data;
	struct tree_node *left;
	struct tree_node *right;
};

static inline struct tree_node *tree_node_alloc(int data);


static inline int max_of_2_ints(int a, int b);
static int tree_height(struct tree_node *root);

static struct tree_node *tree_create_height_test_1(void);
static struct tree_node *tree_create_height_test_2(void);
static void height_test(void);

static struct tree_node *tree_create_bfs_test(void);
void bfs_print_level(struct tree_node *root, int level);
static void bfs_print(struct tree_node *root);
static void bfs_test(void);


static inline void print_func_name(const char *func_name);
static inline void print_new_line(void);

static int tree_node_find_level(struct tree_node *root, int data, int level);
static int tree_nodes_are_siblings(struct tree_node *root, int node1, int node2);
static int tree_nodes_are_cousins(struct tree_node *root, int node1, int node2);
static void cousin_test(void);

static inline void print_func_name(const char *func_name)
{
	printf("%s\n", func_name);
}

static inline void print_new_line(void)
{
	printf("\n");
}

static inline struct tree_node *tree_node_alloc(int data)
{
	struct tree_node *tnode;

	tnode  = malloc(sizeof(*tnode));
	assert(tnode);

	tnode->data = data;
	tnode->left = NULL;
	tnode->right = NULL;

	return tnode;
}

static inline int max_of_2_ints(int a, int b)
{
	return (a > b) ? a :b; 
}

static int tree_height(struct tree_node *root)
{
	int l_height, r_height;

	if (!root) return 0;

	l_height = 1 + tree_height(root->left);
	r_height = 1 + tree_height(root->right);

	return max_of_2_ints(l_height, r_height);
}

void height_test(void)
{
	struct tree_node *root;
	int height;
	
	print_func_name(__func__);

	root = tree_create_height_test_1();
	height = tree_height(root);
	printf("height_test_1 = %d\n", height); 

	root = tree_create_height_test_2();
	height = tree_height(root);
	printf("height_test_2 = %d\n", height); 

	print_new_line();
	return;
}

struct tree_node *tree_create_height_test_2(void)
{
	struct tree_node *root;

	/*
	 *             1
	 *              \
	 *               2
	 *                \
	 *                 3
	 *                  \
	 *                   4
	 * */

	/*level 0*/
	root =  tree_node_alloc(1);

	/*level 1*/
	root->right = tree_node_alloc(2);

	/*level 2*/
	root->right->right = tree_node_alloc(3);

	/*level 3*/
	root->right->right->right = tree_node_alloc(4);

	return root;
}
struct tree_node *tree_create_height_test_1(void)
{
	struct tree_node *root;

	/*
	 *             1
	 *            / \
	 *           2   3
	 *          /
	 *         4
	 *
	 * */

	/*level 0*/
	root =  tree_node_alloc(1);

	/*level 1*/
	root->left = tree_node_alloc(2);
	root->right = tree_node_alloc(3);

	/*level 2*/
	root->left->left = tree_node_alloc(4);

	return root;
}
struct tree_node *tree_create_bfs_test(void)
{
	struct tree_node *root;

	/*
	 *           6
	 *          / \
	 *         3   5
	 *        /|	|\
	 *       7 8    1 3
	 *
	 * */

	/*level 0*/
	root =  tree_node_alloc(6);

	/*level 1*/
	root->left = tree_node_alloc(3);
	root->right = tree_node_alloc(5);

	/*level 2*/
	root->left->left = tree_node_alloc(7);
	root->left->right = tree_node_alloc(8);

	root->right->left = tree_node_alloc(1);
	root->right->right = tree_node_alloc(3);

	return root;
}

void bfs_print_level(struct tree_node *root, int level)
{
	if(!root) return;

	if (level == 1) {
		printf("%d ", root->data);
	}

	bfs_print_level(root->left, level - 1);
	bfs_print_level(root->right, level - 1);
	
	return;
}

void bfs_print(struct tree_node *root)
{
	int height;
	int i;

	height = tree_height(root);
	printf("height = %d\n", height);
	
	for (i = 1; i <= height; i++) {
		bfs_print_level(root, i);
	}
	print_new_line();	
}

void bfs_test(void)
{
	struct tree_node *root;

	print_func_name(__func__);

	root = tree_create_bfs_test();
	bfs_print(root);	

	print_new_line();
	return;
}

static int tree_node_find_level(struct tree_node *root, int data, int level)
{
	int ret;

	if (!root) return 0;

	if (root->data == data) return level;

	ret = tree_node_find_level(root->left, data, level + 1);
	if (ret) return ret;

	return tree_node_find_level(root->right, data, level + 1);

}

static int tree_nodes_are_siblings(struct tree_node *root, int node1, int node2)
{
	if (!root) return 0;

	if (!root->left || !root->right) return 0;

	if ((root->left->data == node1 && root->right->data == node2) ||
		(root->left->data == node2 && root->right->data == node1))
		return 1;
	if (tree_nodes_are_siblings(root->left, node1, node2)) return 1;

	return tree_nodes_are_siblings(root->right, node1, node2);
	 
}

static int tree_nodes_are_cousins(struct tree_node *root, int node1, int node2)
{
	int l1, l2, are_siblings;

	l1 = tree_node_find_level(root, node1, 0);
	l2 = tree_node_find_level(root, node2, 0);
	are_siblings = tree_nodes_are_siblings(root, node1, node2);

	printf("%d is at level %d and %d is at level %d. siblings - %d\n",
					node1, l1, node2, l2, are_siblings);

	return (l1 == l2 && !are_siblings);
}

static void cousin_test(void)
{
	struct tree_node *root;

	print_func_name(__func__);

	root = tree_create_bfs_test();

	printf("7 and 1 are cousins ? - %s\n", 
			tree_nodes_are_cousins(root, 7, 1) ? "YES": "NO");
	printf("3 and 5 are cousins ? - %s\n", 
			tree_nodes_are_cousins(root, 3, 5) ? "YES": "NO");
	printf("7 and 5 are cousins ? - %s\n", 
			tree_nodes_are_cousins(root, 7, 5) ? "YES": "NO");


	print_new_line();
	return;
}

int main(void)
{
	height_test();
	bfs_test();
	cousin_test();
	return 0;
}
