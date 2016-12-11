void swap2(char *temp[], int itemp[], int width) //아이노드번호랑 글자바꾸기
{
	char temp2[4];
	int tmp;
	for(int i=0; i<width; i++)
	{
		for(int j=i+1; j<width+1; j++)
		{
			if(strcmp(temp[i], temp[j])>0)
			{
				strcpy(temp2, temp[i]);
				tmp=itemp[i];
				strcpy(temp[i], temp[j]);
				itemp[i]=itemp[j];
				strcpy(temp[j], temp2);
				itemp[j]=tmp;
			
			}
		}
	}
}
void swap(char *temp[], int  width)//사전식배
{
	char temp2[4];
	for(int i=0; i<width; i++)
	{
		for(int j=i+1; j<width+1; j++)
		{
			if(strcmp(temp[i], temp[j])>0)
			{
				strcpy(temp2, temp[i]);
				strcpy(temp[i], temp[j]);
				strcpy(temp[j], temp2);
			}
		}
	}
}


