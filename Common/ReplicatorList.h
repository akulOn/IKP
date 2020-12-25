#pragma once

#define WIN32_LEAN_AND_MEAN

#include <combaseapi.h>
#include <winsock.h>

typedef struct process_st
{
	GUID processId;
	SOCKET acceptedSocket;
}PROCESS;

typedef struct node_st
{
	PROCESS process;
	struct node_st* next;
}NODE;

void Init(NODE** head);

void PushBack(NODE** head, PROCESS process);

PROCESS InitProcess(GUID processId, SOCKET acceptedSocket);