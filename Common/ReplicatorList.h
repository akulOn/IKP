#pragma once

#define WIN32_LEAN_AND_MEAN

#include <combaseapi.h>
#include <winsock.h>

typedef struct process_st
{
	GUID processId;
	SOCKET acceptedSocket;
}PROCESS;

typedef struct node_st_replicator
{
	PROCESS process;
	struct node_st_replicator* next;
}NODE_REPLICATOR;

void InitReplicatorList(NODE_REPLICATOR** head);

bool PushBack(NODE_REPLICATOR** head, PROCESS process);

void PrintAllProcesses(NODE_REPLICATOR** head);

bool Contains(NODE_REPLICATOR** head, PROCESS process);

PROCESS InitProcess(GUID processId, SOCKET acceptedSocket);