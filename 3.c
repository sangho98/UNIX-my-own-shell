#include <stdio.h>
int binaryto(DataBlock * datablock, int * binary); //data숫자 이진수로 변환
void add_datalist(DataBlock *target, DataBlock * dataBlock);	//데이터 링크드 추가하기
typedef struct DataBlock_
{

	char type;
	char data[128];
	int number;
	struct DataBlock_ *next;
}DataBlock;
typedef struct Inode_
{
	int filetype;
	int fileSize;
	DataBlock *data;
}Inode;
int main(void)
{
	DataBlock *data[1024];
	for(int i = 0; i < 1024; i++)
		data[i] = (DataBlock*)malloc(sizeof(DataBlock));
	
	Inode *inode = (Inode*)malloc(sizeof(Inode));

	add_datalist(inode -> next,data[0]);
	add_datalist(data[0] , data[1]);
}
void add_datalist(DataBlock *target, DataBlock * dataBlock)	//데이터 링크드 추가하기
{
	dataBlock -> next = target -> next;
	target->next = dataBlock;
}
int binnaryto(Datablock *datablock, char * binary)
{
	for(int i=0; datablock->number>0; i++)
	{
		binary[i]=datablock->number %2;
		datablock->number=datablock->number/2;
	}
}
