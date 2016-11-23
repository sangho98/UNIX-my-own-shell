#include <stdio.h>
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

int getBit(char * data,int n)	//data 배열에서 n 번쨰 비트 얻어오기
{
	int charNumber = n / 8;
	int nthBit = n % 8;
	char mask = 1 << (8 - nthBit);
	if((mask & data[charNumber]) == mask)
		return 1;
	else
		return 0;
}

void setBit(char *data, int n , int save)	//data 배열에서 n 번째에다가 save 저장 하기
{
	int charNumber = n / 8;
	int nthBit = n % 8;
	char mask = 1 << (8 - nthBit);
	if(save == 1)
		data[charNumber] |= mask;
	else
		data[charNumber] &= mask;
}

void putDataBlock(Inode * inode , DataBlock * dataBlock)	// 아이노드에다가 데이터블럭 추가하기
{
	if(inode -> fileSize == 0)
	{
			add_datalist(inode -> next . dataBlock);
			return;
	}
	else if(inode -> fileSize >= 129 && inode -> fileSize <= 128 * 102)
	{
		int startBit = (fileSize / 128 - 1) * 10;
		int binary[10];
		binaryto(dataBlock -> number , binary);
		for(int i = 0; i < 10; i++)
			setBit(inode -> next -> next -> data,startBit + i, binary[i]);
	}
}

