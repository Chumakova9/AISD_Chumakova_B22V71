

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

struct Node {
	int key;
	char* info;
	struct Node* left;
	struct Node* right;
	struct Node* parent;
};

// The menu items
const char* msgs[] = { "0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show", "5. Maximum" };
const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

int dialog(const char* msgs[], int);
int D_Add(Node**), D_Find(Node**), D_Delete(Node**), D_Show(Node**), D_Max(Node**);

// The menu item handlers
int (*fptr[])(Node**) = { NULL, D_Add, D_Find, D_Delete, D_Show, D_Max };

int getInt(int* a)
{
	int n;
	do {
		n = scanf_s("%d", a);
		if (n < 0)
			return 0;
		if (n == 0) {
			printf("%s\n", "Error! Repeat input");
			scanf_s("%*c");
		}
	} while (n == 0);

	scanf("%*c");
	return 1;
}

int getString(char* a, int size)
{
	char buf[65536];
	int n;
	do {
		n = scanf_s("%65535[^\n]", buf, (int)sizeof(buf));
		if (n < 0)
			return 0;
		if (n == 0) {
			printf("%s\n", "Error! Repeat input");
			scanf_s("%*c");
		}
	} while (n == 0);

	scanf("%*[^\n]");
	strncpy_s(a, size, buf, sizeof(buf));
	return 1;
}

// Get the user choice what to do next
int dialog(const char* msgs[], int N)
{
	int rc = 0;
	int i, n;

	printf("\n");

	do {
		if (rc != 0)
			puts("You are wrong. Repeat, please!");

		for (i = 0; i < N; ++i)
			puts(msgs[i]);

		puts("Make your choice:");
		n = getInt(&rc);

		if (n == 0)
			rc = 0;
	} while (rc < 0 || rc >= N);

	return rc;
}

void delTree(Node** root)
{
	if (root == NULL || *root == NULL)
		return;

	delTree(&((*root)->left));
	delTree(&((*root)->right));
	free((*root)->info);
	free(*root);
}

Node** find(Node** root, const int key, Node** parent, int* ret)
{
	if (ret == NULL)
		return NULL; // Cannot search

	*ret = 0; // 0 means the key is not found in the tree

	if (root == NULL || *root == NULL)
	{
		return NULL; // Cannot search
	}
	else if ((*root)->key == key)
	{
		*ret = 1; // The exact key is found
		return root;
	}
	else if ((*root)->key > key)
	{
		// Follow the left branch
		if ((*root)->left == NULL)
		{
			if (parent != NULL)
				*parent = *root; // Return the parent for the new node to insert

			return &((*root)->left); // The exact key is not found. Return the left node as a target to insert the new key
		}
		else
			return find(&((*root)->left), key, parent, ret); // Look for the key in the left branch
	}
	else
	{
		// Follow the left branch
		if ((*root)->right == NULL)
		{
			if (parent != NULL)
				*parent = *root; // Return the parent for the new node to insert

			return &((*root)->right); // The exact key is not found. Return the right node as a target to insert the new key
		}
		else
			return find(&((*root)->right), key, parent, ret); // Look for the key in the right branch
	}
}

Node* addNode(Node* root, const int key, const char* info)
{
	if (info == NULL)
		return NULL;

	char* infoCopy = (char*)calloc(strlen(info) + 1, sizeof(char));
	strcpy(infoCopy, info);

	int ret = 0;
	Node* parent = NULL;
	Node** node = find(&root, key, &parent, &ret); // Find the exact node by key or the target to insert a new node

	if (ret == 1)
	{
		printf("Change the key '%d' info: '%s' => '%s'\n", key, (*node)->info, info);
		free((*node)->info); // Delete the old info
		(*node)->info = infoCopy; // Remember the new allocated info copy
		return *node;
	}
	else
	{
		// Create a new node
		Node* newNode = (Node*)calloc(1, sizeof(Node));

		newNode->key = key;
		newNode->info = infoCopy;
		newNode->parent = parent;

		if (node != NULL)
			*node = newNode; // Insert the new node into the tree

		return newNode;
	}
}

char debugTree[20][255];
int maxDepth = 0;

int printSubTree(Node* node, int is_left, int offset, int depth)
{
	char b[20];
	int width = 5;

	if (node == NULL)
		return 0;

	if (depth > maxDepth)
		maxDepth = depth;

	sprintf(b, "[%3d]", node->key);

	int left = printSubTree(node->left, 1, offset, depth + 1);
	int right = printSubTree(node->right, 0, offset + left + width, depth + 1);

	for (int i = 0; i < width; i++)
		debugTree[2 * depth][offset + left + i] = b[i];

	if (depth && is_left) {

		for (int i = 0; i < width + right; i++)
			debugTree[2 * depth - 1][offset + left + width / 2 + i] = '-';

		debugTree[2 * depth - 1][offset + left + width / 2] = '/';
		debugTree[2 * depth - 1][offset + left + width + right + width / 2] = '+';

	}
	else if (depth && !is_left) {

		for (int i = 0; i < left + width; i++)
			debugTree[2 * depth - 1][offset - width / 2 + i] = '-';

		debugTree[2 * depth - 1][offset + left + width / 2] = '\\';
		debugTree[2 * depth - 1][offset - width / 2 - 1] = '+';
	}

	return left + width + right;
}

void printTree(Node* root)
{
	for (int i = 0; i < 20; i++)
		sprintf(debugTree[i], "%80s", " ");

	maxDepth = 0;
	printSubTree(root, 0, 0, 0);

	for (int i = 0; i < 2 * maxDepth + 2; i++)
		printf("%s\n", debugTree[i]);
}

void printNode(Node* item)
{
	if (item == NULL || item->info == NULL)
	{
		printf("NULL\n");
		return;
	}

	printf("Key = %d,\tinfo = \"%s\"\n", item->key, item->info);
}

// Initialize the tree from file 
void initTree(Node** root)
{
	//char filename[] = "data.txt";
	char filename[] = "D:\\Dev\\Tree\\x64\\Debug\\data.txt";
	FILE* fp = fopen(filename, "r");

	if (fp != NULL)
	{
		// reading line by line, max 256 bytes
		const unsigned MAX_LENGTH = 256;
		char buf[MAX_LENGTH];
		int n = 0;

		do
		{
			int k = 0;
			fscanf_s(fp, "%d", &k);
			fscanf_s(fp, "%*c");
			n = fscanf_s(fp, "%65535[^\n]", buf, MAX_LENGTH);
			fscanf_s(fp, "%*c");

			if (n < 0)
				break;

			Node* node = addNode(*root, k, buf);

			if (*root == NULL)
				*root = node; // Remember the tree root
		} while (n > 0);

		// close the file
		fclose(fp);
		printTree(*root);
	}
}

int D_Add(Node** root)
{
	int k, n;
	printf("Enter key:\n");
	n = getInt(&k);

	if (n == 0)
		return 0;

	char info[65536];
	printf("Enter info:\n");

	if (!getString(info, sizeof(info)))
		return 0;

	Node* node = addNode(*root, k, info);

	if (*root == NULL)
		*root = node; // Remember the tree root

	printTree(*root);
	return 1;
}

int D_Find(Node** root)
{
	int k, n;
	puts("Enter key to find item:");
	n = getInt(&k);

	if (n == 0)
		return 0;

	if (*root != NULL)
	{
		int ret = 0;
		Node** node = find(root, k, NULL, &ret);

		if (ret)
		{
			printNode(*node);
			return 1;
		}
	}

	printf("Key %d was not found\n", k);
	return 1;
}

int D_Max(Node** root)
{
	if (*root != NULL)
	{
		int ret = 0;
		Node* parent = NULL;
		Node** node = find(root, INT_MAX, &parent, &ret);

		if (parent != NULL)
		{
			printf("The maximum key info: ");
			printNode(parent);
			return 1;
		}
	}

	return 1;
}

Node* next(Node** cur, int* ret)
{
	if (ret == NULL)
		return NULL; // Cannot search

	*ret = 0; // 0 means the key is not found in the tree

	if (cur == NULL || *cur == NULL)
	{
		return NULL; // Cannot search
	}

	if ((*cur)->right == NULL)
	{
		Node* node = (*cur);

		while (node == node->parent->right)
		{
			// All the parents of the right branch have got a smaller keys
			node = node->parent;

			if (node->parent == NULL)
				return NULL; // No next node, the cur node has got the maximum key
		}

		*ret = 1;
		return node->parent; // The node in the left branch has got a smaller key as compared to the parent. So the parent is the next node
	}
	else
	{
		// Find the minimum key in the right branch
		int r = 0;
		Node* parent = NULL;
		Node** node = find(&((*cur)->right), INT_MIN, &parent, &r);

		if (parent != NULL)
			*ret = 1;

		return parent;
	}
}

int D_Delete(Node** root)
{
	int k, n;
	puts("Enter key to delete item:");
	n = getInt(&k);

	if (n == 0)
		return 0;

	int ret = 0;
	Node* parent = NULL;
	Node** node = find(root, k, &parent, &ret);

	if (ret)
	{
		Node** parentBranch = NULL; // Is the node to delete in the parent's left or right branch?
		Node* parentsNewChild = NULL;
		Node* old = *node;

		if ((*node)->parent != NULL)
		{
			if (*node == (*node)->parent->left)
				parentBranch = &((*node)->parent->left); // Remember the parent's left branch
			else
				parentBranch = &((*node)->parent->right); // Remember the parent's right branch
		}

		if ((*node)->left == NULL && (*node)->right == NULL)
		{
			// The node to delete hasn't got children
			if (node == root)
				*root = NULL;
			else
				parentsNewChild = NULL; // Zero the parent's branch where the node is to be deleted
		}
		else if ((*node)->left != NULL && (*node)->right != NULL)
		{
			// The node to delete has got both left and right branches
			int ret = 0;
			Node* nextNode = next(node, &ret); // Find the next node in the right branch

			if (!ret || nextNode == NULL)
			{
				printf("The next node was not found\n");
				return 1;
			}

			// The next node is always in the parents left branch and the next node hasn't got a left branch
			// Link the next node parent to the next node right branch
			if (nextNode->right != NULL)
				nextNode->right->parent = nextNode->parent;

			nextNode->parent->left = nextNode->right;

			// Reset the next node parent and child branches
			nextNode->parent = (*node)->parent;
			nextNode->left = (*node)->left;
			nextNode->right = (*node)->right;

			if (node == root)
				*root = nextNode;
			else
				parentsNewChild = nextNode; // Put the next node to the parent's branch
		}
		else
		{
			// The node to delete has got only left or right branch
			Node* branch = NULL;

			if ((*node)->left != NULL)
				branch = (*node)->left;
			else
				branch = (*node)->right;

			if (node == root)
				*root = branch;
			else
				parentsNewChild = branch; // Make the node's branch to be the parent's branch
		}

		// Delete the specified node
		free(old->info);
		free(old);

		// Reset the parent's child
		if (parentBranch != NULL)
			*parentBranch = parentsNewChild;

		printTree(*root);
		return 1;
	}

	printf("Node %d was not found\n", k);
	return 1;

	/*
	if (*head != NULL)
	{
		Node* cur = *head;
		Node* last = NULL;

		while (cur != NULL)
		{
			if (k == cur->key)
			{
				if (last != NULL)
					last->next = cur->next;
				else
					*head = cur->next;

				free(cur->info);
				free(cur);
				printf("Node %d was deleted\n", k);
				return 1;
			}

			last = cur;
			cur = cur->next;
		}
	}
	*/
}

int D_Show(Node** root)
{
	if (*root == NULL)
	{
		puts("The tree is empty.");
		return 1;
	}

	int k1, k2, n;

	do
	{
		puts("Enter minimum key of the range:");
		n = getInt(&k1);

		if (n > 0)
		{
			puts("Enter maximum key of the range:");
			n = getInt(&k2);

			if (n <= 0)
				return 1;
		}
		else
			return 1;

		if (k1 > k2)
			puts("The second key is greater than the first one");
	} while (k1 > k2);


	for (int k = k1; k <= k2; k++)
	{
		int ret = 0;
		Node** node = find(root, k, NULL, &ret);

		if (ret)
			printNode(*node);
	}

	return 1;
}

int main()
{
	Node* root = NULL;
	int rc = 0;

	initTree(&root);

	int ret = 0;

	while (rc = dialog(msgs, NMsgs))
		if (!fptr[rc](&root))
			break;

	printf("That's all. Bye!\n");

	delTree(&root);
	return 0;
}
