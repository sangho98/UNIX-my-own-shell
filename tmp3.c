	void myls(char a_i2[30]){
	int s=0,k=0,inode;
	int width=0;
	char *temp[50];
	int itemp[50];
	char ttemp[50];
	int *stemp[50];
	char *ctemp[50];
	char tmp[5],tmp1[4],type[2];
	struct dtree *p = cur->left;
	struct dtree *ioi = cur->left;
if(!strcmp(a_i2,"-i")){
	
	
		while(ioi != NULL)
		{
			temp[width]=(char *)calloc(strlen(ioi->name)+1, sizeof(char));
			strcpy(temp[width], ioi->name);
			itemp[width]=ioi->inode;
			ioi=ioi->right;
			width++;
		}
		swap2(temp, itemp, width-1);
		for(int i=0; i<width; i++)
			printf("%d %s\n", itemp[i], temp[i]);
}

	while(ioi != NULL){
		temp[width]=(char *)calloc(strlen(ioi->name)+1, sizeof(char));
		strcpy(temp[width], ioi->name);
		width++;
	//	printf("%s\n",ioi->name);
		ioi = ioi->right;
	}
	swap(temp, width-1);
	for(int i=0; i<width; i++)
		printf("%s\n", temp[i]);
	

