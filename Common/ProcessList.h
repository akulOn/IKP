// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

typedef struct data_st
{
	char data[100];
}DATA;

typedef struct node_st_process
{
	DATA data;
	struct node_st_process* next;
}NODE_PROCESS;

void InitProcessList(NODE_PROCESS** head);

void PushBack(NODE_PROCESS** head, DATA data);

DATA PopFront(NODE_PROCESS** head);

DATA InitData(char* data);