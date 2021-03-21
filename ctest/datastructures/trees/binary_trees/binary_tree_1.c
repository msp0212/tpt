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

int main(void)
{
	height_test();
	bfs_test();
	return 0;
}
