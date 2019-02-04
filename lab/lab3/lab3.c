#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
	int value;
	struct Node* next;
}Node;

Node* create_node(int value)
{
	Node* new_node;
	new_node = malloc(sizeof(Node));
	if (new_node != NULL) //malloc succeed
	{
		new_node->value = value;
		new_node->next = NULL;
	}
	return new_node;
}

void list_print(Node* head)
{
	Node* curr = head;
	if (curr == NULL)
		return;
	while(curr != NULL && curr->next != NULL) //not the last node
	{
		printf("%d -> ", curr->value);
		curr = curr->next;
	}
	if (curr->next == NULL) //last node
		printf("%d\n", curr->value);
		
}

Node* list_append(Node* head, int value)
{
	Node* curr = head;
	while (curr->next != NULL) //move curr to the end
		curr = curr->next; 
	Node* new_node;
	new_node = malloc(sizeof(Node));
	if (new_node != NULL) //malloc succeed
	{
		new_node->value = value;
		new_node->next = NULL;
		curr->next = new_node;
	}
	return new_node;
}

Node* list_prepend(Node* head, int value)
{
	Node* new_node = create_node(value);
	if (new_node != NULL) //malloc succeed
		new_node->next = head;
	return new_node;
}

Node* list_remove(Node* head, int value)
{
	Node* curr,* prev;
	for (curr = head, prev = NULL; curr != NULL; prev = curr, curr = curr->next)
	{
		if(curr->value == value) //find the node
		{
			if (prev == NULL) //curr is the first node
				head = head->next;
			else
				prev->next = curr->next; //jump the deleting node
			free(curr); //deallocate curr
			break;
		}
	}
	return head;
}

void list_free(Node* head)
{
	if(head == NULL)
		return;
	Node* curr, * after;
	curr = head;
	while (curr != NULL)
	{
		after = curr->next;
		free(curr);
		curr = after;
	}
}

int main()
{
	Node* head = create_node(1);
	list_print(head);
	Node* end = list_append(head, 2);
	list_print(head);
	end->next = create_node(3);
	list_print(head);
	head = list_prepend(head, 0);
	list_print(head);
	list_append(head, 4);
	list_print(head);
	list_append(head, 5);
	list_print(head);
	
	head = list_remove(head, 5);
	list_print(head);
	head = list_remove(head, 3);
	list_print(head);
	head = list_remove(head, 0);
	list_print(head);
	
	list_free(head);
	
	return 0;
}