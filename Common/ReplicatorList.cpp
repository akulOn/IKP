#define _CRT_SECURE_NO_WARNINGS

#include "ReplicatorList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Init(NODE** head)
{
	*head = NULL;
}

void PushBack(NODE** head, PROCESS process)
{
	NODE* tempNode = *head;
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	newNode->process = process;
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

PROCESS InitProcess(GUID processId, SOCKET acceptedSocket)
{
	PROCESS p;
	p.processId = processId;
	p.acceptedSocket = acceptedSocket;

	return p;
}