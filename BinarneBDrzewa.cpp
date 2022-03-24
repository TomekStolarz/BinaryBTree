#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_BUFFER 255


typedef struct node {
	char* keys[3];
	int count;
	struct node* links[4];
}NODE;

void rebuildNode(NODE* badNode, int error);

enum menu {
	Zakoncz = 0,
	Wstaw = 1,
	Znajdz = 2,
	Usun=3
};
const char* menuOptions[] = { "Zakoncz", "Wstaw", "Znajdz", "Usun" };

NODE* root;

NODE* findMinKey(NODE* node)
{
	NODE* pos;
	if (node->links[0] != NULL)
		pos = findMinKey(node->links[0]);
	else
		return node;
	return pos;
}

NODE* findMaxKey(NODE* node)
{
	NODE* pos;
	if (node->links[1] != NULL && node->links[2] == NULL)
		pos = findMaxKey(node->links[1]);
	else if (node->links[2] != NULL)
		pos = findMaxKey(node->links[2]);
	else
		return node;
	return pos;
}

void printNode(NODE* toPrint)
{
	int i;
	if (toPrint)
		for (i = 0;i < toPrint->count;i++)
			printf("%c ", toPrint->keys[i]);
	printf("\n");
}

NODE* alocateNode()
{
	NODE* node;
	node = (NODE*)malloc(sizeof(NODE));
	node->count = 0;
	node->links[0] = node->links[1] = node->links[2] = node->links[3]=NULL;
	node->keys[0] = node->keys[1] = node->keys[2] = NULL;
	return node;
}

NODE* findUpperNode(NODE* startNode, NODE* checkNode,int *pos)
{
	NODE* node = NULL,*x = startNode;
	int i = 0;


	for (i = 0;i <= x->count;i++)
		if (x->links[i] == checkNode)
		{
			*pos = i;
			return x;
		}
			
	
	if (checkNode->keys[0] < x->keys[0])
		node = findUpperNode(x->links[0], checkNode, pos);
	else {
		switch (x->count)
		{
		case 1:
			node = findUpperNode(x->links[1], checkNode, pos);
			break;
		case 2:
			if (checkNode->keys[0] < x->keys[1])
				node = findUpperNode(x->links[1], checkNode, pos);
			else
				node = findUpperNode(x->links[2], checkNode, pos);
			break;
		}
	}
	
	return node;
}

void insertNode(char key, NODE* node)
{
	int i = 0, j;
	for (i = 0;i < node->count;i++)
		if (key < node->keys[i])
			break;

	if (i == node->count)
		node->keys[i] = key;
	else
	{
		for (j = node->count;j > i;j--)
			node->keys[j] = node->keys[j - 1];
		node->keys[i] = key;
	}

	node->count += 1;

	if (node->count == 3)
		rebuildNode(node, 1);
}


void rebuildNode(NODE* badNode, int error)//zwolnic badNode
{
	NODE* leftChild,* rightChild,* tempNode;
	leftChild = rightChild = tempNode = NULL;
	int position, neighbourCount, i,childCount;
	char tempKey;

	switch (error)
	{
		case 0://przebudowywanie w wypadku usuwania gdy lisc
			tempNode = findUpperNode(root, badNode, &position);
			badNode->keys[0] = NULL;
			if (position && tempNode->links[position - 1]->count>1)
			{
				neighbourCount = tempNode->links[position - 1]->count;
				tempKey = tempNode->keys[position - 1];
				badNode->count += 1;
				badNode->keys[0] = tempKey;
				tempNode->keys[position - 1] = tempNode->links[position - 1]->keys[neighbourCount - 1];
				tempNode->links[position - 1]->keys[neighbourCount - 1] = NULL;
				tempNode->links[position - 1]->count -= 1;
				
					
			}
			else if (position == 0 && tempNode->links[position + 1]->count>1)
			{
				
				neighbourCount = tempNode->links[position + 1]->count;
				tempKey = tempNode->keys[position];
				badNode->count += 1;
				badNode->keys[0] = tempKey;
				tempNode->keys[position] = tempNode->links[position + 1]->keys[0];
				tempNode->links[position + 1]->keys[0] = tempNode->links[position + 1]->keys[1];
				tempNode->links[position + 1]->keys[1] = NULL;
				tempNode->links[position + 1]->count -= 1;
				
			}
			else if (position)
			{
				tempKey = tempNode->keys[position - 1];
				tempNode->links[position]->keys[0] = tempNode->links[position - 1]->keys[0];
				tempNode->links[position]->keys[1] = tempKey;
				tempNode->links[position]->count = 2;
				free(tempNode->links[position - 1]);
				if(position==1)
					tempNode->keys[position - 1] = tempNode->keys[position];
				tempNode->links[position - 1] = tempNode->links[position];
				tempNode->links[position] = tempNode->links[position+1];

				tempNode->count -= 1;
				if (!tempNode->count)
					rebuildNode(tempNode, 2);
				else if(position==2) {
					tempNode->keys[position - 1]= NULL;
				}
			}
			else
			{
				tempKey = tempNode->keys[position];
				tempNode->links[position]->keys[0] = tempKey;
				tempNode->links[position]->keys[1] = tempNode->links[position + 1]->keys[0];
				tempNode->links[position]->count = 2;
				free(tempNode->links[position + 1]);
				tempNode->keys[position] = tempNode->keys[position + 1];
				tempNode->count-=1;
				if (!tempNode->count)
					rebuildNode(tempNode, 2);
				else
					tempNode->keys[position + 1] = NULL;
			}

			break;
		case 1://przebudowywanie w wypadku za dużej liczbie elementów w węźle 
			leftChild = alocateNode();
			rightChild = alocateNode();

			leftChild->keys[0] = badNode->keys[0];
			leftChild->count = 1;
			leftChild->links[0] = badNode->links[0];
			leftChild->links[1] = badNode->links[1];

			rightChild->keys[0] = badNode->keys[2];
			rightChild->count = 1;
			rightChild->links[0] = badNode->links[2];
			rightChild->links[1] = badNode->links[3];
			if (badNode == root)
			{
				
				root->links[0] = leftChild;
				root->links[1] = rightChild;
				root->count = 1;
				root->keys[0] = root->keys[1];
				root->keys[1] = root->keys[2] = NULL;
				root->links[2] = root->links[3] = NULL;
			}
			else
			{
				tempNode = findUpperNode(root, badNode, &position);
					switch (position)
					{
					case 0:
						tempNode->links[3] = tempNode->links[2];
						tempNode->links[2] = tempNode->links[1];
						tempNode->links[1] = rightChild;
						tempNode->links[0] = leftChild;
						break;
					case 1:
						tempNode->links[1] = leftChild;
						tempNode->links[3] = tempNode->links[2];
						tempNode->links[2] = rightChild;
						break;
					case 2:
						tempNode->links[2] = leftChild;
						tempNode->links[3] = rightChild;
						break;
					}
				
				insertNode(badNode->keys[1], tempNode);
				free(badNode);
			}
			break;

		case 2:
			childCount = 0, position = 0;
			for (i = 0;i <= badNode->count;i++)
			{
				childCount = badNode->links[i]->count;
				if (childCount == 2)
					break;
			}

			if (badNode->keys[0])
				position = 1;

				tempNode = findMaxKey(badNode->links[position]);
				
				if(badNode->links[position]==tempNode && tempNode->count==1)
				{
					tempNode->keys[1] = badNode->links[position + 1]->keys[0];
					tempNode->count += 1;
					free(badNode->links[position + 1]);
					badNode->links[position + 1] = NULL;	
				}
				else
				{
					
					badNode->keys[position] = tempNode->keys[tempNode->count - 1];
					tempNode->keys[tempNode->count - 1] = NULL;
					tempNode->count -= 1;
					if (tempNode->count == 0)
						rebuildNode(tempNode, 0);
				}
			
		break;
	}


}

NODE* findInsertPosition(char key, NODE* checkNode)
{
	NODE* pos;
	if (checkNode->links[0] != NULL)
	{
				if (key < checkNode->keys[0])
					pos = findInsertPosition(key, checkNode->links[0]);
				else if (checkNode->keys[1] == NULL || key < checkNode->keys[1])
					pos = findInsertPosition(key, checkNode->links[1]);
				else
					pos = findInsertPosition(key, checkNode->links[2]);		
		
	}
	else
		pos=checkNode;
	
	return pos;
}

void insertTree(char key)
{
	NODE* properNode;
	if (!root)
	{
		root = alocateNode();
		insertNode(key, root);
	}
	else {
		properNode = findInsertPosition(key, root);
		insertNode(key, properNode);
	}
}

void inOrder(NODE* myNode) {
	int i;
	if (myNode) {
		for (i = 0; i < myNode->count; i++) {
			inOrder(myNode->links[i]);
			printf("%c ", myNode->keys[i]);
		}
		inOrder(myNode->links[i]);
	}
}


NODE* findKey(char key, NODE* node, int* position)
{
	NODE* temp=NULL;
	int i;
	for (i = 0;i < node->count;i++)
		if (key < node->keys[i])
			break;
		else if (key == node->keys[i])
		{
			*position = i;
			return node;
		}
	if (node->links[i] != NULL)
		temp = findKey(key, node->links[i], position);
	else
		return NULL;
	return temp;
}

void deleteKey(char key)
{
	NODE* node;
	int position = 0;
	node = findKey(key, root,&position);

	if (node)
	{
		if (node->links[0])
		{
			node->keys[position] = NULL;
			rebuildNode(node, 2);
		}
		else
		{
			node->count -= 1;
			if (!node->count)
				rebuildNode(node, 0);
			else {
				if (!position)
					node->keys[0] = node->keys[1];
				node->keys[1] = NULL;
			}
			
		}
	}
	else
	{
		printf("Nie mozna usunac klucza ktorego nie ma w drzewie!\n");
	}

}



int main(int argc, char* argv[]) {

	char buffer[MAX_BUFFER], * string;
	int length, i = 0;
	NODE* find = NULL;
	char c;
	int work=1, position=0;

	printf("Podaj tekst do ustworzenie binarnego B-drzewa:\n");
	scanf("%s", &buffer);
	length = strlen(buffer);
	string = (char*)malloc(length);
	strcpy(string, buffer);


	for (i = 0;i < length;i++)
		insertTree(string[i]);


	while (work)
	{
		printf("Drzewo:\n");
		inOrder(root);
		printf("\n");
		printf("Wybierz co chcesz zrobic:\n");
		for (i = Zakoncz;i <= Usun;i++)
			printf("%d. %s\n", i, menuOptions[i]);
		printf("Podaj liczba: ");
		scanf("%d", &work);
		getchar();
		system("cls");
		switch (work)
		{
		case 1:
			printf("Podaj znak: \n");
			scanf("%c", &c);
			insertTree(c);
			printf("Wstawiono klucz %c do drzewa!\n", c);
			break;
		case 2:
			printf("Podaj znak ktory chcesz znalezc: \n");
			scanf("%c", &c);
			find = findKey(c,root, &position);

			if (find)
			{
				printf("Znaleziono klucz %c w wezle z kluczami:\n",c);
				printNode(find);
			}
			else
				printf("NIe znaleziono klucza %c w drzewie!\n",c);
			break;
		case 3:
			printf("Podaj znak ktory chcesz usunac: \n");
			scanf("%c", &c);
			deleteKey(c);
			break;
		default:
			work = 0;
			break;
		}

		printf("\nNacisnij enter by wyczyscic wynik operacji");
		getchar();getchar();
		system("cls");
	}

	return 0;
}