// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

typedef struct data_st
{
	char data[100];
}DATA;

typedef struct node_st
{
	DATA data;
	struct node_st* next;
}NODE;

//void Init(NODE** head);

void PushBack(NODE** head, DATA data);

DATA InitData(char* data);