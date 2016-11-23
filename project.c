#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

union block{
	struct d_block{
		char data[128];
	}dblock;

	struct sid_block{
		struct d_block *dtb[16];
		struct sid_block *next;
	}sidblock;

	struct did_block{
		struct sid_block *sidtb[16];
		struct did_block *next;
	}didblock;
};

struct inode{
	int file_type;
	char file_date[30];
	int file_size;
	int dir;
	int sin;
	int dou;
	struct d_block *db;
	struct sid_block *sid;
	struct did_block *did;
};

struct s_block{
	unsigned imask, dmask1, dmask2;
};

struct my_file_system{
	unsigned b_block : 16;
	struct s_block super[16];
	struct inode inode[512];
	union block *block[1024];
};

struct dtree{
	struct inode *p;
	struct dtree *left;
	struct dtree *right;
};

void prompt(char [], char []);
void command(char [], char []);
int i_bit_check();
int d_bit_check();
void i_bit_insert();
void d_bit_insert();
void i_bit_del(int);
void d_bit_del(int);
void insert_data(char [],int);
void mymkdir(char []);
void mycd(char []);
void myls(char []);
void myrmdir(char []);
void mytree(struct dtree *,int);

struct my_file_system *mfs;
struct dtree *D=NULL;
struct dtree *cur = NULL;
struct dtree *back = NULL;

char path[30]= {'\0'};
char backup[30]= {'\0'};

int main() {
	

	mfs = malloc(sizeof(struct my_file_system));

	char i[30], a_i[20];
	FILE *ms;

	ms = fopen("myfs","r");

	while(ms == NULL){
		printf("파일 시스템 생성 해 주세요 : ");
		scanf("%s",i);

		if(!strcmp(i,"mymkfs")){
			system("touch myfs");

			for(int i=0;i<512;i++){
				mfs->inode[i].file_type = 2;
			}
			for(int i=0;i<1024;i++){
				mfs->block[i] = malloc(sizeof(union block));
				for(int j=0;j<128;j++)
					mfs->block[i]->dblock.data[j] = '\0';
			}

			mfs->block[0] = malloc(sizeof(union block));

			mfs->inode[0].db = &mfs->block[0]->dblock; // inode 0 의 다이렉트 포인터가 데이터블록 0번을 가리킴
			mfs->inode[0].dir = 0;

			for(int i=0;i<128;i++)
				mfs->block[0]->dblock.data[i] = '\0';
			
			sprintf(mfs->block[0]->dblock.data,"%-3d%-4s%-3d%-4s",0,".",0,"..");
			// 512abcd -> 512번 아이노드 abcd 폴더명 or 파일명

			mfs->inode[0].file_type = 1; // 파일 타입 저장 = '1' (폴더 = 1, 파일 = 0)

			// mfs->inode[0].file_date = date(); // 파일 날짜 저장

			mfs->inode[0].file_size = 0; // 사이즈0 저장

			
			mfs->super[0].imask = pow(2,31);
			mfs->super[0].dmask1 = pow(2,31);

			strcpy(path,"/");

			D = malloc(sizeof(struct dtree));

			D->left = NULL;
			D->right = NULL;
			D->p = &mfs->inode[0];

			cur = malloc(sizeof(struct dtree));
			cur = D;

			break;
		}else{
			printf("다시 입력 하세요.\n");
		}
	}

	while(1){
	
		prompt(i,a_i);


	}


}

void prompt(char i[30], char a_i[20]){
	int tmp;

	printf("\n[ %s ]$ ",path);

	scanf("%s",i);

	if((tmp = getchar()) == '\n'){
		a_i[0] = '\0';
		command(i,a_i);
		return;
	}else{
		scanf("%s",a_i);
		command(i,a_i);
	}

}

void command(char i[30], char a_i[30]){

	if(!strcmp(i,"byebye")){
		exit(1);
	}

	if(!strcmp(i,"mymkdir")){
		if(a_i[0] != '\0'){
			mymkdir(a_i);
		}else{
			printf("폴더명을 입력 해주세요.\n");
		}
	}

	if(!strcmp(i,"mycd")){
		if(a_i[0] != '\0'){
			mycd(a_i);
		}else{
			mycd(NULL);
		}

	}

	if(!strcmp(i,"myls")){
		myls(a_i);
	}

	if(!strcmp(i,"mytree")){
		mytree(D,0);
	}

	if(!strcmp(i,"mypwd")){
		printf("%s\n",path);
	}

	if(!strcmp(i,"myrmdir")){
		myrmdir(a_i);
	}
		

}

// 아이노드 비트열 확인

int i_bit_check(){
	unsigned mask = 1;
	int ivalue=0;

	for(int i=0;i<16;i++){
		mask = 1;
		mask <<= 31; // mask 1로 초기화
		for(int j=0;j<32;j++){
			// 비트열 확인
			if((mfs->super[i].imask & mask) == 0){
				ivalue = (i*32) + (j);
				return ivalue;
			}else{
				mask >>=1;
			}
		}
	}

}

// 데이터 블록 비트열 확인

int d_bit_check(){

	unsigned mask = 1,mask1 = 1;
	int dvalue = 0;

	for(int i=0;i<16;i++){
		mask = 1, mask1 = 1;
		mask <<= 31, mask1 <<=31;
		for(int j=0;j<64;j++){
			// 비트열 확인
			if((mfs->super[i].dmask1 & mask) == 0 && j<32){
				dvalue = (i*64) + (j);
				return dvalue;
			}else if(j<32){
				mask >>=1;
			}

			if((mfs->super[i].dmask2 & mask1) == 0 && j>31){
				dvalue = (i*64) + (j);
				return dvalue;
			}else if(j>31){
				mask1 >>=1;
			}
		}

	}

}

void i_bit_insert(){
	int inode;
	unsigned mask = 1;
	mask <<= 31;

	inode = i_bit_check();

	mask >>= (inode%32);
	mfs->super[inode/32].imask |= mask;

}

// data블록 사용 여부 비트열 삽입
void d_bit_insert(){
	int data;
	unsigned mask = 1;
	mask <<= 31;

	data = d_bit_check();

	if(data%64 <= 31){
		mask >>= (data%32);
		mfs->super[data/64].dmask1 |= mask;
	}else{
		mask >>= (data%32);
		mfs->super[data/64].dmask2 |= mask;
	}

}

void i_bit_del(int id){
	unsigned mask = 1;
	mask <<= 31;

	mask >>= (id%32);
	mfs->super[id/32].imask ^= mask;

	mfs->inode[id].file_type = -1;

	for(int i=0;i<30;i++)
		mfs->inode[id].file_date[i] = '\0';

	mfs->inode[id].file_size = 0;

	mfs->inode[id].dir = -1;

	mfs->inode[id].sin = -1;

	mfs->inode[id].dou = -1;

	mfs->inode[id].db = NULL;

	mfs->inode[id].sid = NULL;

	mfs->inode[id].did = NULL;

}

void d_bit_del(int id){
	int dd = mfs->inode[id].dir;
	unsigned mask = 1;
	mask <<= 31;

	if(dd%64 <= 31){
		mask >>= (dd%32);
		mfs->super[dd/64].dmask1 ^= mask;
	}else{
		mask >>= (dd%32);
		mfs->super[dd/64].dmask2 ^= mask;
	}

}

void insert_data(char a_i[30],int inode){
	char tmp[128],tmp2[128];

	if(cur->p->db->data[0] >= '1' && cur->p->db->data[0] <= '9'){
	}else{
	sprintf(tmp,"%-7s",cur->p->db->data);
	}

	sprintf(cur->p->db->data,"%-3d%-4s",inode,a_i);
	strcat(tmp,cur->p->db->data);

	sprintf(cur->p->db->data,"%-7s",tmp);

}

void mymkdir(char a_i[30]){
	int inode,data;
	char tmp[3];

	inode = i_bit_check();
	data = d_bit_check();

	if(cur->left == NULL){
		struct dtree *new = malloc(sizeof(struct dtree));

		new->p = &mfs->inode[inode];
		new->left = NULL;
		new->right = NULL;

		mfs->inode[inode].file_type = 1;
		//mfs->inode[inode].file_date = cal_date;
		mfs->inode[inode].file_size = 0;
		mfs->inode[inode].dir = data;

		mfs->block[data] = malloc(sizeof(struct d_block));
		mfs->inode[inode].db = &mfs->block[data]->dblock;

		for(int i=0;i<128;i++)
			mfs->block[data]->dblock.data[i] = '\0';

		sprintf(mfs->block[data]->dblock.data,"%-3d%-4s%-3d%-4s",inode,".",0,"..");

		cur->left = new;

		i_bit_insert();
		d_bit_insert();

		insert_data(a_i,inode);

	}else{
		struct dtree *new = malloc(sizeof(struct dtree));
		struct dtree *temp = cur->left;
		
		while(temp->right!=NULL)
			temp = temp->right;

		new->p = &mfs->inode[inode];
		new->left = NULL;
		new->right = NULL;

		mfs->inode[inode].file_type = 1;
		//mfs->inode[inode].file_date = cal_date;
		mfs->inode[inode].file_size = 0;
		mfs->inode[inode].dir = data;
		mfs->block[data] = malloc(sizeof(struct d_block));
		mfs->inode[inode].db = &mfs->block[data]->dblock;

		for(int i=0;i<128;i++)
			mfs->block[data]->dblock.data[i] = '\0';

		sprintf(mfs->block[data]->dblock.data,"%-3d%-4s%-3d%-4s",inode,".",0,"..");

		temp->right = new;

		i_bit_insert();
		d_bit_insert();

		insert_data(a_i,inode);
	}

}

void myls(char a_i[30]){
	int s=0,k=0;
	char tmp[4];
	struct dtree *p = cur->left;

	printf("%s\n",cur->p->db->data);
	while(s<13){
		k=0;
		for(int i=s;i<s+3;i++){
		}
		for(int j=s+3;j<s+7;j++){
			tmp[k] = cur->p->db->data[j]; 
			k++;
		}
		printf(":%s ",tmp);
		s+= 7;
	}

	if(cur->left !=NULL){
		while(1){
			k=0;
			for(int i=s;i<s+3;i++){
			}
			for(int j=s+3;j<s+7;j++){
				tmp[k] = cur->p->db->data[j]; 
				k++;
			}
			printf(":%s ",tmp);
			if(p->right==NULL)
				break;
			s+= 7;
			p = p->right;
		}
	}else{
		return;
	}
}

//수정 필요
void mycd(char a_i[30]){
	int s=0,k,ck=999,n=0;
	char tmp[5],tmp2[40];
	struct dtree *p = cur->left;

	if(a_i == NULL){
		sprintf(path,"%s","/");
		cur = D;
		return;
	}

	if(a_i !=NULL){
		sprintf(backup,"%s",path);

		while(1){
			
		k=0;
			for(int i=0;i<5;i++)
				tmp[i] = '\0';

			for(int j=s+3;j<s+7;j++){
				if(!((cur->p->db->data[j] >='a' && cur->p->db->data[j] <='z') || (cur->p->db->data[j] >= 'A' && cur->p->db->data[j] <= 'Z') || (cur->p->db->data[j] >= '1' && cur->p->db->data[j] <= '9') || (cur->p->db->data[j] == '.'))){
					tmp[k] = '\0';
					break;
				}
				tmp[k] = cur->p->db->data[j]; 
				k++;
			}

			if(!strcmp(a_i,tmp)){
				ck = n;
			}

			if(cur->p->db->data[s+7] == '\0')	
				break;
		s+=7;
		n++;
		}

		back = p;

		if(ck == 999){
			printf("잘못된 경로 입니다.\n");
			return;
		}

		if(ck == 0)
			return;

		if(ck == 1){
			cur = back;
			sprintf(path,"%s",backup);
			return;
		}


		for(int i=0;i<ck-2;i++){
			p = p->right;
		}

		cur = p;

		sprintf(tmp2,"%s",path);

		if(path[0] == '/' && path[1] == '\0'){
			sprintf(path,"/%s",a_i);
		}else{
			sprintf(path,"%s/%s",tmp2,a_i);
		}


		
	}

			

}

void myrmdir(char a_i[30]){
	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	struct dtree *p = cur->left;
	struct dtree *t = cur;

	for(int i=0;i<100;i++)
		tmp2[i] = '\0';

	for(int i=0;i<100;i++)
		tmp3[i] = '\0';

	if(a_i !=NULL){

		while(1){
			
		k=0;
			for(int i=0;i<5;i++)
				tmp[i] = '\0';

			for(int j=s+3;j<s+7;j++){
				if(!((cur->p->db->data[j] >='a' && cur->p->db->data[j] <='z') || (cur->p->db->data[j] >= 'A' && cur->p->db->data[j] <= 'Z') || (cur->p->db->data[j] >= '1' && cur->p->db->data[j] <= '9') || (cur->p->db->data[j] == '.'))){
					tmp[k] = '\0';
					break;
				}
				tmp[k] = cur->p->db->data[j]; 
				k++;
			}

			if(!strcmp(a_i,tmp)){
				ck = n;
			}

			if(cur->p->db->data[s+7] == '\0'){	
				l = n;
				break;
			}
		s+=7;
		n++;
		}

		if(ck == 999){
			printf("잘못된 경로 입니다.\n");
			return;
		}

		if(ck == 2){
			if(l == 2){
				cur->left = NULL;
				for(int i=0;i<ck*7;i++){
					tmp2[i] = cur->p->db->data[i];
				}
				for(int i=ck*7;i<(ck*7)+3;i++){
					tmp5[i-(ck*7)]= cur->p->db->data[i];
				}
				id = atoi(tmp5);
				d_bit_del(id);
				i_bit_del(id);
				for(int i=(ck*7)+7;i<(l*7);i++){
					tmp3[i] = cur->p->db->data[i];
				}
				sprintf(tmp4,"%s%s",tmp2,tmp3);
				sprintf(cur->p->db->data,"%s",tmp4);
				return;
			}
			t = cur->left;
			cur->left = cur->left->right;
			t->right = NULL;
			free(t);
			for(int i=0;i<ck*7;i++){
				tmp2[i] = cur->p->db->data[i];
			}
			for(int i=ck*7;i<(ck*7)+3;i++){
				tmp5[i-(ck*7)]= cur->p->db->data[i];
			}
			id = atoi(tmp5);
			d_bit_del(id);
			i_bit_del(id);
			for(int i=(ck*7)+7;i<(l*7)+7;i++){
				tmp3[m] = cur->p->db->data[i];
				m++;
			}
			sprintf(tmp4,"%s%s",tmp2,tmp3);
			sprintf(cur->p->db->data,"%s",tmp4);
			return;
		}

		for(int i=0;i<ck-3;i++){
			p = p->right;
		}

		back = p->right;
		p->right = p->right->right;
		back->right=NULL;
		free(back);
		for(int i=0;i<ck*7;i++){
			tmp2[i] = cur->p->db->data[i];
		}
		for(int i=ck*7;i<(ck*7)+3;i++){
			tmp5[i-(ck*7)]= cur->p->db->data[i];
		}
		id = atoi(tmp5);
		d_bit_del(id);
		i_bit_del(id);
		for(int i=(ck*7)+7;i<(l*7)+7;i++){
			tmp3[m] = cur->p->db->data[i];
			m++;
		}
		sprintf(tmp4,"%s%s",tmp2,tmp3);
		sprintf(cur->p->db->data,"%s",tmp4);

	}
		
	

}

void mytree(struct dtree *D,int d){
	for(int i=0;i<d;i++)
		printf(" ");
	printf("%d\n",D->p->dir);

	if(D->left!=NULL)
		mytree(D->left,d+1);
	if(D->right!=NULL)
		mytree(D->right,d);

}
