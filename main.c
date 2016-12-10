#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct d_block{
	int num; // 데이터블록 번호
	unsigned char data[128];
}dblock;

struct inode{
	int file_type;
	char file_date[30];
	int file_size;
	int dir;
	int sin;
	int dou;
	int num;
	int numP;
	int numS;
	struct d_block *db;
	struct d_block *sidb;
	struct d_block *didb;
};

struct s_block{
	unsigned imask, dmask1, dmask2;
};

struct my_file_system{
	unsigned b_block : 16;
	struct s_block super[16];
	struct inode *inode[512];
	struct d_block *block[1024];
};

struct dtree{
	int inode;
	char name[5];
	struct inode *p;
	struct dtree *left;
	struct dtree *right;
};

struct btree{
	struct d_block *p;
	struct btree *next;

};

void prompt(char [], char [],char [],char []);
void command(char [], char [],char [],char []);
int i_bit_check();
int d_bit_check();
void i_bit_insert();
void d_bit_insert();
void i_bit_del(int);
void d_bit_del(int);
void cal_date(int);
int cal_size(char []);
void set_sidbit(char [],int);
int* get_sidbit(int);
int *get_sidbit_ADD(int);
void make_sid(int);
void set_didbit(int);
int* get_didbit(int);
void set_dsidbit(int);
int* get_dsidbit(int);
void make_did(int);
void in_data(char [],int);
void sin_data(char [],int,int);
void dsin_data(char [],int,int);
void in_dir(char [],int);
void mymkdir(char []);
int mycd(char []);
int mycd_func(char []);
void myls(char []);
//void myls_func(char [],char []);
void myrmdir(char []);
//void myrmdir_func(char []);
void mytree(struct dtree*,int);
void mytree_func(struct dtree*,int);
void myshowinode(char []);
void myshowblock(char []);
void mystate();
void mycpfrom(char[],char[]);
void mytouch(char []);
void mycat(char []);
void mycpto(char [],char []);
void myshowfile(char [],char [],char []);
void mycp(char [],char []);
void mymv(char [],char []);
int mymv_func(char []);
//void myrm(char []);

struct my_file_system *mfs;

struct dtree *D=NULL;
struct dtree *cur = NULL;
struct dtree *back = NULL;
struct dtree *back2 = NULL;
struct dtree *back3 = NULL;
struct dtree *back4 = NULL;

char path[30]= {'\0'};
char backup[30]= {'\0'};
char backup2[30]= {'\0'};
char backup3[30]= {'\0'};
char backup4[30]= {'\0'};

int main() {


	mfs = malloc(sizeof(struct my_file_system));

	char make[10];
	FILE *ms;

	ms = fopen("myfs","r");

	if(ms != NULL){
		// myfs 이진파일 불러들이기 (함수 생성 가능)

		system("rm myfs");
		main();

	}
	while(ms == NULL){
		printf("Error ! Please make your file system.\n");
		printf("command : ");
		scanf("%s",make);

		if(!strcmp(make,"mymkfs")){
			system("touch myfs");

			for(int i=0;i<16;i++){
				mfs->super[i].imask = 0;
				mfs->super[i].dmask2 = 0;
				mfs->super[i].dmask1 = 0;
			}

			mfs->block[0] = malloc(sizeof(struct d_block));
			mfs->inode[0] = malloc(sizeof(struct inode));
			mfs->inode[0]->db = mfs->block[0];
			mfs->inode[0]->dir = 0;
			mfs->inode[0]->sin = 0;
			mfs->inode[0]->dou = 0;

			/* 데이터 블록 트리 구조 */

			for(int i=0;i<128;i++)
				mfs->block[0]->data[i] = '\0';

			sprintf(mfs->block[0]->data,"%-3d%-4s%-3d%-4s",0,".",0,"..");
			// 512abcd -> 512번 아이노드 abcd 폴더명 or 파일명

			mfs->inode[0]->file_type = 1; // 파일 타입 저장 = '1' (폴더 = 1, 파일 = 0)
			cal_date(0);
			mfs->inode[0]->file_size = 0; // 사이즈0 저장
			mfs->inode[0]->num = 0;


			mfs->super[0].imask = pow(2,31);
			mfs->super[0].dmask1 = pow(2,31);

			strcpy(path,"/");

			D = malloc(sizeof(struct dtree));

			strcpy(D->name,"/");
			D->left = NULL;
			D->right = NULL;
			D->p = mfs->inode[0];

			cur = malloc(sizeof(struct dtree));
			cur = D;

			break;
		}else{
			printf("Retry.\n");
		}
	}

	while(1){

		char i[30], a_i[20],a_i2[20],a_i3[20];

		prompt(i,a_i,a_i2,a_i3);

	}


}

void prompt(char i[30], char a_i[20],char a_i2[20],char a_i3[20]){
	char input[100],*result;
	int tmp = 0;

	printf("\n[ %s ]$ ",path); 


	scanf("%s",i);

	if((tmp = getchar()) == '\n'){
		a_i[0] = '\0';
		a_i2[0] = '\0';
		a_i3[0] = '\0';
		command(i,a_i,a_i2,a_i3);
		return;
	}

	scanf("%s",a_i);

	if((tmp = getchar()) == '\n'){
		a_i2[0] = '\0';
		a_i3[0] = '\0';
		command(i,a_i,a_i2,a_i3);
		return;
	}


	scanf("%s",a_i2);

	if((tmp = getchar()) == '\n'){
		a_i3[0] = '\0';
		command(i,a_i,a_i2,a_i3);
		return;
	}else{
		scanf("%s",a_i3);
		command(i,a_i,a_i2,a_i3);
	};

}

void command(char i[30], char a_i[30],char a_i2[20],char a_i3[20]){
	char name[5];

	if(!strcmp(i,"byebye")){
		exit(1);
	}

	if(!strcmp(i,"mymkdir")){
		if(a_i[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
			mymkdir(name);
		}else{
			printf("폴더명을 입력 해주세요.\n");
		}
	}

	if(!strcmp(i,"mycpfrom")){
		if(a_i2[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
			mycpfrom(a_i,name);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	if(!strcmp(i,"mycd")){
		if(a_i[0] != '\0'){
			mycd_func(a_i);
		}else{
			mycd(NULL);
		}

	}

	if(!strcmp(i,"myls")){
		myls(a_i);
	}

	if(!strcmp(i,"mytree")){
		if(a_i[0] != '\0'){
			if(!strcmp(a_i,"/")){
				struct dtree *tr = D;
				mytree_func(tr,0);
				return;
			}
			int ck;

			sprintf(backup3,"%s",path);
			back3 = cur;

			ck = mycd_func(a_i);

			if(ck == 1){
				return;
			}

			struct dtree *tr = cur;

			mytree_func(tr,0);

			cur = back3;
			sprintf(path,"%s",backup3);
		}else{
			struct dtree *tr = cur;
			mytree(tr,0);
		}
	}

	if(!strcmp(i,"mypwd")){
		printf("%s\n",path);
	}

	if(!strcmp(i,"myrmdir")){
		myrmdir(a_i);
	}

	if(!strcmp(i,"myshowinode")){
		myshowinode(a_i);
	}

	if(!strcmp(i,"myshowblock")){
		myshowblock(a_i);
	}

	if(!strcmp(i,"mystate")){
		mystate();
	}

	if(!strcmp(i,"mycat")){
		mycat(a_i);
	}

	if( !(i[0] == 'm' && i[1] == 'y') ){
		sprintf(i,"%s %s %s",i,a_i,a_i2);
		system(i);
	}

	if(!strcmp(i,"mytouch")){
		if(a_i[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
			mytouch(name);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	if(!strcmp(i,"myshowfile")){
		myshowfile(a_i,a_i2,a_i3);
	}

	if(!strcmp(i,"mycpto")){
		mycpto(a_i,a_i2);
	}

	if(!strcmp(i,"mycp")){
		sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
		mycp(a_i,name);
		system("rm loperz");
	}

	if(!strcmp(i,"mymv")){
		sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
		mymv(a_i,name);
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

	printf("id = %d\n",id);

	free(mfs->inode[id]);
	mfs->inode[id] = NULL;

}

void d_bit_del(int id){
	int dd = mfs->inode[id]->dir;
	unsigned mask = 1;
	mask <<= 31;

	if(dd%64 <= 31){
		mask >>= (dd%32);
		mfs->super[dd/64].dmask1 ^= mask;
	}else{
		mask >>= (dd%32);
		mfs->super[dd/64].dmask2 ^= mask;
	}

	printf("dd = %d\n",dd);
	free(mfs->block[dd]);
	mfs->block[dd] = NULL;

}

void cal_date(int inode){
	struct tm *t;
	time_t timer; // 시간 측정

	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기

	sprintf(mfs->inode[inode]->file_date,"%04d/%02d/%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);

}

int cal_size(char a_i[20]){
	FILE *fp;

	fp = fopen(a_i,"rt");

	fseek(fp,0,SEEK_END);


	return ftell(fp);

}

void set_sidbit(char a_i[20],int inode){
	unsigned in,n,a;
	int i=0,k=0,m=0,size,loopQ,loopP;

	size = cal_size(a_i);
	loopQ = ( (size-128) / 128 );
	loopP = size % 128;

	if(loopP != 0)
		loopQ += 1;

	if(loopQ > 102){
		loopQ = 102;
	}

	mfs->inode[inode]->num = loopQ;
	mfs->inode[inode]->numS = loopQ;

	while(m<loopQ){

		if(m == 102)
			return;

		in = d_bit_check();

		mfs->block[in] = malloc(sizeof(struct d_block));

		d_bit_insert();

		if(i%4 == 0){
			n = in >> 2;
			mfs->inode[inode]->sidb->data[k++] = n;
			a = in;
			a = a & 0x3;
			a = a << 6;
			mfs->inode[inode]->sidb->data[k] = a;
		}

		if(i%4 == 1){
			n = in >> 4;
			mfs->inode[inode]->sidb->data[k++] |= n;
			a = in;
			a = a & 0xF;
			a = a << 4;
			mfs->inode[inode]->sidb->data[k] = a;
		}

		if(i%4 == 2){
			n = in >> 6;
			mfs->inode[inode]->sidb->data[k++] |= n;
			a = in;
			a = a & 0x3F;
			a = a << 2;
			mfs->inode[inode]->sidb->data[k] = a;
		}

		if(i%4 ==3){
			n = in >> 8;
			mfs->inode[inode]->sidb->data[k++] |= n;
			a = in;
			a = a & 0xFF;
			mfs->inode[inode]->sidb->data[k] = a;
			k++;
		}

		i++;
		m++;

	}

}

int *get_sidbit(int inode){
	unsigned c,n;
	int loopQ,k=0,data,i=0,a=1;

	loopQ = mfs->inode[inode]->num;
	data = mfs->inode[inode]->sin;

	int *tmp;
	tmp = (int *)malloc( (loopQ) * sizeof(int));

	while(i<loopQ){

		if(i == 102)
			return tmp;

		if(i%4 == 0){
			c = mfs->block[data]->data[k++];
			c = c << 2;
			c = c & 0x3FF;
			n = mfs->block[data]->data[k]; 
			n = n >> 6; 
			c += n;
			tmp[k-a] = c;
		}

		if(i%4 == 1){
			c = mfs->block[data]->data[k++];
			c = c << 4;
			c = c & 0x3F0;
			n = mfs->block[data]->data[k]; 
			n = n >> 4; 
			c += n;
			tmp[k-a] = c;
		}

		if(i%4 == 2){
			c = mfs->block[data]->data[k++];
			c = c << 6;
			c = c & 0x3C0;
			n = mfs->block[data]->data[k]; 
			n = n >> 2; 
			c += n;
			tmp[k-a] = c;
		}

		if(i%4 == 3){
			c = mfs->block[data]->data[k++];
			c = c << 8;
			c = c & 0x300;
			n = mfs->block[data]->data[k]; 
			c += n;
			tmp[k-(a++)] = c;
			k++;
		}
		i++;

	}

	return tmp;

}

int *get_sidbit_ADD(int inode){
	unsigned c,n;
	int loopQ,k=0,data,i=0,a=1;

	loopQ = mfs->inode[inode]->numS;
	data = mfs->inode[inode]->sin;

	int *tmp;
	tmp = (int *)malloc( (loopQ) * sizeof(int));

	while(i<loopQ){

		if(i == 102)
			return tmp;

		if(i%4 == 0){
			c = mfs->block[data]->data[k++];
			c = c << 2;
			c = c & 0x3FF;
			n = mfs->block[data]->data[k]; 
			n = n >> 6; 
			c += n;
			tmp[k-a] = c;
		}

		if(i%4 == 1){
			c = mfs->block[data]->data[k++];
			c = c << 4;
			c = c & 0x3F0;
			n = mfs->block[data]->data[k]; 
			n = n >> 4; 
			c += n;
			tmp[k-a] = c;
		}

		if(i%4 == 2){
			c = mfs->block[data]->data[k++];
			c = c << 6;
			c = c & 0x3C0;
			n = mfs->block[data]->data[k]; 
			n = n >> 2; 
			c += n;
			tmp[k-a] = c;
		}

		if(i%4 == 3){
			c = mfs->block[data]->data[k++];
			c = c << 8;
			c = c & 0x300;
			n = mfs->block[data]->data[k]; 
			c += n;
			tmp[k-(a++)] = c;
			k++;
		}
		i++;

	}

	return tmp;

}

void make_sid(int inode){
	int data;

	data = d_bit_check();

	mfs->block[data] = malloc(sizeof(struct d_block));

	mfs->inode[inode]->sidb = mfs->block[data];
	mfs->inode[inode]->sin = data;

	for(int i=0;i<128;i++)
		mfs->inode[inode]->sidb->data[i] = '\0'; 

	d_bit_insert();

}

void set_didbit(int inode){
	unsigned in,n,a;
	int i=0,k=0,m=0,size,loopQ,loopP;

	size = mfs->inode[inode]->file_size;
	loopQ = ( (size-(13184)) / 13056 );
	loopP = size % 13056;

	if(loopP != 0)
		loopQ += 1;

	if(loopQ > 102){
		loopQ = 102;
	}

	mfs->inode[inode]->num = loopQ;

	for(int i=0;i<128;i++){
		mfs->inode[inode]->didb->data[i] = 0;
	}

	while(m<loopQ){

		if(m == 102)
			return;

		in = d_bit_check();

		mfs->block[in] = malloc(sizeof(struct d_block));

		d_bit_insert();

		if(i%4 == 0){
			n = in >> 2;
			mfs->inode[inode]->didb->data[k++] = n;
			a = in;
			a = a & 0x3;
			a = a << 6;
			mfs->inode[inode]->didb->data[k] = a;
		}

		if(i%4 == 1){
			n = in >> 4;
			mfs->inode[inode]->didb->data[k++] |= n;
			a = in;
			a = a & 0xF;
			a = a << 4;
			mfs->inode[inode]->didb->data[k] = a;
		}

		if(i%4 == 2){
			n = in >> 6;
			mfs->inode[inode]->didb->data[k++] |= n;
			a = in;
			a = a & 0x3F;
			a = a << 2;
			mfs->inode[inode]->didb->data[k] = a;
		}

		if(i%4 ==3){
			n = in >> 8;
			mfs->inode[inode]->didb->data[k++] |= n;
			a = in;
			a = a & 0xFF;
			mfs->inode[inode]->didb->data[k] = a;
			k++;
		}

		i++;
		m++;

	}
}

int *get_didbit(int inode){
	unsigned c,n;
	int loopQ,k=0,data,i=0,a=1;

	loopQ = mfs->inode[inode]->num;
	data = mfs->inode[inode]->dou;

	int *tmp;
	tmp = (int *) malloc(loopQ * sizeof(int));

	while(i<loopQ){

		if(i == 102)
			return tmp;


		if(i%4 == 0){
			c = mfs->block[data]->data[k++];
			c = c << 2;
			c = c & 0x3FF;
			n = mfs->block[data]->data[k]; 
			n = n >> 6; 
			c += n;
			tmp[k-a] = c;
		}

		if(i%4 == 1){
			c = mfs->block[data]->data[k++];
			c = c << 4;
			c = c & 0x3F0;
			n = mfs->block[data]->data[k]; 
			n = n >> 4; 
			c += n;
			tmp[k-a] = c;
		}

		if(i%4 == 2){
			c = mfs->block[data]->data[k++];
			c = c << 6;
			c = c & 0x3C0;
			n = mfs->block[data]->data[k]; 
			n = n >> 2; 
			c += n;
			tmp[k-a] = c;
		}

		if(i%4 == 3){
			c = mfs->block[data]->data[k++];
			c = c << 8;
			c = c & 0x300;
			n = mfs->block[data]->data[k]; 
			c += n;
			tmp[k-(a++)] = c;
			k++;
		}
		i++;

	}

	return tmp;

}

void set_dsidbit(int inode){
	unsigned in,n,a;
	int i=0,k=0,m=0,z=0,loopQ,size,loopP,loopM;
	int data;

	size = mfs->inode[inode]->file_size;
	loopQ = mfs->inode[inode]->num;

	int *tmp;
	tmp = (int *) malloc(sizeof( (loopQ) * sizeof(int) ));

	tmp = get_didbit(inode);

	if(loopQ > 102){
		loopQ = 102;
	}

	while(m<loopQ-1){

		data = tmp[m++];
		k=0;
		i=0;

		for(int z=0;z<102;z++){

			in = d_bit_check();

			mfs->block[in] = malloc(sizeof(struct d_block));

			d_bit_insert();

			if(i%4 == 0){
				n = in >> 2;
				mfs->block[data]->data[k++] = n;
				a = in;
				a = a & 0x3;
				a = a << 6;
				mfs->block[data]->data[k] = a;
			}

			if(i%4 == 1){
				n = in >> 4;
				mfs->block[data]->data[k++] |= n;
				a = in;
				a = a & 0xF;
				a = a << 4;
				mfs->block[data]->data[k] = a;
			}

			if(i%4 == 2){
				n = in >> 6;
				mfs->block[data]->data[k++] |= n;
				a = in;
				a = a & 0x3F;
				a = a << 2;
				mfs->block[data]->data[k] = a;
			}

			if(i%4 ==3){
				n = in >> 8;
				mfs->block[data]->data[k++] |= n;
				a = in;
				a = a & 0xFF;
				mfs->block[data]->data[k] = a;
				k++;
			}

			i++;
		}

	}

	loopP = ((size-13184) - (13056 * m)) / 128 ;
	loopM = ((size-13184) - (13056 * m)) % 128 ;

	if(loopM != 0)
		loopP +=2;

	mfs->inode[inode]->numP = loopP;

	data = tmp[m];

	k = 0;
	i = 0;

	for(int u=0;u<loopP;u++){

		in = d_bit_check();

		mfs->block[in] = malloc(sizeof(struct d_block));

		d_bit_insert();

		if(i%4 == 0){
			n = in >> 2;
			mfs->block[data]->data[k++] = n;
			a = in;
			a = a & 0x3;
			a = a << 6;
			mfs->block[data]->data[k] = a;
		}

		if(i%4 == 1){
			n = in >> 4;
			mfs->block[data]->data[k++] |= n;
			a = in;
			a = a & 0xF;
			a = a << 4;
			mfs->block[data]->data[k] = a;
		}

		if(i%4 == 2){
			n = in >> 6;
			mfs->block[data]->data[k++] |= n;
			a = in;
			a = a & 0x3F;
			a = a << 2;
			mfs->block[data]->data[k] = a;
		}

		if(i%4 ==3){
			n = in >> 8;
			mfs->block[data]->data[k++] |= n;
			a = in;
			a = a & 0xFF;
			mfs->block[data]->data[k] = a;
			k++;
		}

		i++;

	}
}

int *get_dsidbit(int inode){
	unsigned c,n;
	int loopQ,loopP,k=0,p=0,data,i=0,a=1,u=0,s=0,b=0;

	loopQ = mfs->inode[inode]->num;
	loopP = mfs->inode[inode]->numP;

	int *tmp,*tmp2;
	tmp = malloc( ( ((loopQ - 1) * 102) + loopP) * sizeof(int));
	tmp2 = malloc((loopQ) * sizeof(int));

	tmp2 = get_didbit(inode);

	while(p<loopQ-1){

		data = tmp2[p];

		i=0;
		k=0;

		for(int b=0;b<102;b++){

			if(i%4 == 0){
				c = mfs->block[data]->data[k++];
				c = c << 2;
				c = c & 0x3FF;
				n = mfs->block[data]->data[k]; 
				n = n >> 6; 
				c += n;
				tmp[s++] = c;
			}

			if(i%4 == 1){
				c = mfs->block[data]->data[k++];
				c = c << 4;
				c = c & 0x3F0;
				n = mfs->block[data]->data[k]; 
				n = n >> 4; 
				c += n;
				tmp[s++] = c;
			}

			if(i%4 == 2){
				c = mfs->block[data]->data[k++];
				c = c << 6;
				c = c & 0x3C0;
				n = mfs->block[data]->data[k]; 
				n = n >> 2; 
				c += n;
				tmp[s++] = c;
			}

			if(i%4 == 3){
				c = mfs->block[data]->data[k++];
				c = c << 8;
				c = c & 0x300;
				n = mfs->block[data]->data[k]; 
				c += n;
				tmp[s++] = c;
				k++;
			}
			i++;
		}
		p++;
	}

	i = 0;
	k = 0;

	data = tmp2[p];

	u = (loopQ - 1) * 102;

	while(i<loopP+1){

		if(i%4 == 0){
			c = mfs->block[data]->data[k++];
			c = c << 2;
			c = c & 0x3FF;
			n = mfs->block[data]->data[k]; 
			n = n >> 6; 
			c += n;
			tmp[u++] = c;
		}

		if(i%4 == 1){
			c = mfs->block[data]->data[k++];
			c = c << 4;
			c = c & 0x3F0;
			n = mfs->block[data]->data[k]; 
			n = n >> 4; 
			c += n;
			tmp[u++] = c;
		}

		if(i%4 == 2){
			c = mfs->block[data]->data[k++];
			c = c << 6;
			c = c & 0x3C0;
			n = mfs->block[data]->data[k]; 
			n = n >> 2; 
			c += n;
			tmp[u++] = c;
		}

		if(i%4 == 3){
			c = mfs->block[data]->data[k++];
			c = c << 8;
			c = c & 0x300;
			n = mfs->block[data]->data[k]; 
			c += n;
			tmp[u++] = c;
			k++;
		}
		i++;
	}


	free(tmp2);
	tmp2 = NULL;

	return tmp;

}

void make_did(int inode){
	int data;

	data = d_bit_check();

	mfs->block[data] = malloc(sizeof(struct d_block));

	mfs->inode[inode]->didb = mfs->block[data];
	mfs->inode[inode]->dou = data;

	for(int i=0;i<128;i++)
		mfs->inode[inode]->didb->data[i] = '\0';

	d_bit_insert();
}

void in_data(char a_i[20],int inode){
	FILE *fp;
	int fpos,size,ch,k=0,data;

	fp = fopen(a_i,"rt");
	size = cal_size(a_i);

	fseek(fp,0,SEEK_SET);


	for(int i=0;i<128;i++){

		if(ftell(fp) == size){
			mfs->inode[inode]->db->data[i] = '\0';
			return;
		}

		ch = fgetc(fp);

		mfs->inode[inode]->db->data[i] = ch;
	}

	make_sid(inode);
	set_sidbit(a_i,inode);
	sin_data(a_i,inode,ftell(fp));

	return;

}

void sin_data(char a_i[30],int inode,int fpos){
	FILE *fp;
	int size,ch,k=0,n=0,m=0,z=0,a=0,loopQ,data;
	int *tmp;

	fp = fopen(a_i,"rt");
	size = mfs->inode[inode]->file_size;
	loopQ = mfs->inode[inode]->num;
	tmp = malloc(mfs->inode[inode]->numS * sizeof(int));
	tmp = get_sidbit(inode);

	fseek(fp,fpos,SEEK_SET);

	while(n<loopQ){

		data = tmp[m++];

		for(int i=0;i<128;i++){

			if(ftell(fp) == size){
				mfs->block[data]->data[i] = '\0';
				return;
			}

			ch = fgetc(fp);
			mfs->block[data]->data[i] = ch;
		}

		n++;
	}

	make_did(inode);
	set_didbit(inode);
	set_dsidbit(inode);
	dsin_data(a_i,inode,ftell(fp));
	return;

}

void dsin_data(char a_i[30],int inode,int fpos){
	FILE *fp;
	int size,ch,k=0,n=0,m=0,z=0,loopQ,loopP,data,x;
	int *tmp;

	fp = fopen(a_i,"rt");

	size = mfs->inode[inode]->file_size;

	loopQ = mfs->inode[inode]->num;
	loopP = mfs->inode[inode]->numP;

	x = (( (loopQ-1) * 102) + loopP );

	tmp = malloc( ((loopQ-1) * 102 + loopP) * sizeof(int));

	tmp = get_dsidbit(inode);

	fseek(fp,fpos,SEEK_SET);

	while(n<x){

		data = tmp[m++];

		for(int i=0;i<128;i++){

			if(ftell(fp) == size){
				mfs->block[data]->data[i] = '\0';
				return;
			}

			ch = fgetc(fp);

			mfs->block[data]->data[i] = ch;
		}

		n++;
	}


}
void in_dir(char name[5],int inode){
	char tmp[128],tmp2[128];

	sprintf(tmp,"%-7s",cur->p->db->data);

	sprintf(cur->p->db->data,"%-3d%-4s",inode,name);
	strcat(tmp,cur->p->db->data);

	sprintf(cur->p->db->data,"%-7s\0",tmp);

}

// ok
void mymkdir(char name[5]){
	int inode,data;
	char tmp[3];

	inode = i_bit_check();
	data = d_bit_check();

	if(cur->left == NULL){
		struct dtree *new = malloc(sizeof(struct dtree));

		mfs->inode[inode] = malloc(sizeof(struct inode));

		strcpy(new->name,name);
		new->inode = inode;
		new->p = mfs->inode[inode];
		new->left = NULL;
		new->right = NULL;

		mfs->inode[inode]->file_type = 1;
		cal_date(inode);
		mfs->inode[inode]->file_size = 0;
		mfs->inode[inode]->dir = data;
		mfs->inode[inode]->sin = -1;
		mfs->inode[inode]->dou = -1;
		mfs->inode[inode]->num = 0;

		mfs->block[data] = malloc(sizeof(struct d_block));
		mfs->inode[inode]->db = mfs->block[data];

		for(int i=0;i<128;i++)
			mfs->block[data]->data[i] = '\0';

		sprintf(mfs->block[data]->data,"%-3d%-4s%-3d%-4s",inode,".",cur->p->dir,"..");

		cur->left = new;

		i_bit_insert();
		d_bit_insert();

		in_dir(name,inode);

	}else{
		struct dtree *new = malloc(sizeof(struct dtree));
		struct dtree *temp = cur->left;

		while(temp->right!=NULL)
			temp = temp->right;

		mfs->inode[inode] = malloc(sizeof(struct inode));

		strcpy(new->name,name);
		new->inode = inode;
		new->p = mfs->inode[inode];
		new->left = NULL;
		new->right = NULL;

		mfs->inode[inode]->file_type = 1;
		cal_date(inode);
		mfs->inode[inode]->file_size = 0;
		mfs->inode[inode]->dir = data;
		mfs->inode[inode]->sin = -1;
		mfs->inode[inode]->dou = -1;
		mfs->inode[inode]->num = 0;

		mfs->block[data] = malloc(sizeof(struct d_block));
		mfs->inode[inode]->db = mfs->block[data];

		for(int i=0;i<128;i++)
			mfs->block[data]->data[i] = '\0';

		sprintf(mfs->block[data]->data,"%-3d%-4s%-3d%-4s",inode,".",cur->p->dir,"..");

		temp->right = new;

		i_bit_insert();
		d_bit_insert();

		in_dir(name,inode);
	}

}

void myls(char a_i2[30]){
	int s=0,k=0,inode;
	char tmp[5],tmp1[4],type[2];
	struct dtree *p = cur->left;


	if(!strcmp(a_i2,"-i")){
		while(s<13){
			k=0;
			for(int i=s;i<s+3;i++){
				tmp1[k] = cur->p->db->data[i];
				k++;
			}
			k=0;
			for(int j=s+3;j<s+7;j++){
				tmp[k] = cur->p->db->data[j]; 
				k++;
			}
			inode = atoi(tmp1);
			printf("%d %s\n",inode,tmp);
			s+= 7;
		}

		if(cur->left !=NULL){
			while(1){
				k=0;
				for(int i=s;i<s+3;i++){
					tmp1[k] = cur->p->db->data[i];
					k++;
				}
				k=0;
				for(int j=s+3;j<s+7;j++){
					tmp[k] = cur->p->db->data[j]; 
					k++;
				}
				inode = atoi(tmp1);
				printf("%d %s\n",inode,tmp);
				if(p->right==NULL)
					break;
				s+= 7;
				p = p->right;
			}
		}else{
			return;
		}

		return;
	}

	if(!strcmp(a_i2,"-l")){
		while(s<13){
			k=0;
			for(int i=s;i<s+3;i++){
				tmp1[k] = cur->p->db->data[i];
				k++;
			}
			k=0;
			for(int j=s+3;j<s+7;j++){
				tmp[k] = cur->p->db->data[j]; 
				k++;
			}
			inode = atoi(tmp1);
			if(mfs->inode[inode]->file_type == 1){
				sprintf(type,"%s","d");
			}else if(mfs->inode[inode]->file_type == 0){
				sprintf(type,"%s","-");
			}

			printf("%s %d %s %s\n",type,mfs->inode[inode]->file_size,mfs->inode[inode]->file_date,tmp);

			s+= 7;
		}

		if(cur->left !=NULL){
			while(1){
				k=0;
				for(int i=s;i<s+3;i++){
					tmp1[k] = cur->p->db->data[i];
					k++;
				}
				k=0;
				for(int j=s+3;j<s+7;j++){
					tmp[k] = cur->p->db->data[j]; 
					k++;
				}
				inode = atoi(tmp1);

				if(mfs->inode[inode]->file_type == 1){
					sprintf(type,"%s","d");
				}else if(mfs->inode[inode]->file_type == 0){
					sprintf(type,"%s","-");
				}

				printf("%s %d %s %s\n",type,mfs->inode[inode]->file_size,mfs->inode[inode]->file_date,tmp);
				if(p->right==NULL)
					break;
				s+= 7;
				p = p->right;
			}
		}else{
			return;
		}

		return;
	}

	if( (!strcmp(a_i2,"-li")) || (!strcmp(a_i2,"-il")) ){

		while(s<13){
			k=0;
			for(int i=s;i<s+3;i++){
				tmp1[k] = cur->p->db->data[i];
				k++;
			}
			k=0;
			for(int j=s+3;j<s+7;j++){
				tmp[k] = cur->p->db->data[j]; 
				k++;
			}
			inode = atoi(tmp1);
			if(mfs->inode[inode]->file_type == 1){
				sprintf(type,"%s","d");
			}else if(mfs->inode[inode]->file_type == 0){
				sprintf(type,"%s","-");
			}

			printf("%d %s %d %s %s\n",inode,type,mfs->inode[inode]->file_size,mfs->inode[inode]->file_date,tmp);

			s+= 7;
		}

		if(cur->left !=NULL){
			while(1){
				k=0;
				for(int i=s;i<s+3;i++){
					tmp1[k] = cur->p->db->data[i];
					k++;
				}
				k=0;
				for(int j=s+3;j<s+7;j++){
					tmp[k] = cur->p->db->data[j]; 
					k++;
				}
				inode = atoi(tmp1);

				if(mfs->inode[inode]->file_type == 1){
					sprintf(type,"%s","d");
				}else if(mfs->inode[inode]->file_type == 0){
					sprintf(type,"%s","-");
				}

				printf("%d %s %d %s %s\n",inode,type,mfs->inode[inode]->file_size,mfs->inode[inode]->file_date,tmp);
				if(p->right==NULL)
					break;
				s+= 7;
				p = p->right;
			}
		}else{
			return;
		}

		return;
	}

	struct dtree *ioi = cur->left;

	while(ioi != NULL){
		printf("%s\n",ioi->name);
		ioi = ioi->right;
	}
	/*
	while(s<13){
		k=0;
		for(int i=s;i<s+3;i++){
		}
		for(int j=s+3;j<s+7;j++){
			tmp[k] = cur->p->db->data[j]; 
			k++;
		}
		printf("%s ",tmp);
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
			printf("%s ",tmp);
			if(p->right==NULL)
				break;
			s+= 7;
			p = p->right;
		}
	}else{
		return;
	}
	*/
}

int mycd(char *a_i){
	int s=0,k,ck=999,n=0;
	char tmp[5],tmp2[40];
	struct dtree *p = cur->left;

	if(a_i == NULL){
		sprintf(path,"%s","/");
		cur = D;
		return 1;
	}

	if(a_i[0] == '/'){
		sprintf(path,"%s","/");
		cur = D;
		return 1;
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
			strcpy(path,backup2);
			return 1;
		}

		if(ck == 0)
			return 0;

		// 미 구현

		if(ck == 1){
			printf("요기 실행\n");
			cur = back;
			sprintf(path,"%s",backup);
			return 3;
		}


		for(int i=0;i<ck-2;i++){
			p = p->right;
		}

		if(p->p->file_type == 0){
			printf("잘못된 경로 입니다.\n");
			strcpy(path,backup2);
			return 1;
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

int mycd_func(char a_i[30]){
	int s;
	char *line;
	char tmp;

	line = strtok(a_i,"/");

	strcpy(backup2,path);

	back2 = cur;

	if(a_i[0] == '/'){
		mycd(a_i);
	}


	while(line != NULL){
		// 여기에 추가 하면 될듯!
		s =	mycd(line);
		printf("%s\n",line);

		if(s == 1){
			cur = back2;
			return 1;
		}

		line = strtok(NULL,"/");
	}

}

// ok
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
				if(cur->left->p->db->data[14] >= '0' && cur->left->p->db->data[14] <= '9'){
					printf("빈 디렉터리가 아닙니다.\n");
					return;
				}

				for(int i=0;i<ck*7;i++){
					tmp2[i] = cur->p->db->data[i];
				}
				for(int i=ck*7;i<(ck*7)+3;i++){
					tmp5[i-(ck*7)]= cur->p->db->data[i];
				}
				id = atoi(tmp5);
				if(mfs->inode[id]->file_type == 0){
					printf("디렉터리가 아닙니다.\n");
					return;
				}
				d_bit_del(id);
				i_bit_del(id);
				for(int i=(ck*7)+7;i<(l*7);i++){
					tmp3[i] = cur->p->db->data[i];
				}
				sprintf(tmp4,"%s%s",tmp2,tmp3);
				sprintf(cur->p->db->data,"%s",tmp4);
				cur->left = NULL;
				return;
			}
			t = cur->left;

			if(t->p->db->data[14] >= '0' && t->p->db->data[14] <= '9'){
				printf("%s\n",t->p->db->data);
				printf("빈 디렉터리가 아닙니다.\n");
				return;
			}

			for(int i=0;i<ck*7;i++){
				tmp2[i] = cur->p->db->data[i];
			}
			for(int i=ck*7;i<(ck*7)+3;i++){
				tmp5[i-(ck*7)]= cur->p->db->data[i];
			}
			id = atoi(tmp5);
			if(mfs->inode[id]->file_type == 0){
				printf("디렉터리가 아닙니다.\n");
				return;
			}
			d_bit_del(id);
			i_bit_del(id);
			for(int i=(ck*7)+7;i<(l*7)+7;i++){
				tmp3[m] = cur->p->db->data[i];
				m++;
			}
			sprintf(tmp4,"%s%s",tmp2,tmp3);
			sprintf(cur->p->db->data,"%s",tmp4);
			cur->left = cur->left->right;
			t->right = NULL;
			free(t);
			t = NULL; // 추가
			return;
		}

		for(int i=0;i<ck-3;i++){
			p = p->right;
		}

		if(p->right->p->db->data[14] >= '0' && p->right->p->db->data[14] <= '9'){
			printf("빈 디렉터리가 아닙니다.\n");
			return;
		}

		for(int i=0;i<ck*7;i++){
			tmp2[i] = cur->p->db->data[i];
		}
		for(int i=ck*7;i<(ck*7)+3;i++){
			tmp5[i-(ck*7)]= cur->p->db->data[i];
		}
		id = atoi(tmp5);
		if(mfs->inode[id]->file_type == 0){
			printf("디렉터리가 아닙니다.\n");
			return;
		}
		d_bit_del(id);
		i_bit_del(id);
		for(int i=(ck*7)+7;i<(l*7)+7;i++){
			tmp3[m] = cur->p->db->data[i];
			m++;
		}
		sprintf(tmp4,"%s%s",tmp2,tmp3);
		sprintf(cur->p->db->data,"%s",tmp4);
		back = p->right;
		p->right = p->right->right;
		back->right=NULL;
		free(back);

	}

}
/*
   void myrmdir_func(char a_i[30]){
   char *line,*line2;
   char tmp[30],tmp2[100][5];
   int s=0,i=0,a=0;

   line = strtok(a_i,"/");

   strcpy(tmp,a_i);

   while(line != NULL){


   sprintf(tmp2[s],"%s",line);
   line = strtok(NULL,"/");
   s++;
   }

   strcpy(backup2,path);

   back2 = cur;

   if(a_i[0] == '/'){
   mycd(a_i);
   }

   while(i<s){

   if(i==s-1){
   myrmdir(tmp2[i]);
   cur = back2;
   strcpy(path,backup2);
   return;
   }

   mycd(tmp2[i]);

   i++;

   }


   }
 */

void mytree(struct dtree *tr,int d){

	if(tr != NULL){
		for(int i=0;i<d;i++)
			printf("--");
		if(d!=0)
			printf("* ");
		printf("%s\n",tr->name);
		mytree(tr->left,d+1);
		mytree(tr->right,d);
	}

}

void mytree_func(struct dtree *tr,int d){


	if(tr != NULL){
		for(int i=0;i<d;i++)
			printf("--");
		if(d!=0)
			printf("* ");
		printf("%s\n",tr->name);
		mytree_func(tr->left,d+1);
		mytree_func(tr->right,d);
	}

}

void myshowinode(char a_i[30]){
	int num,chk;

	chk = a_i[0];

	if(!(chk >= 48 && chk <= 57)){
		printf("숫자를 입력해 주세요.\n");
		return;
	}

	num = atoi(a_i);

	if(mfs->inode[num] == NULL){
		printf("해당 아이노드는 사용하고 있지 않습니다.\n");
		return;
	}else{
		if(mfs->inode[num]->file_type == 1)
			printf("file type : directory file\n");
		else
			printf("file type : regular file\n");
		printf("file size : %d byte\n", mfs->inode[num]->file_size);
		printf("modified time : %s\n",mfs->inode[num]->file_date);
		printf("data block list : %d, %d, %d\n", mfs->inode[num]->dir, mfs->inode[num]->sin, mfs->inode[num]->dou);
	}

}

void myshowblock(char a_i[30]){
	int num,chk,i=0;

	chk = a_i[0];

	if(!(chk >= 48 && chk <= 57)){
		printf("숫자를 입력해 주세요.\n");
		return;
	}


	num = atoi(a_i);

	if(mfs->block[num] == NULL){
		printf("해당 블럭은 사용하고 있지 않습니다.\n");
		return;
	}else{
		while(mfs->block[num]->data[i] != '\0'){
			printf("%c",mfs->block[num]->data[i]);
			i++;
		}
	}

}

void mystate(){
	unsigned mask = 1;
	int ivalue=0;

	for(int i=0;i<16;i++){
		mask = 1;
		mask <<= 31; // mask 1로 초기화
		for(int j=0;j<32;j++){
			// 비트열 확인
			if((mfs->super[i].imask & mask) == 0){
				ivalue++;
			}else{
				mask >>=1;
			}
		}
	}

	unsigned mask1 = 1,mask2 = 1;
	int dvalue = 0;

	for(int i=0;i<16;i++){
		mask1 = 1, mask2 = 1;
		mask1 <<= 31, mask2 <<=31;
		for(int j=0;j<64;j++){
			// 비트열 확인
			if((mfs->super[i].dmask1 & mask1) == 0 && j<32){
				dvalue++;
			}else if(j<32){
				mask1 >>=1;
			}

			if((mfs->super[i].dmask2 & mask2) == 0 && j>31){
				dvalue++; 
			}else if(j>31){
				mask2 >>=1;
			}
		}

	}

	printf("free inode : %d\n",ivalue);
	printf("free data block : %d\n",dvalue);

}

void mycpfrom(char a_i[20],char name[5]){

	int inode,data,size;
	char tmp[3];
	FILE *fp;

	fp = fopen(a_i,"rt");

	if(fp == NULL){
		printf("정확한 파일명이 아닙니다.\n");
		return;
	}

	size = cal_size(a_i);
	inode = i_bit_check();
	data = d_bit_check();

	if(cur->left == NULL){
		struct dtree *new = malloc(sizeof(struct dtree));

		mfs->inode[inode] = malloc(sizeof(struct inode));

		strcpy(new->name,name);
		new->p = mfs->inode[inode];
		new->inode = inode;
		new->left = NULL;
		new->right = NULL;

		mfs->inode[inode]->file_type = 0;
		cal_date(inode);
		mfs->inode[inode]->file_size = size;

		mfs->inode[inode]->dir = data;
		mfs->inode[inode]->sin = -1;
		mfs->inode[inode]->dou = -1;

		mfs->inode[inode]->num = 0;

		mfs->block[data] = malloc(sizeof(struct d_block));
		mfs->inode[inode]->db = mfs->block[data];

		// 데이터블럭 링크 

		for(int i=0;i<128;i++)
			mfs->block[data]->data[i] = '\0';


		i_bit_insert();
		d_bit_insert();

		in_data(a_i,inode);
		in_dir(name,inode);

		cur->left = new;

	}else{
		struct dtree *new = malloc(sizeof(struct dtree));
		struct dtree *temp = cur->left;

		while(temp->right!=NULL)
			temp = temp->right;

		mfs->inode[inode] = malloc(sizeof(struct inode));

		strcpy(new->name,name);
		new->inode = inode;
		new->p = mfs->inode[inode];
		new->left = NULL;
		new->right = NULL;

		mfs->inode[inode]->file_type = 0;
		cal_date(inode);
		mfs->inode[inode]->file_size = size;
		mfs->inode[inode]->dir = data;
		mfs->inode[inode]->sin = -1;
		mfs->inode[inode]->dou = -1;
		mfs->inode[inode]->num = 0;

		mfs->block[data] = malloc(sizeof(struct d_block));
		mfs->inode[inode]->db = mfs->block[data];

		// 데이터블럭 링크 

		for(int i=0;i<128;i++)
			mfs->block[data]->data[i] = '\0';


		i_bit_insert();
		d_bit_insert();

		in_data(a_i,inode);
		in_dir(name,inode);

		temp->right = new;

	}

}

void mytouch(char name[5]){
	int s=0,k,ck=-1,n=0,ckin= -1;
	int inode,data;
	char tmp[5],tmp1[4];
	struct dtree *p = cur->left;

	while(s<13){
		k=0;
		for(int i=s;i<s+3;i++){
			tmp1[k] = cur->p->db->data[i];
			k++;
		}
		k=0;
		for(int j=s+3;j<s+7;j++){
			tmp[k] = cur->p->db->data[j]; 
			k++;
		}
		s+= 7;
	}

	if(cur->left !=NULL){
		while(1){
			k=0;
			for(int i=s;i<s+3;i++){
				tmp1[k] = cur->p->db->data[i];
				k++;
			}
			k=0;
			for(int j=s+3;j<s+7;j++){
				tmp[k] = cur->p->db->data[j]; 
				k++;
			}
			if(!strcmp(name,tmp)){
				ckin = atoi(tmp1);
				break;
			}
			if(p->right==NULL)
				break;
			s+= 7;
			p = p->right;
		}
	}

	if(ckin == -1){

		inode = i_bit_check();
		data = d_bit_check();

		if(cur->left == NULL){
			struct dtree *new = malloc(sizeof(struct dtree));

			mfs->inode[inode] = malloc(sizeof(struct inode));

			strcpy(new->name,name);
			new->inode = inode;
			new->p = mfs->inode[inode];
			new->left = NULL;
			new->right = NULL;

			mfs->inode[inode]->file_type = 0;
			cal_date(inode);
			mfs->inode[inode]->file_size = 0;

			mfs->inode[inode]->dir = data;
			mfs->inode[inode]->sin = -1;
			mfs->inode[inode]->dou = -1;

			mfs->inode[inode]->num = 0;

			mfs->block[data] = malloc(sizeof(struct d_block));
			mfs->inode[inode]->db = mfs->block[data];

			// 데이터블럭 링크 

			for(int i=0;i<128;i++)
				mfs->block[data]->data[i] = '\0';


			i_bit_insert();
			d_bit_insert();

			in_dir(name,inode);

			cur->left = new;

		}else{
			struct dtree *new = malloc(sizeof(struct dtree));
			struct dtree *temp = cur->left;

			while(temp->right!=NULL)
				temp = temp->right;

			mfs->inode[inode] = malloc(sizeof(struct inode));

			strcpy(new->name,name);
			new->inode = inode;
			new->p = mfs->inode[inode];
			new->left = NULL;
			new->right = NULL;

			mfs->inode[inode]->file_type = 0;
			cal_date(inode);
			mfs->inode[inode]->file_size = 0;
			mfs->inode[inode]->dir = data;
			mfs->inode[inode]->sin = -1;
			mfs->inode[inode]->dou = -1;
			mfs->inode[inode]->num = 0;

			mfs->block[data] = malloc(sizeof(struct d_block));
			mfs->inode[inode]->db = mfs->block[data];

			// 데이터블럭 링크 

			for(int i=0;i<128;i++)
				mfs->block[data]->data[i] = '\0';


			i_bit_insert();
			d_bit_insert();

			in_dir(name,inode);

			temp->right = new;

		}

	}else{
		// 시간 함수
		cal_date(ckin);

	}


}

void mycat(char a_i[30]){
	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

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
	/////////////////////////////////////////////////

	if(ck == 999){
		printf("잘못된 경로 입니다.\n");
		return;
	}

	if(ck == 2){
		if(l == 2){

			for(int i=ck*7;i<(ck*7)+3;i++){
				tmp5[i-(ck*7)]= cur->p->db->data[i];
			}
			id = atoi(tmp5);

		}

		for(int i=ck*7;i<(ck*7)+3;i++){
			tmp5[i-(ck*7)]= cur->p->db->data[i];
		}
		id = atoi(tmp5);

	}


	for(int i=ck*7;i<(ck*7)+3;i++){
		tmp5[i-(ck*7)]= cur->p->db->data[i];
	}
	id = atoi(tmp5);

	/////////////////////////////////////////////////

	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		for(int i=0;i<mfs->inode[id]->file_size;i++)
			printf("%c",mfs->inode[id]->db->data[i]);

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);

		//여기서 출력

		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<mfs->inode[id]->numS+1;i++)
			temp[i] = num[i-1];

		//////////////////////////////////////////

		struct btree *head = NULL;
		struct btree *tail = NULL;

		b = 0;

		while(temp[b] != 0){

			struct btree *new = malloc(sizeof(struct btree));

			new->p = mfs->block[temp[b]];
			new->next = NULL;

			if(head == NULL && tail == NULL)
				head = tail = new;
			else{
				tail->next = new;
				tail = new;
			}

			b++;

		}

		struct btree *print = head;

		while(print != NULL){
			for(int i=0;i<128;i++)
				printf("%c",print->p->data[i]);
			print = print->next;
		}

		return;
	}

	int loopQ,loopP;

	loopQ = mfs->inode[id]->num;
	loopP = mfs->inode[id]->numP;

	num = malloc(102 * sizeof(int));

	num = get_sidbit_ADD(id);

	num2 = malloc( ( ((loopQ - 1) * 102) + loopP +1 ) * sizeof(int));
	for(int i=0;i< (((loopQ - 1) * 102) + loopP) +1;i++)
		num2[i] = 0;

	num2 = get_dsidbit(id);

	int b=0;

	temp[0] = mfs->inode[id]->dir;

	for(int i=1;i<103;i++)
		temp[i] = num[i-1];

	while(1){
		if(num2[b] == 0)
			break;

		temp[b+103] = num2[b];

		b++;
	}

	//////////////////////////////////////////

	struct btree *head = NULL;
	struct btree *tail = NULL;

	b = 0;


	while(temp[b] != 0){

		struct btree *new = malloc(sizeof(struct btree));

		new->p = mfs->block[temp[b]];
		new->next = NULL;

		if(head == NULL && tail == NULL)
			head = tail = new;
		else{
			tail->next = new;
			tail = new;
		}

		b++;
	}

	struct btree *print = head;

	while(print != NULL){
		for(int i=0;i<128;i++)
			printf("%c",print->p->data[i]);
		print = print->next;
	}


	num = NULL;
	num2 = NULL;

}

void myshowfile(char a_i[30],char a_i2[30],char a_i3[30]){
	int num1,num2;

	num1 = atoi(a_i);
	num2 = atoi(a_i2);
}

void mycpto(char a_i[30],char a_i2[30]){
	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	FILE *fp;

	fp = fopen(a_i2,"wt");

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
	/////////////////////////////////////////////////

	if(ck == 999){
		printf("잘못된 경로 입니다.\n");
		return;
	}

	if(ck == 2){
		if(l == 2){

			for(int i=ck*7;i<(ck*7)+3;i++){
				tmp5[i-(ck*7)]= cur->p->db->data[i];
			}
			id = atoi(tmp5);

		}

		for(int i=ck*7;i<(ck*7)+3;i++){
			tmp5[i-(ck*7)]= cur->p->db->data[i];
		}
		id = atoi(tmp5);

	}


	for(int i=ck*7;i<(ck*7)+3;i++){
		tmp5[i-(ck*7)]= cur->p->db->data[i];
	}
	id = atoi(tmp5);

	/////////////////////////////////////////////////

	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		for(int i=0;i<mfs->inode[id]->file_size;i++)
			printf("%c",mfs->inode[id]->db->data[i]);

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);

		//여기서 출력

		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<mfs->inode[id]->numS+1;i++)
			temp[i] = num[i-1];

		//////////////////////////////////////////

		struct btree *head = NULL;
		struct btree *tail = NULL;

		b = 0;

		while(temp[b] != 0){

			struct btree *new = malloc(sizeof(struct btree));

			new->p = mfs->block[temp[b]];
			new->next = NULL;

			if(head == NULL && tail == NULL)
				head = tail = new;
			else{
				tail->next = new;
				tail = new;
			}

			b++;

		}

		struct btree *print = head;

		while(print != NULL){
			for(int i=0;i<128;i++)
				printf("%c",print->p->data[i]);
			print = print->next;
		}

		return;
	}

	int loopQ,loopP;

	loopQ = mfs->inode[id]->num;
	loopP = mfs->inode[id]->numP;

	num = malloc(102 * sizeof(int));

	num = get_sidbit_ADD(id);

	num2 = malloc( ( ((loopQ - 1) * 102) + loopP ) * sizeof(int));

	num2 = get_dsidbit(id);

	int b=0;

	temp[0] = mfs->inode[id]->dir;

	for(int i=1;i<103;i++)
		temp[i] = num[i-1];

	while(1){
		if(num2[b] == 0)
			break;

		temp[b+103] = num2[b];

		b++;
	}

	//////////////////////////////////////////

	struct btree *head = NULL;
	struct btree *tail = NULL;

	b = 0;

	while(temp[b] != 0){

		struct btree *new = malloc(sizeof(struct btree));

		new->p = mfs->block[temp[b]];
		new->next = NULL;

		if(head == NULL && tail == NULL)
			head = tail = new;
		else{
			tail->next = new;
			tail = new;
		}

		b++;
	}

	struct btree *print = head;

	int tmmp,v=0;

	while(print != NULL){
		v = 0;
		while(print->p->data[v] != '\0'){
			tmmp = print->p->data[v];

			fputc(tmmp,fp);
			v++;
		}
		print = print->next;
	}

	num = NULL;
	num2 = NULL;
}

void mymv(char a_i[20], char a_i2[20]){
	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4],tmp6[20];
	int pp=0,ppp=0;
	struct dtree *bback = malloc(sizeof(struct dtree));
	struct dtree *p = cur->left;

	pp = mymv_func(a_i);

	for(int i=0;i<100;i++)
		tmp2[i] = '\0';

	for(int i=0;i<100;i++)
		tmp3[i] = '\0';
	for(int i=0;i<4;i++)
		tmp5[i] = '\0';

	printf(" pp = %d\n",pp);

	if(pp == 999){

		printf("파일명이 정확하지 않습니다.\n");
		return;

	}else{

		ppp = mymv_func(a_i2);

		printf(" ppp = %d\n",ppp);
		if(ppp == 999){
			//존재 x 파일명 바꾸기

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
			/////////////////////////////////////////////////

			if(ck == 999){
				printf("잘못된 경로 입니다.\n");
				return;
			}
			struct dtree *p = cur->left;

			for(int i=0;i<ck-2;i++)
				p = p->right;

			if(ck == 2){
				if(l == 2){
					for(int i=0;i<ck*7;i++){
						tmp2[i] = cur->p->db->data[i];
					}
					for(int i=ck*7;i<(ck*7)+3;i++){
						tmp5[i-(ck*7)]= cur->p->db->data[i];
					}
					for(int i=(ck*7)+7;i<(l*7)+7;i++){
						tmp3[i] = cur->p->db->data[i];
					}
					id = atoi(tmp5);
					sprintf(tmp4,"%s%-3d%-4s%s",tmp2,id,a_i2,tmp3);
					sprintf(cur->p->db->data,"%s",tmp4);
					sprintf(p->right->name,"%s",a_i2);
					return;
				}

				for(int i=0;i<ck*7;i++){
					tmp2[i] = cur->p->db->data[i];
				}
				for(int i=ck*7;i<(ck*7)+3;i++){
					tmp5[i-(ck*7)]= cur->p->db->data[i];
				}
				for(int i=(ck*7)+7;i<(l*7)+7;i++){
					tmp3[m] = cur->p->db->data[i];
					m++;
				}
				id = atoi(tmp5);
				sprintf(tmp4,"%s%-3d%-4s%s",tmp2,id,a_i2,tmp3);
				sprintf(cur->p->db->data,"%s",tmp4);
				sprintf(p->right->name,"%s",a_i2);
				return;
			}

			for(int i=0;i<ck*7;i++){
				tmp2[i] = cur->p->db->data[i];
			}
			for(int i=ck*7;i<(ck*7)+3;i++){
				tmp5[i-(ck*7)]= cur->p->db->data[i];
			}
			for(int i=(ck*7)+7;i<(l*7)+7;i++){
				tmp3[m] = cur->p->db->data[i];
				m++;
			}
			id = atoi(tmp5);
			sprintf(tmp4,"%s%-3d%4s%s",tmp2,id,a_i2,tmp3);
			sprintf(cur->p->db->data,"%s",tmp4);
			sprintf(p->right->name,"%s",a_i2);
			return;

		}else{
			int id2;
			char tmp0[30],tmp11[30],tmp22[30],tmp33[30],tmp44[30];
			// 존재 o 파일 옮기기
			// 파일이면 리턴, 폴더면 폴더 들어감
			while(1){
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
				/////////////////////////////////////////////////

				if(ck == 999){
					printf("잘못된 경로 입니다.\n");
					return;
				}

				if(ck == 2){
					if(l == 2){
						for(int i=0;i<ck*7;i++){
							tmp11[i] = cur->p->db->data[i];
						}
						for(int i=ck*7;i<(ck*7)+3;i++){
							tmp22[i-(ck*7)]= cur->p->db->data[i];
						}
						for(int i=(ck*7)+7;i<(l*7)+7;i++){
							tmp33[i] = cur->p->db->data[i];
						}
						id2 = atoi(tmp22);
						sprintf(tmp44,"%s%s",tmp11,tmp33);
						break;
					}

					for(int i=0;i<ck*7;i++){
						tmp11[i] = cur->p->db->data[i];
					}
					for(int i=ck*7;i<(ck*7)+3;i++){
						tmp22[i-(ck*7)]= cur->p->db->data[i];
					}
					for(int i=(ck*7)+7;i<(l*7)+7;i++){
						tmp33[m] = cur->p->db->data[i];
						m++;
					}
					id2 = atoi(tmp22);
					sprintf(tmp44,"%s%s",tmp11,tmp33);
					break;
				}

				for(int i=0;i<ck*7;i++){
					tmp11[i] = cur->p->db->data[i];
				}
				for(int i=ck*7;i<(ck*7)+3;i++){
					tmp22[i-(ck*7)]= cur->p->db->data[i];
				}
				for(int i=(ck*7)+7;i<(l*7)+7;i++){
					tmp33[m] = cur->p->db->data[i];
					m++;
				}
				id = atoi(tmp22);
				sprintf(tmp44,"%s%s",tmp11,tmp33);
				break;
			}
			// 폴더 들어가고 아이노드 번호 쓰고 현재 폴더에 아이노드 번호 및 이름 삭제
			printf("여기 안들어오는거같은데?\n");
			n = 0;
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

				if(!strcmp(a_i2,tmp)){
					ck = n;
				}

				if(cur->p->db->data[s+7] == '\0'){	
					l = n;
					break;
				}
				s+=7;
				n++;
			}
			/////////////////////////////////////////////////

			if(ck == 999){
				printf("잘못된 경로 입니다.\n");
				return;
			}

			while(1){

				if(ck == 2){
					if(l == 2){
						for(int i=0;i<ck*7;i++){
							tmp2[i] = cur->p->db->data[i];
						}
						for(int i=ck*7;i<(ck*7)+3;i++){
							tmp5[i-((ck*7))]= cur->p->db->data[i];
						}
						for(int i=(ck*7)+7;i<(l*7);i++){
							tmp3[i] = cur->p->db->data[i];
						}
						id = atoi(tmp5);

						int ppck;

						sprintf(backup4,"%s",path);

						back4 = cur;

						sprintf(a_i2,"%s",a_i2);
						ppck = mycd_func(a_i2);

						if(ppck == 1){
							return;
						}

						if(cur->left == NULL){
							cur->left = bback;
							in_dir(a_i,id2);
						}else{
							struct dtree *temp = cur->left;

							while(temp->right != NULL)
								temp = temp->right;

							temp->right = bback;

							in_dir(a_i,id2);
						}


						bback = cur->left;
						bback->right = NULL;

						cur->left = NULL;

						cur = back4;

						sprintf(path,"%s",backup4);
						sprintf(cur->p->db->data,"%s",tmp44);
						break;
					}

					for(int i=0;i<ck*7;i++){
						tmp2[i] = cur->p->db->data[i];
					}
					for(int i=ck*7;i<(ck*7)+3;i++){
						tmp5[i-(ck*7)]= cur->p->db->data[i];
					}
					for(int i=ck*7+3;i<(ck*7)+7;i++){
						tmp6[i-((ck*7)+3)]= cur->p->db->data[i];
					}
					for(int i=(ck*7)+7;i<(l*7)+7;i++){
						tmp3[m] = cur->p->db->data[i];
						m++;
					}
					id = atoi(tmp5);

					int ppck;

					sprintf(backup4,"%s",path);

					back4 = cur;

					sprintf(a_i2,"%s",a_i2);
					ppck = mycd_func(a_i2);

					if(ppck == 1){
						return;
					}


					if(cur->left == NULL){
						cur->left = bback;
						in_dir(a_i,id2);
					}else{
						struct dtree *temp = cur->left;

						while(temp->right != NULL)
							temp = temp->right;

						temp->right = bback;

						in_dir(a_i,id2);
					}

					struct dtree *t;

					t = cur->left;

					bback = cur->left;
					bback->right = NULL;

					cur->left = cur->left->right;
					t->right = NULL;
					free(t);
					t = NULL;

					cur = back4;

					sprintf(path,"%s",backup4);
					sprintf(cur->p->db->data,"%s",tmp44);

					break;
				}

				for(int i=0;i<ck-3;i++){
					p = p->right;
				}

				for(int i=0;i<ck*7;i++){
					tmp2[i] = cur->p->db->data[i];
				}
				for(int i=ck*7;i<(ck*7)+3;i++){
					tmp5[i-(ck*7)]= cur->p->db->data[i];
				}
				for(int i=ck*7+3;i<(ck*7)+7;i++){
					tmp6[i-((ck*7)+3)]= cur->p->db->data[i];
				}
				for(int i=(ck*7)+7;i<(l*7)+7;i++){
					tmp3[m] = cur->p->db->data[i];
					m++;
				}
				id = atoi(tmp5);

				int ppck;

				sprintf(backup4,"%s",path);

				back4 = cur;
				sprintf(a_i2,"%s",a_i2);

				ppck = mycd_func(a_i2);

				if(ppck == 1){
					return;
				}

				if(cur->left == NULL){
					cur->left = bback;
					in_dir(a_i,id2);
				}else{
					struct dtree *temp = cur->left;

					while(temp->right != NULL)
						temp = temp->right;

					temp->right = bback;

					in_dir(a_i,id2);
				}


				struct dtree *bbback;

				bback = p->right;
				bback->right = NULL;

				bbback = p->right;
				p->right = p->right->right;

				bbback->right = NULL;
				free(bbback);

				cur = back4;

				sprintf(path,"%s",backup4);
				sprintf(cur->p->db->data,"%s",tmp44);

				break;
			}

			// 지워주기 완료
			// 이제 파일 옮겨야 한다 -> cd로 들어간 후 맨마지막에 추가 in_dir 쓰면 될듯!
			// id = 아이노드 번호 tmp6 = 아이노드 이름
			// 연결 끊어준 후 폴더에 들어가서 연결 -> bback;

		}

	}


}

int mymv_func(char a_i[20]){
	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4],tmp6[20];
	int pp=0,ppp=0;

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

	return ck;
}

void mycp(char a_i[20],char name[5]){

	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	FILE *fp;

	fp = fopen("loperz","wt");

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
	/////////////////////////////////////////////////

	if(ck == 999){
		printf("잘못된 경로 입니다.\n");
		return;
	}

	if(ck == 2){
		if(l == 2){

			for(int i=ck*7;i<(ck*7)+3;i++){
				tmp5[i-(ck*7)]= cur->p->db->data[i];
			}
			id = atoi(tmp5);

		}

		for(int i=ck*7;i<(ck*7)+3;i++){
			tmp5[i-(ck*7)]= cur->p->db->data[i];
		}
		id = atoi(tmp5);

	}


	for(int i=ck*7;i<(ck*7)+3;i++){
		tmp5[i-(ck*7)]= cur->p->db->data[i];
	}
	id = atoi(tmp5);

	/////////////////////////////////////////////////

	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		for(int i=0;i<mfs->inode[id]->file_size;i++)
			printf("%c",mfs->inode[id]->db->data[i]);

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);

		//여기서 출력

		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<mfs->inode[id]->numS+1;i++)
			temp[i] = num[i-1];

		//////////////////////////////////////////

		struct btree *head = NULL;
		struct btree *tail = NULL;

		b = 0;

		while(temp[b] != 0){

			struct btree *new = malloc(sizeof(struct btree));

			new->p = mfs->block[temp[b]];
			new->next = NULL;

			if(head == NULL && tail == NULL)
				head = tail = new;
			else{
				tail->next = new;
				tail = new;
			}

			b++;

		}

		struct btree *print = head;

		while(print != NULL){
			for(int i=0;i<128;i++)
				printf("%c",print->p->data[i]);
			print = print->next;
		}

		return;
	}

	int loopQ,loopP;

	loopQ = mfs->inode[id]->num;
	loopP = mfs->inode[id]->numP;

	num = malloc(102 * sizeof(int));

	num = get_sidbit_ADD(id);

	num2 = malloc( ( ((loopQ - 1) * 102) + loopP ) * sizeof(int));

	num2 = get_dsidbit(id);

	int b=0;

	temp[0] = mfs->inode[id]->dir;

	for(int i=1;i<103;i++)
		temp[i] = num[i-1];

	while(1){
		if(num2[b] == 0)
			break;

		temp[b+103] = num2[b];

		b++;
	}

	//////////////////////////////////////////

	struct btree *head = NULL;
	struct btree *tail = NULL;

	b = 0;

	while(temp[b] != 0){

		struct btree *new = malloc(sizeof(struct btree));

		new->p = mfs->block[temp[b]];
		new->next = NULL;

		if(head == NULL && tail == NULL)
			head = tail = new;
		else{
			tail->next = new;
			tail = new;
		}

		b++;
	}

	struct btree *print = head;

	int tmmp,v=0;

	while(print != NULL){
		v = 0;
		for(int v=0;v<128;v++){
			if(print->p->data[v] == '\0'){
				print = print->next;
				break;
			}
			tmmp = print->p->data[v];

			fputc(tmmp,fp);
		}
		print = print->next;
	}
	//파일 저장 완료
	// 다시 mycpfrom 하기

	mycpfrom("loperz",name);


	num = NULL;
	num2 = NULL;
}
