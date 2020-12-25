#define _CRT_SECURE_NO_WARNINGS

#include "ProcessList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*void Init(NODE **head)
{
	*head = NULL;
}*/

void PushBack(NODE** head, DATA data)
{
	NODE* tempNode = *head;
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	newNode->data = data;
	newNode->next = NULL;

	if (tempNode == NULL) //dodajemo prvi element
	{
		*head = newNode;
		return;
	}
	while (tempNode->next != NULL)	//dodajemo na kraj
	{
		tempNode = tempNode->next;
	}
	tempNode->next = newNode;
}

DATA InitData(char* data)
{
	DATA d;
	strcpy(d.data, data);

	return d;
}