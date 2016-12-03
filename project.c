#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct d_block{
	int num; // 데이터블록 번호
	int type;
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
	struct inode inode[512];
	struct d_block *block[1024];
};

struct dtree{
	struct inode *p;
	struct dtree *left;
	struct dtree *right;
};

void prompt(char [], char [],char []);
void command(char [], char [],char []);
int i_bit_check();
int d_bit_check();
void i_bit_insert();
void d_bit_insert();
void i_bit_del(int);
void d_bit_del(int);
void cal_date(int);
int cal_size(char []);
void set_sidbit(char [],int);
unsigned* get_sidbit(int);
void make_sid(int);
void set_didbit(char [],int);
unsigned* get_didbit(int);
void set_dsidbit(char [],int);
unsigned* get_dsidbit(int);
void make_did(int);
void in_data(char [],int);
void sin_data(char [],int,int);
void in_dir(char [],int);
void mymkdir(char []);
void mycd(char []);
void myls(char []);
void myrmdir(char []);
void mytree(struct dtree *,int);
void myshowinode(char []);
void myshowblock(char []);
void mystate();
void mycpfrom(char[],char[]);
void mycat(char[]);

struct my_file_system *mfs;

struct dtree *D=NULL;
struct dtree *cur = NULL;
struct dtree *back = NULL;

char path[30]= {'\0'};
char backup[30]= {'\0'};

int main() {


	mfs = malloc(sizeof(struct my_file_system));

	char i[30], a_i[20],a_i2[20];
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
			for(int i=0;i<16;i++){
				mfs->super[i].imask = 0;
				mfs->super[i].dmask2 = 0;
				mfs->super[i].dmask1 = 0;
			}

			mfs->block[0] = malloc(sizeof(struct d_block));
			mfs->inode[0].db = mfs->block[0];
			mfs->inode[0].dir = 0;
			mfs->inode[0].sin = 0;
			mfs->inode[0].dou = 0;

			/* 데이터 블록 트리 구조 */

			for(int i=0;i<128;i++)
				mfs->block[0]->data[i] = '\0';

			sprintf(mfs->block[0]->data,"%-3d%-4s%-3d%-4s",0,".",0,"..");
			// 512abcd -> 512번 아이노드 abcd 폴더명 or 파일명

			mfs->inode[0].file_type = 1; // 파일 타입 저장 = '1' (폴더 = 1, 파일 = 0)
			cal_date(0);
			mfs->inode[0].file_size = 0; // 사이즈0 저장
			mfs->inode[0].num = 0;


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

		prompt(i,a_i,a_i2);

	}


}

void prompt(char i[30], char a_i[20],char a_i2[20]){
	int tmp;

	printf("\n[ %s ]$ ",path);


	scanf("%s",i);

	if((tmp = getchar()) == '\n'){
		a_i[0] = '\0';
		command(i,a_i,a_i2);
		return;
	}

	scanf("%s",a_i);

	if((tmp = getchar()) == '\n'){
		a_i2[0] = '\0';
		command(i,a_i,a_i2);
		return;
	}else{
		scanf("%s",a_i2);
		command(i,a_i,a_i2);
	}

}

void command(char i[30], char a_i[30],char a_i2[20]){
	char name[5];

	if(!strcmp(i,"byebye")){
		system("rm myfs");
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

void cal_date(int inode){
	struct tm *t;
	time_t timer; // 시간 측정

	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기

	sprintf(mfs->inode[inode].file_date,"%04d/%02d/%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);

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

	mfs->inode[inode].num = loopQ;

	for(int i=0;i<128;i++){
		mfs->inode[inode].sidb->data[i] &= 0;
	}

	while(m<loopQ){

		if(m == 102)
			return;

		in = d_bit_check();

		mfs->block[in] = malloc(sizeof(struct d_block));

		d_bit_insert();

		if(i%4 == 0){
			n = in >> 2;
			mfs->inode[inode].sidb->data[k++] = n;
			a = in;
			a = a & 0x3;
			a = a << 6;
			mfs->inode[inode].sidb->data[k] = a;
		}

		if(i%4 == 1){
			n = in >> 4;
			mfs->inode[inode].sidb->data[k++] |= n;
			a = in;
			a = a & 0xF;
			a = a << 4;
			mfs->inode[inode].sidb->data[k] = a;
		}

		if(i%4 == 2){
			n = in >> 6;
			mfs->inode[inode].sidb->data[k++] |= n;
			a = in;
			a = a & 0x3F;
			a = a << 2;
			mfs->inode[inode].sidb->data[k] = a;
		}

		if(i%4 ==3){
			n = in >> 8;
			mfs->inode[inode].sidb->data[k++] |= n;
			a = in;
			a = a & 0xFF;
			mfs->inode[inode].sidb->data[k] = a;
			k++;
		}

		i++;
		m++;

	}

}

unsigned *get_sidbit(int inode){
	unsigned c,n;
	int loopQ,k=0,data,i=0,a=1;

	loopQ = mfs->inode[inode].num;
	data = mfs->inode[inode].sin;

	unsigned *tmp;
	tmp = (unsigned *)malloc( (loopQ) * sizeof(unsigned));

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

	mfs->inode[inode].sidb = mfs->block[data];
	mfs->inode[inode].sin = data;

	for(int i=0;i<128;i++)
		mfs->inode[inode].sidb->data[i] = '\0'; 

	d_bit_insert();
}

void set_didbit(char a_i[20],int inode){
	unsigned in,n,a;
	int i=0,k=0,m=0,size,loopQ,loopP;

	size = cal_size(a_i);
	loopQ = ( (size-(13184)) / 13056 );
	loopP = size % 13056;

	if(loopP != 0)
		loopQ += 1;

	if(loopQ > 102){
		loopQ = 102;
	}

	mfs->inode[inode].num = loopQ;

	for(int i=0;i<128;i++){
		mfs->inode[inode].didb->data[i] = 0;
	}

	while(m<loopQ){

		if(m == 102)
			return;

		in = d_bit_check();

		mfs->block[in] = malloc(sizeof(struct d_block));

		d_bit_insert();

		if(i%4 == 0){
			n = in >> 2;
			mfs->inode[inode].didb->data[k++] = n;
			a = in;
			a = a & 0x3;
			a = a << 6;
			mfs->inode[inode].didb->data[k] = a;
		}

		if(i%4 == 1){
			n = in >> 4;
			mfs->inode[inode].didb->data[k++] |= n;
			a = in;
			a = a & 0xF;
			a = a << 4;
			mfs->inode[inode].didb->data[k] = a;
		}

		if(i%4 == 2){
			n = in >> 6;
			mfs->inode[inode].didb->data[k++] |= n;
			a = in;
			a = a & 0x3F;
			a = a << 2;
			mfs->inode[inode].didb->data[k] = a;
		}

		if(i%4 ==3){
			n = in >> 8;
			mfs->inode[inode].didb->data[k++] |= n;
			a = in;
			a = a & 0xFF;
			mfs->inode[inode].didb->data[k] = a;
			k++;
		}

		i++;
		m++;

	}

}

unsigned *get_didbit(int inode){
	unsigned c,n;
	int loopQ,k=0,data,i=0,a=1;

	loopQ = mfs->inode[inode].num;
	data = mfs->inode[inode].dou;

	unsigned *tmp;
	tmp = (unsigned *) malloc((loopQ) * sizeof(unsigned));

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

void set_dsidbit(char a_i[20],int inode){
	unsigned in,n,a;
	int i=0,k=0,m=0,z=0,p=0,size,loopQ,loopP,loopM;
	int data;

	size = cal_size(a_i);
	loopQ = mfs->inode[inode].num;

	unsigned *tmp;
	tmp = (unsigned *) malloc(sizeof( (loopQ) * sizeof(unsigned) ));

	tmp = get_didbit(inode);
	
	if(loopQ > 102){
		loopQ = 102;
	}

	printf("loopQ = %d\n",loopQ);

	while(m<loopQ){
		i=0;

		data = tmp[m];

		/*
		if( (m+1) == loopQ ){
			loopP = ((size-13184) - (13056 * m)) / 128 ;
			loopM = ((size-13184) - (13056 * m)) % 128 ;

			if(loopM != 0)
				loopP += 1;

			i=0;
			// loopP 만큼 할당해준다
			for(int p=0;p<loopP;p++){

				in = d_bit_check();

				mfs->block[in] = malloc(sizeof(struct d_block));
				for(int i=0;i<128;i++){
					mfs->block[in]->data[i] &= 0;
				}

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

				printf("%d번 데이터 블럭 할당 완료\n",in);

				i++;
			}
		}
		*/

		while(z < 102 && (m+1) != loopQ){

			in = d_bit_check();

			mfs->block[in] = malloc(sizeof(struct d_block));

			for(int i=0;i<128;i++){
				mfs->block[in]->data[i] = 0;
			}

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
			z++;

		}
		m++;
		z=0;
		i=0;
	}

}

unsigned *get_dsidbit(int inode){
	unsigned c,n;
	int loopQ,k=0,data,i=0,a=1;

	loopQ = mfs->inode[inode].num;
	data = mfs->inode[inode].dou;

	unsigned *tmp;
	tmp = (unsigned *) malloc((loopQ) * sizeof(unsigned));

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

void make_did(int inode){
	int data;

	data = d_bit_check();

	mfs->block[data] = malloc(sizeof(struct d_block));

	mfs->inode[inode].didb = mfs->block[data];
	mfs->inode[inode].dou = data;

	for(int i=0;i<128;i++)
		mfs->inode[inode].didb->data[i] = '\0';

	d_bit_insert();
}

void in_data(char a_i[20],int inode){
	FILE *fp;
	int fpos,size,ch,k=0,data;

	fp = fopen(a_i,"rt");
	size = cal_size(a_i);

	fseek(fp,0,SEEK_SET);

	while(1){

		if(ftell(fp) > 128){
			mfs->inode[inode].db->data[k] = '\0';
			make_sid(inode);
			set_sidbit(a_i,inode);
			sin_data(a_i,inode,ftell(fp));
			return;
		}
		if(ftell(fp) == size){
			mfs->inode[inode].db->data[k] = '\0';
			return;
		}

		ch = fgetc(fp);

		mfs->inode[inode].db->data[k] = ch;

		k++;

	}


}

void sin_data(char a_i[20],int inode,int fpos){
	FILE *fp;
	int size,ch,k=0,n=0,m=0,z=0,loopQ,data;
	int *tmp;

	fp = fopen(a_i,"rt");
	size = cal_size(a_i);
	loopQ = (mfs->inode[inode].num * 128);
	tmp = get_sidbit(inode);

	fseek(fp,fpos,SEEK_SET);

	while(n<loopQ){

		if(z > 101){
			mfs->block[data]->data[k] = '\0';
			make_did(inode);
			set_didbit(a_i,inode);
			set_dsidbit(a_i,inode);
			return;
		}


		if(n % 128 == 0){
			if(n/127 != 0)
				mfs->block[data]->data[k] = '\0';
			data = tmp[m++];
			k = 0;
			z++;
		}


		if(ftell(fp) == size){
			mfs->block[data]->data[k] = '\0';
			return;
		}


		ch = fgetc(fp);

		mfs->block[data]->data[k] = ch;

		k++;
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

		new->p = &mfs->inode[inode];
		new->left = NULL;
		new->right = NULL;

		mfs->inode[inode].file_type = 1;
		cal_date(inode);
		mfs->inode[inode].file_size = 0;
		mfs->inode[inode].dir = data;
		mfs->inode[inode].num = 0;

		mfs->block[data] = malloc(sizeof(struct d_block));
		mfs->inode[inode].db = mfs->block[data];

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

		new->p = &mfs->inode[inode];
		new->left = NULL;
		new->right = NULL;

		mfs->inode[inode].file_type = 1;
		cal_date(inode);
		mfs->inode[inode].file_size = 0;
		mfs->inode[inode].dir = data;
		mfs->inode[inode].num = 0;

		mfs->block[data] = malloc(sizeof(struct d_block));
		mfs->inode[inode].db = mfs->block[data];

		for(int i=0;i<128;i++)
			mfs->block[data]->data[i] = '\0';

		sprintf(mfs->block[data]->data,"%-3d%-4s%-3d%-4s",inode,".",cur->p->dir,"..");

		temp->right = new;

		i_bit_insert();
		d_bit_insert();

		in_dir(name,inode);
	}

}

// 옵션 추가
void myls(char a_i[30]){
	int s=0,k=0,inode;
	char tmp[5],tmp1[4],type[2];
	struct dtree *p = cur->left;

	if(!strcmp(a_i,"-i")){
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

	if(!strcmp(a_i,"-l")){
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
			if(mfs->inode[inode].file_type == 1){
				sprintf(type,"%s","d");
			}else if(mfs->inode[inode].file_type == 0){
				sprintf(type,"%s","-");
			}

			printf("%s %d %s %s\n",type,mfs->inode[inode].file_size,mfs->inode[inode].file_date,tmp);

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

				if(mfs->inode[inode].file_type == 1){
					sprintf(type,"%s","d");
				}else if(mfs->inode[inode].file_type == 0){
					sprintf(type,"%s","-");
				}

				printf("%s %d %s %s\n",type,mfs->inode[inode].file_size,mfs->inode[inode].file_date,tmp);
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

	if( (!strcmp(a_i,"-li")) || (!strcmp(a_i,"-il")) ){

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
			if(mfs->inode[inode].file_type == 1){
				sprintf(type,"%s","d");
			}else if(mfs->inode[inode].file_type == 0){
				sprintf(type,"%s","-");
			}

			printf("%d %s %d %s %s\n",inode,type,mfs->inode[inode].file_size,mfs->inode[inode].file_date,tmp);

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

				if(mfs->inode[inode].file_type == 1){
					sprintf(type,"%s","d");
				}else if(mfs->inode[inode].file_type == 0){
					sprintf(type,"%s","-");
				}

				printf("%d %s %d %s %s\n",inode,type,mfs->inode[inode].file_size,mfs->inode[inode].file_date,tmp);
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
}

// ok
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

void myshowinode(char a_i[30]){
	int num;

	num = atoi(a_i);

	if(mfs->inode[num].file_type == 1)
		printf("file type : directory file\n");
	else
		printf("file type : regular file\n");
	printf("file size : %d byte\n", mfs->inode[num].file_size);
	printf("modified time : %s\n",mfs->inode[num].file_date);
	printf("data block list : %d, %d, %d\n", mfs->inode[num].dir, mfs->inode[num].sin, mfs->inode[num].dou);

}

void myshowblock(char a_i[30]){
	int num;

	num = atoi(a_i);

	printf("%s\n",mfs->block[num]->data);

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

	size = cal_size(a_i);
	inode = i_bit_check();
	data = d_bit_check();

	if(cur->left == NULL){
		struct dtree *new = malloc(sizeof(struct dtree));

		new->p = &mfs->inode[inode];
		new->left = NULL;
		new->right = NULL;

		mfs->inode[inode].file_type = 0;
		cal_date(inode);
		mfs->inode[inode].file_size = size;

		mfs->inode[inode].dir = data;

		mfs->inode[inode].num = 0;

		mfs->block[data] = malloc(sizeof(struct d_block));
		mfs->inode[inode].db = mfs->block[data];

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

		new->p = &mfs->inode[inode];
		new->left = NULL;
		new->right = NULL;

		mfs->inode[inode].file_type = 0;
		cal_date(inode);
		mfs->inode[inode].file_size = size;
		mfs->inode[inode].dir = data;
		mfs->inode[inode].num = 0;

		mfs->block[data] = malloc(sizeof(struct d_block));
		mfs->inode[inode].db = mfs->block[data];

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

void mycat(char a_i[20]){

}
