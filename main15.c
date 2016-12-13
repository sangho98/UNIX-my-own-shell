/**************************************************************************
 * 팀명 : Eㅣ 없이 맑은 상호 조	 							              *
 * 팀원 : 박상호, 박채영, 배서현									      *
 * 프로젝트 기간 : 11/3 ~ 12/11										      *
 * 프로젝트 기여도 -> 박상호 : 33.34%  배서현 : 33.33%  박채영 33.33%     *
 * 프로젝트 깃허브 사이트 : https://github.com/sangho98/UNIX-my-own-shell *
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct d_block{
	int num; // 데이터블록 번호
	unsigned char data[128];
}dblock;

struct inode{			//inode구조체
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

struct s_block{				//슈퍼블록
	unsigned imask, dmask1, dmask2;
};

struct my_file_system{			//MY 파일 시스템
	unsigned b_block : 16;
	struct s_block super[16];
	struct inode *inode[512];
	struct d_block *block[1024];
};

struct dtree{ // 디렉터리 및 파일 트리 구조 (이진 트리로 구현)
	int inode;
	char name[5];
	struct inode *p;
	struct dtree *left;
	struct dtree *right;
};

struct btree{ // myrm, mycat 등의 명령어 실행 시 다이렉트, 싱글 인다이렉트 블록, 더블 인다이렉트 블록이 가리키는 데이터 블럭들 링크드 리스트로 연결
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
void d_bit_del_func(int);
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
void myls_func(char [],char []);
void swap(char **,int);
void swap2(char **,int [],int);
void myrmdir(char []);
void myrmdir_func(char []);
void mytree(struct dtree*,int);
void mytree_func(struct dtree*,int);
void myshowinode(char []);
void myshowblock(char []);
void mystate();
void mycpfrom(char[],char[]);
void mytouch(char []);
void mytouch_func(char []);
void mycat(char []);
void mycpto(char [],char []);
void mycp(char [],char []);
void mymv(char [],char []);
int mymv_func(char []);
void myshowfile(char [],char [],char []);
void myrm(char []);
void myrm_func(char []);
void myrm_func2(char [],int);

struct my_file_system *mfs;

struct dtree *D=NULL; // 루트디렉터리 부터 이진 트리
struct dtree *cur = NULL; // 현재 작업 디렉터리 가리키는 이진 트리
struct dtree *back = NULL; // 백업 트리
struct dtree *back2 = NULL; // 백업 트리
struct dtree *back3 = NULL; // 백업 트리
struct dtree *back4 = NULL; // 백업 트리

char path[100]= {'\0'}; // mypwd 실행 및 현재 작업 디렉터리 표시
char backup[100]= {'\0'}; // 백업 경로
char backup2[100]= {'\0'}; // 백업 경로
char backup3[100]= {'\0'}; // 백업 경로
char backup4[100]= {'\0'}; // 백업 경로

int main() {


	mfs = malloc(sizeof(struct my_file_system)); // my_file_system  메모리 동적 할당

	char make[10];
	FILE *ms;

	ms = fopen("myfs","rb");

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
			mfs->inode[0]->sin = -1;
			mfs->inode[0]->dou = -1;

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

		// 명령어 실행 구간

		char i[30], a_i[20],a_i2[20],a_i3[20];

		prompt(i,a_i,a_i2,a_i3);

	}


}

void prompt(char i[30], char a_i[20],char a_i2[20],char a_i3[20]){		// 명령어 입력 받는 프롬프트(쉘)
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

void command(char i[30], char a_i[30],char a_i2[20],char a_i3[20]){		//명령어 실행 함수
	char name[5];

	if(!strcmp(i,"byebye")){
		exit(1);
	}

	else if(!strcmp(i,"mymkdir")){
		if(a_i[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
			mymkdir(name);
		}else{
			printf("폴더명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"mycpfrom")){
		if(a_i2[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
			mycpfrom(a_i,name);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"mycd")){
		if(a_i[0] != '\0'){
			mycd_func(a_i);
		}else{
			mycd(NULL);
		}

	}

	else if(!strcmp(i,"myls")){
		myls_func(a_i,a_i2);
	}

	else if(!strcmp(i,"mytree")){
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

	else if(!strcmp(i,"mypwd")){
		printf("%s\n",path);
	}

	else if(!strcmp(i,"myrmdir")){
		myrmdir_func(a_i);
	}

	else if(!strcmp(i,"myshowinode")){
		myshowinode(a_i);
	}

	else if(!strcmp(i,"myshowblock")){
		myshowblock(a_i);
	}

	else if(!strcmp(i,"mystate")){
		mystate();
	}

	else if(!strcmp(i,"mycat")){
		sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
		mycat(name);
	}

	else if( !(i[0] == 'm' && i[1] == 'y') ){
		sprintf(i,"%s %s %s %s",i,a_i,a_i2,a_i3);
		system(i);
	}

	else if(!strcmp(i,"mytouch")){
		if(a_i[0] != '\0'){
			mytouch_func(a_i);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"myshowfile")){
		if(a_i3[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i3[0],a_i3[1],a_i3[2],a_i3[3]);
			myshowfile(a_i,a_i2,name);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"mycpto")){
		sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
		mycpto(name,a_i2);
	}

	else if(!strcmp(i,"mycp")){
		char name2[5];
		sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
		sprintf(name2,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
		mycp(name2,name);
		system("rm loperz");
	}

	else if(!strcmp(i,"mymv")){
		sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
		mymv(a_i,name);
	}

	else if(!strcmp(i,"mymkfs")){
		printf("myfs가 이미 존재합니다!\n");
		return;
	}

	else if(!strcmp(i,"myrm")){
		myrm_func(a_i);
	}else{

		printf("다시 입력해 주세요\n");
		return;

	}

}


int i_bit_check(){			// 아이노드 비트열 확인
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


int d_bit_check(){		//데이터 블록 비트열 확인

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

void i_bit_insert(){		//아이노트 블록 비트열 확인
	int inode;
	unsigned mask = 1;
	mask <<= 31;

	inode = i_bit_check();

	mask >>= (inode%32);
	mfs->super[inode/32].imask |= mask;

}

void d_bit_insert(){		//data블록 사용 여부 비트열 확인
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

void i_bit_del(int id){		//아이노트 블록 비트열 삭제
	unsigned mask = 1;
	mask <<= 31;

	mask >>= (id%32);
	mfs->super[id/32].imask ^= mask;

	free(mfs->inode[id]);
	mfs->inode[id] = NULL;

}

void d_bit_del(int id){		//data블록 비트열 삭제
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

	free(mfs->block[dd]);
	mfs->block[dd] = NULL;

}

void d_bit_del_func(int dd){  // data블록 비트열 삭제 (myrm 시 사용)
	unsigned mask = 1;
	mask <<= 31;

	if(dd%64 <= 31){
		mask >>= (dd%32);
		mfs->super[dd/64].dmask1 ^= mask;
	}else{
		mask >>= (dd%32);
		mfs->super[dd/64].dmask2 ^= mask;
	}

	free(mfs->block[dd]);
	mfs->block[dd] = NULL;

}

void cal_date(int inode){		//시간, 날짜 계산
	struct tm *t;
	time_t timer; // 시간 측정

	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기

	sprintf(mfs->inode[inode]->file_date,"%04d/%02d/%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);

}

int cal_size(char a_i[20]){		//사이즈 계산
	FILE *fp;

	fp = fopen(a_i,"rt");

	fseek(fp,0,SEEK_END);


	return ftell(fp);

}

void set_sidbit(char a_i[20],int inode){		//single indirect block 설정
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

int *get_sidbit(int inode){		//single indirect block안에 있는 datablock얻기 
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

int *get_sidbit_ADD(int inode){		//linked list로 데이터블록 연결할때 single indirect block안에 있는 datablock 얻기
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

void make_sid(int inode){		//single indirect block 만들기
	int data;

	data = d_bit_check();

	mfs->block[data] = malloc(sizeof(struct d_block));

	mfs->inode[inode]->sidb = mfs->block[data];
	mfs->inode[inode]->sin = data;

	for(int i=0;i<128;i++)
		mfs->inode[inode]->sidb->data[i] = '\0'; 

	d_bit_insert();

}

void set_didbit(int inode){		//double indirect block 설정
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

int *get_didbit(int inode){		//double indirect block이 가리키는 single indirect block 얻기
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

void set_dsidbit(int inode){	//double indirect block이 가리키는 single indirect block이 가리키는 datablock 설정
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

int *get_dsidbit(int inode){	//double indirect block이 가리키는 single indirect block이 가리키는 datablock얻기
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

	/**************************************************************************
 * 팀명 : Eㅣ 없이 맑은 상호 조	 							              *
 * 팀원 : 박상호, 박채영, 배서현									      *
 * 프로젝트 기간 : 11/3 ~ 12/11										      *
 * 프로젝트 기여도 -> 박상호 : 33.34%  배서현 : 33.33%  박채영 33.33%     *
 * 프로젝트 깃허브 사이트 : https://github.com/sangho98/UNIX-my-own-shell *
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct d_block{
	int num; // 데이터블록 번호
	unsigned char data[128];
}dblock;

struct inode{			//inode구조체
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

struct s_block{				//슈퍼블록
	unsigned imask, dmask1, dmask2;
};

struct my_file_system{			//MY 파일 시스템
	unsigned b_block : 16;
	struct s_block super[16];
	struct inode *inode[512];
	struct d_block *block[1024];
};

struct dtree{ // 디렉터리 및 파일 트리 구조 (이진 트리로 구현)
	int inode;
	char name[5];
	struct inode *p;
	struct dtree *left;
	struct dtree *right;
};

struct btree{ // myrm, mycat 등의 명령어 실행 시 다이렉트, 싱글 인다이렉트 블록, 더블 인다이렉트 블록이 가리키는 데이터 블럭들 링크드 리스트로 연결
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
void d_bit_del_func(int);
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
void myls_func(char [],char []);
void swap(char **,int);
void swap2(char **,int [],int);
void myrmdir(char []);
void myrmdir_func(char []);
void mytree(struct dtree*,int);
void mytree_func(struct dtree*,int);
void myshowinode(char []);
void myshowblock(char []);
void mystate();
void mycpfrom(char[],char[]);
void mytouch(char []);
void mytouch_func(char []);
void mycat(char []);
void mycpto(char [],char []);
void mycp(char [],char []);
void mymv(char [],char []);
int mymv_func(char []);
void myshowfile(char [],char [],char []);
void myrm(char []);
void myrm_func(char []);
void myrm_func2(char [],int);

struct my_file_system *mfs;

struct dtree *D=NULL; // 루트디렉터리 부터 이진 트리
struct dtree *cur = NULL; // 현재 작업 디렉터리 가리키는 이진 트리
struct dtree *back = NULL; // 백업 트리
struct dtree *back2 = NULL; // 백업 트리
struct dtree *back3 = NULL; // 백업 트리
struct dtree *back4 = NULL; // 백업 트리

char path[100]= {'\0'}; // mypwd 실행 및 현재 작업 디렉터리 표시
char backup[100]= {'\0'}; // 백업 경로
char backup2[100]= {'\0'}; // 백업 경로
char backup3[100]= {'\0'}; // 백업 경로
char backup4[100]= {'\0'}; // 백업 경로

int main() {


	mfs = malloc(sizeof(struct my_file_system)); // my_file_system  메모리 동적 할당

	char make[10];
	FILE *ms;

	ms = fopen("myfs","rb");

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
			mfs->inode[0]->sin = -1;
			mfs->inode[0]->dou = -1;

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

		// 명령어 실행 구간

		char i[30], a_i[20],a_i2[20],a_i3[20];

		prompt(i,a_i,a_i2,a_i3);

	}


}

void prompt(char i[30], char a_i[20],char a_i2[20],char a_i3[20]){		// 명령어 입력 받는 프롬프트(쉘)
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

void command(char i[30], char a_i[30],char a_i2[20],char a_i3[20]){		//명령어 실행 함수
	char name[5];

	if(!strcmp(i,"byebye")){
		exit(1);
	}

	else if(!strcmp(i,"mymkdir")){
		if(a_i[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
			mymkdir(name);
		}else{
			printf("폴더명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"mycpfrom")){
		if(a_i2[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
			mycpfrom(a_i,name);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"mycd")){
		if(a_i[0] != '\0'){
			mycd_func(a_i);
		}else{
			mycd(NULL);
		}

	}

	else if(!strcmp(i,"myls")){
		myls_func(a_i,a_i2);
	}

	else if(!strcmp(i,"mytree")){
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

	else if(!strcmp(i,"mypwd")){
		printf("%s\n",path);
	}

	else if(!strcmp(i,"myrmdir")){
		myrmdir_func(a_i);
	}

	else if(!strcmp(i,"myshowinode")){
		myshowinode(a_i);
	}

	else if(!strcmp(i,"myshowblock")){
		myshowblock(a_i);
	}

	else if(!strcmp(i,"mystate")){
		mystate();
	}

	else if(!strcmp(i,"mycat")){
		sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
		mycat(name);
	}

	else if( !(i[0] == 'm' && i[1] == 'y') ){
		sprintf(i,"%s %s %s %s",i,a_i,a_i2,a_i3);
		system(i);
	}

	else if(!strcmp(i,"mytouch")){
		if(a_i[0] != '\0'){
			mytouch_func(a_i);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"myshowfile")){
		if(a_i3[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i3[0],a_i3[1],a_i3[2],a_i3[3]);
			myshowfile(a_i,a_i2,name);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"mycpto")){
		sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
		mycpto(name,a_i2);
	}

	else if(!strcmp(i,"mycp")){
		char name2[5];
		sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
		sprintf(name2,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
		mycp(name2,name);
		system("rm loperz");
	}

	else if(!strcmp(i,"mymv")){
		sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
		mymv(a_i,name);
	}

	else if(!strcmp(i,"mymkfs")){
		printf("myfs가 이미 존재합니다!\n");
		return;
	}

	else if(!strcmp(i,"myrm")){
		myrm_func(a_i);
	}else{

		printf("다시 입력해 주세요\n");
		return;

	}

}


int i_bit_check(){			// 아이노드 비트열 확인
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


int d_bit_check(){		//데이터 블록 비트열 확인

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

void i_bit_insert(){		//아이노트 블록 비트열 확인
	int inode;
	unsigned mask = 1;
	mask <<= 31;

	inode = i_bit_check();

	mask >>= (inode%32);
	mfs->super[inode/32].imask |= mask;

}

void d_bit_insert(){		//data블록 사용 여부 비트열 확인
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

void i_bit_del(int id){		//아이노트 블록 비트열 삭제
	unsigned mask = 1;
	mask <<= 31;

	mask >>= (id%32);
	mfs->super[id/32].imask ^= mask;

	free(mfs->inode[id]);
	mfs->inode[id] = NULL;

}

void d_bit_del(int id){		//data블록 비트열 삭제
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

	free(mfs->block[dd]);
	mfs->block[dd] = NULL;

}

void d_bit_del_func(int dd){  // data블록 비트열 삭제 (myrm 시 사용)
	unsigned mask = 1;
	mask <<= 31;

	if(dd%64 <= 31){
		mask >>= (dd%32);
		mfs->super[dd/64].dmask1 ^= mask;
	}else{
		mask >>= (dd%32);
		mfs->super[dd/64].dmask2 ^= mask;
	}

	free(mfs->block[dd]);
	mfs->block[dd] = NULL;

}

void cal_date(int inode){		//시간, 날짜 계산
	struct tm *t;
	time_t timer; // 시간 측정

	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기

	sprintf(mfs->inode[inode]->file_date,"%04d/%02d/%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);

}

int cal_size(char a_i[20]){		//사이즈 계산
	FILE *fp;

	fp = fopen(a_i,"rt");

	fseek(fp,0,SEEK_END);


	return ftell(fp);

}

void set_sidbit(char a_i[20],int inode){		//single indirect block 설정
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

int *get_sidbit(int inode){		//single indirect block안에 있는 datablock얻기 
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

int *get_sidbit_ADD(int inode){		//linked list로 데이터블록 연결할때 single indirect block안에 있는 datablock 얻기
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

void make_sid(int inode){		//single indirect block 만들기
	int data;

	data = d_bit_check();

	mfs->block[data] = malloc(sizeof(struct d_block));

	mfs->inode[inode]->sidb = mfs->block[data];
	mfs->inode[inode]->sin = data;

	for(int i=0;i<128;i++)
		mfs->inode[inode]->sidb->data[i] = '\0'; 

	d_bit_insert();

}

void set_didbit(int inode){		//double indirect block 설정
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

int *get_didbit(int inode){		//double indirect block이 가리키는 single indirect block 얻기
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

void set_dsidbit(int inode){	//double indirect block이 가리키는 single indirect block이 가리키는 datablock 설정
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

int *get_dsidbit(int inode){	//double indirect block이 가리키는 single indirect block이 가리키는 datablock얻기
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
				c = mfs->block[data]->da/**************************************************************************
 * 팀명 : Eㅣ 없이 맑은 상호 조	 							              *
 * 팀원 : 박상호, 박채영, 배서현									      *
 * 프로젝트 기간 : 11/3 ~ 12/11										      *
 * 프로젝트 기여도 -> 박상호 : 33.34%  배서현 : 33.33%  박채영 33.33%     *
 * 프로젝트 깃허브 사이트 : https://github.com/sangho98/UNIX-my-own-shell *
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct d_block{
	int num; // 데이터블록 번호
	unsigned char data[128];
}dblock;

struct inode{			//inode구조체
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

struct s_block{				//슈퍼블록
	unsigned imask, dmask1, dmask2;
};

struct my_file_system{			//MY 파일 시스템
	unsigned b_block : 16;
	struct s_block super[16];
	struct inode *inode[512];
	struct d_block *block[1024];
};

struct dtree{ // 디렉터리 및 파일 트리 구조 (이진 트리로 구현)
	int inode;
	char name[5];
	struct inode *p;
	struct dtree *left;
	struct dtree *right;
};

struct btree{ // myrm, mycat 등의 명령어 실행 시 다이렉트, 싱글 인다이렉트 블록, 더블 인다이렉트 블록이 가리키는 데이터 블럭들 링크드 리스트로 연결
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
void d_bit_del_func(int);
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
void myls_func(char [],char []);
void swap(char **,int);
void swap2(char **,int [],int);
void myrmdir(char []);
void myrmdir_func(char []);
void mytree(struct dtree*,int);
void mytree_func(struct dtree*,int);
void myshowinode(char []);
void myshowblock(char []);
void mystate();
void mycpfrom(char[],char[]);
void mytouch(char []);
void mytouch_func(char []);
void mycat(char []);
void mycpto(char [],char []);
void mycp(char [],char []);
void mymv(char [],char []);
int mymv_func(char []);
void myshowfile(char [],char [],char []);
void myrm(char []);
void myrm_func(char []);
void myrm_func2(char [],int);

struct my_file_system *mfs;

struct dtree *D=NULL; // 루트디렉터리 부터 이진 트리
struct dtree *cur = NULL; // 현재 작업 디렉터리 가리키는 이진 트리
struct dtree *back = NULL; // 백업 트리
struct dtree *back2 = NULL; // 백업 트리
struct dtree *back3 = NULL; // 백업 트리
struct dtree *back4 = NULL; // 백업 트리

char path[100]= {'\0'}; // mypwd 실행 및 현재 작업 디렉터리 표시
char backup[100]= {'\0'}; // 백업 경로
char backup2[100]= {'\0'}; // 백업 경로
char backup3[100]= {'\0'}; // 백업 경로
char backup4[100]= {'\0'}; // 백업 경로

int main() {


	mfs = malloc(sizeof(struct my_file_system)); // my_file_system  메모리 동적 할당

	char make[10];
	FILE *ms;

	ms = fopen("myfs","rb");

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
			mfs->inode[0]->sin = -1;
			mfs->inode[0]->dou = -1;

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

		// 명령어 실행 구간

		char i[30], a_i[20],a_i2[20],a_i3[20];

		prompt(i,a_i,a_i2,a_i3);

	}


}

void prompt(char i[30], char a_i[20],char a_i2[20],char a_i3[20]){		// 명령어 입력 받는 프롬프트(쉘)
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

void command(char i[30], char a_i[30],char a_i2[20],char a_i3[20]){		//명령어 실행 함수
	char name[5];

	if(!strcmp(i,"byebye")){
		exit(1);
	}

	else if(!strcmp(i,"mymkdir")){
		if(a_i[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
			mymkdir(name);
		}else{
			printf("폴더명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"mycpfrom")){
		if(a_i2[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
			mycpfrom(a_i,name);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"mycd")){
		if(a_i[0] != '\0'){
			mycd_func(a_i);
		}else{
			mycd(NULL);
		}

	}

	else if(!strcmp(i,"myls")){
		myls_func(a_i,a_i2);
	}

	else if(!strcmp(i,"mytree")){
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

	else if(!strcmp(i,"mypwd")){
		printf("%s\n",path);
	}

	else if(!strcmp(i,"myrmdir")){
		myrmdir_func(a_i);
	}

	else if(!strcmp(i,"myshowinode")){
		myshowinode(a_i);
	}

	else if(!strcmp(i,"myshowblock")){
		myshowblock(a_i);
	}

	else if(!strcmp(i,"mystate")){
		mystate();
	}

	else if(!strcmp(i,"mycat")){
		sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
		mycat(name);
	}

	else if( !(i[0] == 'm' && i[1] == 'y') ){
		sprintf(i,"%s %s %s %s",i,a_i,a_i2,a_i3);
		system(i);
	}

	else if(!strcmp(i,"mytouch")){
		if(a_i[0] != '\0'){
			mytouch_func(a_i);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"myshowfile")){
		if(a_i3[0] != '\0'){
			sprintf(name,"%c%c%c%c",a_i3[0],a_i3[1],a_i3[2],a_i3[3]);
			myshowfile(a_i,a_i2,name);
		}else{
			printf("파일명을 입력 해주세요.\n");
		}
	}

	else if(!strcmp(i,"mycpto")){
		sprintf(name,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
		mycpto(name,a_i2);
	}

	else if(!strcmp(i,"mycp")){
		char name2[5];
		sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
		sprintf(name2,"%c%c%c%c",a_i[0],a_i[1],a_i[2],a_i[3]);
		mycp(name2,name);
		system("rm loperz");
	}

	else if(!strcmp(i,"mymv")){
		sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);
		mymv(a_i,name);
	}

	else if(!strcmp(i,"mymkfs")){
		printf("myfs가 이미 존재합니다!\n");
		return;
	}

	else if(!strcmp(i,"myrm")){
		myrm_func(a_i);
	}else{

		printf("다시 입력해 주세요\n");
		return;

	}

}


int i_bit_check(){			// 아이노드 비트열 확인
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


int d_bit_check(){		//데이터 블록 비트열 확인

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

void i_bit_insert(){		//아이노트 블록 비트열 확인
	int inode;
	unsigned mask = 1;
	mask <<= 31;

	inode = i_bit_check();

	mask >>= (inode%32);
	mfs->super[inode/32].imask |= mask;

}

void d_bit_insert(){		//data블록 사용 여부 비트열 확인
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

void i_bit_del(int id){		//아이노트 블록 비트열 삭제
	unsigned mask = 1;
	mask <<= 31;

	mask >>= (id%32);
	mfs->super[id/32].imask ^= mask;

	free(mfs->inode[id]);
	mfs->inode[id] = NULL;

}

void d_bit_del(int id){		//data블록 비트열 삭제
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

	free(mfs->block[dd]);
	mfs->block[dd] = NULL;

}

void d_bit_del_func(int dd){  // data블록 비트열 삭제 (myrm 시 사용)
	unsigned mask = 1;
	mask <<= 31;

	if(dd%64 <= 31){
		mask >>= (dd%32);
		mfs->super[dd/64].dmask1 ^= mask;
	}else{
		mask >>= (dd%32);
		mfs->super[dd/64].dmask2 ^= mask;
	}

	free(mfs->block[dd]);
	mfs->block[dd] = NULL;

}

void cal_date(int inode){		//시간, 날짜 계산
	struct tm *t;
	time_t timer; // 시간 측정

	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기

	sprintf(mfs->inode[inode]->file_date,"%04d/%02d/%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);

}

int cal_size(char a_i[20]){		//사이즈 계산
	FILE *fp;

	fp = fopen(a_i,"rt");

	fseek(fp,0,SEEK_END);


	return ftell(fp);

}

void set_sidbit(char a_i[20],int inode){		//single indirect block 설정
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

int *get_sidbit(int inode){		//single indirect block안에 있는 datablock얻기 
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

int *get_sidbit_ADD(int inode){		//linked list로 데이터블록 연결할때 single indirect block안에 있는 datablock 얻기
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

void make_sid(int inode){		//single indirect block 만들기
	int data;

	data = d_bit_check();

	mfs->block[data] = malloc(sizeof(struct d_block));

	mfs->inode[inode]->sidb = mfs->block[data];
	mfs->inode[inode]->sin = data;

	for(int i=0;i<128;i++)
		mfs->inode[inode]->sidb->data[i] = '\0'; 

	d_bit_insert();

}

void set_didbit(int inode){		//double indirect block 설정
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

int *get_didbit(int inode){		//double indirect block이 가리키는 single indirect block 얻기
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

void set_dsidbit(int inode){	//double indirect block이 가리키는 single indirect block이 가리키는 datablock 설정
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

int *get_dsidbit(int inode){	//double indirect block이 가리키는 single indirect block이 가리키는 datablock얻기
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

void make_did(int inode){		//double indirect block 만들기
	int data;

	data = d_bit_check();

	mfs->block[data] = malloc(sizeof(struct d_block));

	mfs->inode[inode]->didb = mfs->block[data];
	mfs->inode[inode]->dou = data;

	for(int i=0;i<128;i++)
		mfs->inode[inode]->didb->data[i] = '\0';

	d_bit_insert();
}

void in_data(char a_i[20],int inode){		//direct block에 삽입
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

void sin_data(char a_i[30],int inode,int fpos){		//single indirect block이 가리키는 direct block에 삽입
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

void dsin_data(char a_i[30],int inode,int fpos){		//double indirect block이 가리키는 single indirect block이 가리키는 direct block에 삽입
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
void in_dir(char name[5],int inode){		//디렉터리안에 아이노드번호, 폴더이름, 파일이름 넣기
	char tmp[128],tmp2[128];

	sprintf(tmp,"%-7s",cur->p->db->data);

	sprintf(cur->p->db->data,"%-3d%-4s",inode,name);
	strcat(tmp,cur->p->db->data);

	sprintf(cur->p->db->data,"%-7s",tmp);

}

// ok
void mymkdir(char name[5]){			//디렉터리를 생성하는 명령어 mymkdir
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

void myls(char a_i2[30]){		//파일 목록을 출력하는 명령어 myls
	int s=0,k=0,inode;
	int width=0;
	char *temp[1024];
	int itemp[1024];
	char ttemp[1024];
	int *stemp[1024];
	char *ctemp[1024];
	char tmp[5],tmp1[4],type[2];
	struct dtree *p = cur->left;


	if(!strcmp(a_i2,"-i")){
		struct dtree *ioi = cur->left;
		while(ioi != NULL)
		{
			temp[width]=(char *)calloc(strlen(ioi->name)+1, sizeof(char));
			strcpy(temp[width], ioi->name);
			itemp[width]=ioi->inode;
			ioi=ioi->right;
			width++;
		}
		swap2(temp, itemp, width-1);
		printf(".\n..\n");
		for(int i=0; i<width; i++)
			printf("%d %s\n", itemp[i], temp[i]);

		return;

	}

	if(!strcmp(a_i2,"-l")){
		return;

	}

	if( (!strcmp(a_i2,"-li")) || (!strcmp(a_i2,"-il")) ){

		return;
	}


	struct dtree *ioi = cur->left;


	while(ioi != NULL){
		temp[width]=(char *)calloc(strlen(ioi->name)+1, sizeof(char));
		strcpy(temp[width], ioi->name);
		width++;
		ioi = ioi->right;
	}
	swap(temp, width-1);

	printf(".\n..\n");
	for(int i=0; i<width; i++)
		printf("%s\n", temp[i]);

	return;
}

void myls_func(char a_i[20],char a_i2[20]){   //절대경로와 상대경로를 통해 myls함수 접근 
	int s;
	char *line;
	char tmp;

	line = strtok(a_i,"/");

	strcpy(backup2,path);

	back2 = cur;

	if(a_i[0] == '/'){
		mycd(a_i);
	}

	if(!strcmp(a_i,"-i")){
		myls(a_i);
		return;
	}else if(!strcmp(a_i,"-l")){
		myls(a_i);
		return;
	}else if(!strcmp(a_i,"-l")){
		myls(a_i);
		return;
	}

	while(line != NULL){

		s =	mycd(line);

		if(s == 1){
			cur = back2;
			return;
		}

		line = strtok(NULL,"/");
	}

	myls(a_i2);

	cur = back2;

	strcpy(path,backup2);

	return;

}
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
void swap(char *temp[], int  width) // myls 사전식배열
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



int mycd(char *a_i){		// 작업 디렉터리를 이동하는 명령어 mycd
	int s=0,k,ck=999,n=0;
	char tmp[5],tmp2[40];
	struct dtree *p = cur->left;
	struct dtree *test = cur->left;

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
			if(test == NULL){
				return 4;
			}

			if(!strcmp(test->name,a_i)){

				if(test->p->file_type == 0){
					printf("잘못된 경로 입니다.\n");
					strcpy(path,backup2);
					return 1;
				}

				cur = test;

				sprintf(tmp2,"%s",path);

				if(path[0] == '/' && path[1] == '\0'){
					sprintf(path,"/%s",a_i);
				}else{
					sprintf(path,"%s/%s",tmp2,a_i);
				}
				break;
			}

			if(test->right == NULL){
				printf("잘못된 경로 입니다.\n");
				break;
			}
			test = test->right;
		}

	}



}

int mycd_func(char a_i[30]){		//절대경로와 상대경로를 통해 mycd함수 접근
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

		s =	mycd(line);

		if(s == 1){
			cur = back2;
			return 1;
		}

		line = strtok(NULL,"/");
	}

}


void myrmdir(char a_i[30]){			//디렉터리를 삭제하는 명령어 myrmdir
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
			t = NULL; 
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
void myrmdir_func(char a_i[30]){		//절대경로와 상대경로를 통해서 myrmdir 함수 접근
	char *line,*line2;
	char tmp[30],tmp2[100][5];
	int s=0,i=0,a=0;

	strcpy(tmp,a_i);
	line = strtok(a_i,"/");


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

void mytree(struct dtree *tr,int d){		//지정된 디렉터리부터 디렉터리 구조  출력. 지정안되면 현재 디렉터리부터 구조 출력 mytree

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

void mytree_func(struct dtree *tr,int d){		//절대경로와 상대경로를 통해서 mytree함수 접근


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

void myshowinode(char a_i[30]){			//지정된 inode에 들어있는 애용 보기 myshowinode
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

void myshowblock(char a_i[30]){			//지정된 데이터 블록에 들어있는 내용 보기	myshowblock
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

void mystate(){			//현재 파일시스템의 상태 출력 	mystate
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

void mycpfrom(char a_i[20],char name[5]){		//일반 파일을 MY 파일시스템의 파일로 복사  	mycpfrom

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

void mytouch(char name[5]){			//파일 수정 시간을 변경하는 명령어 명시된 파일이 현재 디렉터리에 없을 경우하는 크기 0바이트mytouch
	int s=0,k,ck=-1,n=0,ckin= -1;
	int inode,data;
	char tmp[5],tmp1[4];
	struct dtree *p = cur->left;


	while(p != NULL){

		if(!strcmp(p->name,name)){

			ckin = p->inode;
			break;
		}


		p = p->right;
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

void mytouch_func(char a_i[20]){		//절대경로와 상대경로를 통해서 mytouch함수 접근
	char *line,*line2;
	char tmp[30],tmp2[100][5];
	int s=0,i=0,a=0;

	strcpy(tmp,a_i);
	line = strtok(a_i,"/");


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
			mytouch(tmp2[i]);
			cur = back2;
			strcpy(path,backup2);
			return;
		}

		mycd(tmp2[i]);

		i++;

	}

}

void mycat(char a_i[30]){		//파일 연결 몇 출력 명령어 		mycat		
	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];


	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}


	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		for(int i=0;i<mfs->inode[id]->file_size;i++)
			printf("%c",mfs->inode[id]->db->data[i]);

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);



		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<mfs->inode[id]->numS+1;i++)
			temp[i] = num[i-1];



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

void mycpto(char a_i[30],char a_i2[30]){   		//MY 파일시스템에 있는 파일을 일반 파일로 복사 mycpto 
	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	FILE *fp;

	fp = fopen(a_i2,"wb");

	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}


	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		for(int i=0;i<mfs->inode[id]->file_size;i++)
			printf("%c",mfs->inode[id]->db->data[i]);

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);


		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<mfs->inode[id]->numS+1;i++)
			temp[i] = num[i-1];


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

	int tmmp,v=0,check;
	char data[128];

	while(print != NULL){

		fwrite(&print->p->data,sizeof(print->p->data),1,fp);

		print = print->next;
	}

	fclose(fp);

	num = NULL;
	num2 = NULL;
}

void mymv(char a_i[20], char a_i2[20]){			//파일을 이동하거나 이름을 변경하는 	mymv
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


	if(pp == 999){

		printf("파일명이 정확하지 않습니다.\n");
		return;

	}else{

		ppp = mymv_func(a_i2);

		if(ppp == 999){
			//존재 x 파일명 바꾸기
			struct dtree *pppp = cur->left;
			struct dtree *pd = cur->left;
			char front[128],fback[128];
			char name[5];
			int i=0,o=0;

			sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);

			name[5] = '\0';

			while(pppp != NULL){
				pppp = pppp->right;
				o++;
			}

			pppp = cur->left;

			while( pd != NULL){

				if(!strcmp(pd->name,a_i)){

					strcpy(pd->name,name);

					for(int j=0;j<(i+2)*7+3;j++){
						front[j] = cur->p->db->data[j];
					}

					front[((i+2)*7)+3] = '\0';

					int j = (i+3)*7;
					int k = 0;

					for(int a=j;a<(o+2)*7;a++){
						fback[a-j] = cur->p->db->data[a];
					}

					fback[(o+2) * 7] = '\0';

					// front, fback 이용해서 아이노드 번호 이름 삭제

					sprintf(cur->p->db->data,"%s%-4s%s",front,name,fback);

					break;
				}

				pd = pd->right;

				i++;
			}



		}else{
			struct dtree *pd = cur->left;
			struct dtree *pdd = cur->left;
			struct dtree *pppp = cur->left;
			struct dtree *temmp= cur->left;
			struct dtree *usep = malloc(sizeof(struct dtree));;
			struct dtree *use = malloc(sizeof(struct dtree));;
			int i = 0,o=0,ii=0;
			char front[128],fback[128];
			char name[5];
			int inode;
			// 존재 o 파일 옮기기
			// 파일이면 리턴, 폴더면 폴더 들어감
			// 폴더 들어가고 아이노드 번호 쓰고 현재 폴더에 아이노드 번호 및 이름 삭제
			// 이제 파일 옮겨야 한다 -> cd로 들어간 후 맨마지막에 추가 
			// id = 아이노드 번호 tmp6 = 아이노드 이름
			// 연결 끊어준 후 폴더에 들어가서 연결 -> bback;
			while(pppp != NULL){
				pppp = pppp->right;
				o++;
			}
			pppp = cur->left;

			while(1){

				if(!strcmp(pdd->name,a_i)){

					// 앞 뒤 가져오기
					for(int j=0;j<(i+2)*7;j++){
						front[j] = cur->p->db->data[j];
					}
					front[((i+2)*7)] = '\0';

					int j = (i+3)*7;
					int k = 0;

					for(int a=j;a<(o+2)*7;a++){
						fback[a-j] = cur->p->db->data[a];
					}

					// front, fback 이용해서 아이노드 번호 이름 삭제

					sprintf(cur->p->db->data,"%s%s",front,fback);

					// 여기서 부터 끊어주자 먼저 바로 전 노드 검색


					for(int j=0;j<i-1;j++){
						temmp = temmp->right;
					}

					inode = pdd->inode;
					strcpy(name,pdd->name);


					// 트리구조 끊어준다.

					usep = pdd;
					ii = i;
					break;

				}
				pdd = pdd->right;
				i++;

			}

			while(1){

				if(!strcmp(pd->name,a_i2)){
					struct dtree *temp = pd->left;

					if(pd->p->file_type == 0){
						printf("파일 입니다.\n");
						return;
					}
					while(1){

						if(ii==0){
							if(o==1){
								cur->left = NULL;
								break;
							}
							cur->left = cur->left->right;
							break;
						}else{
							printf("inode = %d,name = %s\n",temmp->inode,temmp->name);
							temmp->right = temmp->right->right;
							break;
						}
					}

					if(temp == NULL){

						usep->right = NULL;
						usep->left = NULL;

						pd->left = usep;

					}else{
						while(temp->right != NULL)
							temp = temp->right;

						usep->right = NULL;
						usep->left= NULL;

						temp->right = usep;

					}

					// 데이터에 아이노드 이름 추가
					char all[128];

					strcpy(all,pd->p->db->data);
					sprintf(pd->p->db->data,"%s%-3d%-4s",all,inode,name);
					return;
				}
				pd = pd->right;

			}

		}

	}


}

int mymv_func(char a_i[20]){		//절대경로와 상대경로를 통해서 mymv함수로 접근
	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4],tmp6[20];
	int pp=0,ppp=0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i)){
			return 1;

			break;
		}

		pd = pd->right;
	}

	return 999;
}

void mycp(char a_i[20],char name[5]){		//MY 파일시스템에 있는 파일을 복사 mycp

	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	mycpto(a_i,"loperz");

	//파일 저장 완료
	// 다시 mycpfrom 하기

	mycpfrom("loperz",name);

	num = NULL;
	num2 = NULL;
}

void myshowfile(char a_i[20],char a_i2[20],char a_i3[20]){		//파일을 지정된 부분 출력 myshowfile
	int num11,num22;
	int loops,loopf,start,rem,remm;
	int remain = 0,remains = 0;

	num11 = atoi(a_i);
	num22 = atoi(a_i2);

	rem = num11 % 128;

	if(rem == 0){
		remm = num11 - (( (num11 / 128) - 1) * 128);
	}

	loops = num11 / 128;

	remains = num11 % 128;

	loopf = num22 / 128;

	remain = num22 % 128;

	start = num11 / 128;

	if(remain != 0)
		loopf += 1;

	if(remains == 0){
		start -= 1;
		loops -= 1;
	}

	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i3)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}

	if(mfs->inode[id]->sin == -1){

		int nn=loops;

		if(loops == 0 && loopf == 1){
			int abcd;
			int dcba;
			dcba = num11 % 129;
			abcd = num22 % 129;
			for(int i=dcba-1;i<abcd;i++)
				printf("%c",mfs->inode[id]->db->data[i]);
		}

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);

		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<103;i++)
			temp[i] = num[i-1];

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
		int nn=loops;

		for(int i=0;i<loops;i++)
			print = print->next;

		if(loops == 0 && loopf == 1){
			int abcd;
			abcd = num22 % 129;
			for(int i=0;i<abcd;i++)
				printf("%c",print->p->data[i]);
			return;
		}

		while(1){

			if(loopf == nn){
				for(int i=0;i<remain;i++){
					printf("%c",print->p->data[i]);
				}
				break;
			}
			if(loops == nn){

				if( rem == 0 ){
					printf("%c",print->p->data[remm]);
				}

				for(int i=remains-1;i<128;i++)
					printf("%c",print->p->data[i]);
			}

			if(loopf != nn && loops != nn){

				for(int i=0;i<128;i++)
					printf("%c",print->p->data[i]);

			}

			print = print->next;
			nn++;
		}

		return;
	}

	int loopQ,loopP;

	loopQ = mfs->inode[id]->num;
	loopP = mfs->inode[id]->numP;

	num = malloc(102 * sizeof(int));

	num = get_sidbit_ADD(id);

	num2 = malloc( ( ((loopQ - 1) * 102) + loopP +1 ) * sizeof(int));

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
	int nn=loops;

	for(int i=0;i<loops;i++)
		print = print->next;

	if(loops == 0 && loopf == 1){
		int abcd;
		abcd = num22 % 129;
		for(int i=0;i<abcd;i++)
			printf("%c",print->p->data[i]);
		return;
	}

	while(1){

		if(loopf == nn){
			for(int i=0;i<remain;i++){
				printf("%c",print->p->data[i]);
			}
			break;
		}
		if(loops == nn){

			if( rem == 0 ){
				printf("%c",print->p->data[remm]);
			}

			for(int i=remains-1;i<128;i++)
				printf("%c",print->p->data[i]);
		}

		if(loopf != nn && loops != nn){

			for(int i=0;i<128;i++)
				printf("%c",print->p->data[i]);

		}

		print = print->next;
		nn++;
	}

	num = NULL;
	num2 = NULL;
}

void myrm(char name[5]){		//파일을 삭제하는 명령 		myrm
	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];


	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,name)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}

	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		int dd;
		dd = mfs->inode[id]->dir;

		free(mfs->block[dd]);

		mfs->block[dd] = NULL;

		d_bit_del_func(dd);

		myrm_func2(name,id);

		struct dtree *pppp = cur->left;
		struct dtree *ppd = cur->left;
		char front[128], fback[128];
		int u=0,o=0;

		while(pppp != NULL){
			pppp = pppp->right;
			o++;
		}

		pppp = cur->left;

		while( ppd != NULL ){

			if(!strcmp(ppd->name,name)){

				for(int j=0;j<(u+2)*7;j++){
					front[j] = cur->p->db->data[j];
				}

				front[((u+2)*7)] = '\0';

				int j = (u+3)*7;
				int k = 0;

				for(int a=j;a<(o+2)*7;a++){
					fback[a-j] = cur->p->db->data[a];
				}

				fback[(o+2) * 7] = '\0';

				// front, fback 이용해서 아이노드 번호 이름 삭제

				sprintf(cur->p->db->data,"%s%s",front,fback);

				break;
			}

			ppd = ppd->right;

			u++;
		}


		i_bit_del(id);

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


		struct btree *head = NULL;
		struct btree *tail = NULL;

		b = 0;

		while(temp[b] != 0){

			free(mfs->block[temp[b]]);
			mfs->block[temp[b]] = NULL;

			d_bit_del_func(temp[b]);

			b++;
		}

		myrm_func2(name,id);

		struct dtree *pppp = cur->left;
		struct dtree *ppd = cur->left;
		char front[128], fback[128];
		int u=0,o=0;

		while(pppp != NULL){
			pppp = pppp->right;
			o++;
		}

		pppp = cur->left;

		while( ppd != NULL ){

			if(!strcmp(ppd->name,name)){

				for(int j=0;j<(u+2)*7;j++){
					front[j] = cur->p->db->data[j];
				}

				front[((u+2)*7)] = '\0';

				int j = (u+3)*7;
				int k = 0;

				for(int a=j;a<(o+2)*7;a++){
					fback[a-j] = cur->p->db->data[a];
				}

				fback[(o+2) * 7] = '\0';

				// front, fback 이용해서 아이노드 번호 이름 삭제

				sprintf(cur->p->db->data,"%s%s",front,fback);

				break;
			}

			ppd = ppd->right;

			u++;
		}


		i_bit_del(id);

		return;
	}

	int loopQ,loopP;

	loopQ = mfs->inode[id]->num;
	loopP = mfs->inode[id]->numP;

	num = malloc(102 * sizeof(int));

	num = get_sidbit_ADD(id);

	num2 = malloc( ( ((loopQ - 1) * 102) + loopP +1 ) * sizeof(int));

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

	temp[b+104] = '\0';


	struct btree *head = NULL;
	struct btree *tail = NULL;

	b = 0;


	while(temp[b] != 0){

		free(mfs->block[temp[b]]);
		mfs->block[temp[b]] = NULL;

		d_bit_del_func(temp[b]);

		b++;
	}

	myrm_func2(name,id);

	struct dtree *pppp = cur->left;
	struct dtree *ppd = cur->left;
	char front[128], fback[128];
	int u=0,o=0;

	while(pppp != NULL){
		pppp = pppp->right;
		o++;
	}

	pppp = cur->left;

	while( ppd != NULL ){

		if(!strcmp(ppd->name,name)){

			for(int j=0;j<(u+2)*7;j++){
				front[j] = cur->p->db->data[j];
			}

			front[((u+2)*7)] = '\0';

			int j = (u+3)*7;
			int k = 0;

			for(int a=j;a<(o+2)*7;a++){
				fback[a-j] = cur->p->db->data[a];
			}

			fback[(o+2) * 7] = '\0';

			// front, fback 이용해서 아이노드 번호 이름 삭제

			sprintf(cur->p->db->data,"%s%s",front,fback);

			break;
		}

		ppd = ppd->right;

		u++;
	}


	i_bit_del(id);

	num = NULL;
	num2 = NULL;

}

void myrm_func(char a_i[30]){      // 절대경로와 상대경로를 통해서 이 함수로 접근
	char *line,*line2;
	char tmp[30],tmp2[100][5];
	int s=0,i=0,a=0;

	strcpy(tmp,a_i);
	line = strtok(a_i,"/");


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
			myrm(tmp2[i]);
			cur = back2;
			strcpy(path,backup2);
			return;
		}

		mycd(tmp2[i]);

		i++;

	}


}

void myrm_func2(char a_i[20],int id){		// tree구조 끊어주기

	int s=0,k,ck=999,n=0,l,m=0,dd;
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
				if(mfs->inode[id]->file_type == 1){
					printf("파일이 아닙니다.\n");
					return;
				}
				cur->left = NULL;
				return;
			}
			t = cur->left;

			if(mfs->inode[id]->file_type == 1){
				printf("파일이 아닙니다.\n");
				return;
			}

			cur->left = cur->left->right;
			t->right = NULL;
			free(t);
			t = NULL; // 추가
			return;
		}

		for(int i=0;i<ck-3;i++){
			p = p->right;
		}

		if(mfs->inode[id]->file_type == 1){
			printf("파일이 아닙니다.\n");
			return;
		}
		back = p->right;
		p->right = p->right->right;
		back->right=NULL;
		free(back);

	}

}
ta[k++];
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

void make_did(int inode){		//double indirect block 만들기
	int data;

	data = d_bit_check();

	mfs->block[data] = malloc(sizeof(struct d_block));

	mfs->inode[inode]->didb = mfs->block[data];
	mfs->inode[inode]->dou = data;

	for(int i=0;i<128;i++)
		mfs->inode[inode]->didb->data[i] = '\0';

	d_bit_insert();
}

void in_data(char a_i[20],int inode){		//direct block에 삽입
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

void sin_data(char a_i[30],int inode,int fpos){		//single indirect block이 가리키는 direct block에 삽입
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

void dsin_data(char a_i[30],int inode,int fpos){		//double indirect block이 가리키는 single indirect block이 가리키는 direct block에 삽입
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
void in_dir(char name[5],int inode){		//디렉터리안에 아이노드번호, 폴더이름, 파일이름 넣기
	char tmp[128],tmp2[128];

	sprintf(tmp,"%-7s",cur->p->db->data);

	sprintf(cur->p->db->data,"%-3d%-4s",inode,name);
	strcat(tmp,cur->p->db->data);

	sprintf(cur->p->db->data,"%-7s",tmp);

}

// ok
void mymkdir(char name[5]){			//디렉터리를 생성하는 명령어 mymkdir
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

void myls(char a_i2[30]){		//파일 목록을 출력하는 명령어 myls
	int s=0,k=0,inode;
	int width=0;
	char *temp[1024];
	int itemp[1024];
	char ttemp[1024];
	int *stemp[1024];
	char *ctemp[1024];
	char tmp[5],tmp1[4],type[2];
	struct dtree *p = cur->left;


	if(!strcmp(a_i2,"-i")){
		struct dtree *ioi = cur->left;
		while(ioi != NULL)
		{
			temp[width]=(char *)calloc(strlen(ioi->name)+1, sizeof(char));
			strcpy(temp[width], ioi->name);
			itemp[width]=ioi->inode;
			ioi=ioi->right;
			width++;
		}
		swap2(temp, itemp, width-1);
		printf(".\n..\n");
		for(int i=0; i<width; i++)
			printf("%d %s\n", itemp[i], temp[i]);

		return;

	}

	if(!strcmp(a_i2,"-l")){
		return;

	}

	if( (!strcmp(a_i2,"-li")) || (!strcmp(a_i2,"-il")) ){

		return;
	}


	struct dtree *ioi = cur->left;


	while(ioi != NULL){
		temp[width]=(char *)calloc(strlen(ioi->name)+1, sizeof(char));
		strcpy(temp[width], ioi->name);
		width++;
		ioi = ioi->right;
	}
	swap(temp, width-1);

	printf(".\n..\n");
	for(int i=0; i<width; i++)
		printf("%s\n", temp[i]);

	return;
}

void myls_func(char a_i[20],char a_i2[20]){   //절대경로와 상대경로를 통해 myls함수 접근 
	int s;
	char *line;
	char tmp;

	line = strtok(a_i,"/");

	strcpy(backup2,path);

	back2 = cur;

	if(a_i[0] == '/'){
		mycd(a_i);
	}

	if(!strcmp(a_i,"-i")){
		myls(a_i);
		return;
	}else if(!strcmp(a_i,"-l")){
		myls(a_i);
		return;
	}else if(!strcmp(a_i,"-l")){
		myls(a_i);
		return;
	}

	while(line != NULL){

		s =	mycd(line);

		if(s == 1){
			cur = back2;
			return;
		}

		line = strtok(NULL,"/");
	}

	myls(a_i2);

	cur = back2;

	strcpy(path,backup2);

	return;

}
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
void swap(char *temp[], int  width) // myls 사전식배열
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



int mycd(char *a_i){		// 작업 디렉터리를 이동하는 명령어 mycd
	int s=0,k,ck=999,n=0;
	char tmp[5],tmp2[40];
	struct dtree *p = cur->left;
	struct dtree *test = cur->left;

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
			if(test == NULL){
				return 4;
			}

			if(!strcmp(test->name,a_i)){

				if(test->p->file_type == 0){
					printf("잘못된 경로 입니다.\n");
					strcpy(path,backup2);
					return 1;
				}

				cur = test;

				sprintf(tmp2,"%s",path);

				if(path[0] == '/' && path[1] == '\0'){
					sprintf(path,"/%s",a_i);
				}else{
					sprintf(path,"%s/%s",tmp2,a_i);
				}
				break;
			}

			if(test->right == NULL){
				printf("잘못된 경로 입니다.\n");
				break;
			}
			test = test->right;
		}

	}



}

int mycd_func(char a_i[30]){		//절대경로와 상대경로를 통해 mycd함수 접근
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

		s =	mycd(line);

		if(s == 1){
			cur = back2;
			return 1;
		}

		line = strtok(NULL,"/");
	}

}


void myrmdir(char a_i[30]){			//디렉터리를 삭제하는 명령어 myrmdir
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
			t = NULL; 
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
void myrmdir_func(char a_i[30]){		//절대경로와 상대경로를 통해서 myrmdir 함수 접근
	char *line,*line2;
	char tmp[30],tmp2[100][5];
	int s=0,i=0,a=0;

	strcpy(tmp,a_i);
	line = strtok(a_i,"/");


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

void mytree(struct dtree *tr,int d){		//지정된 디렉터리부터 디렉터리 구조  출력. 지정안되면 현재 디렉터리부터 구조 출력 mytree

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

void mytree_func(struct dtree *tr,int d){		//절대경로와 상대경로를 통해서 mytree함수 접근


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

void myshowinode(char a_i[30]){			//지정된 inode에 들어있는 애용 보기 myshowinode
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

void myshowblock(char a_i[30]){			//지정된 데이터 블록에 들어있는 내용 보기	myshowblock
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

void mystate(){			//현재 파일시스템의 상태 출력 	mystate
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

void mycpfrom(char a_i[20],char name[5]){		//일반 파일을 MY 파일시스템의 파일로 복사  	mycpfrom

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

void mytouch(char name[5]){			//파일 수정 시간을 변경하는 명령어 명시된 파일이 현재 디렉터리에 없을 경우하는 크기 0바이트mytouch
	int s=0,k,ck=-1,n=0,ckin= -1;
	int inode,data;
	char tmp[5],tmp1[4];
	struct dtree *p = cur->left;


	while(p != NULL){

		if(!strcmp(p->name,name)){

			ckin = p->inode;
			break;
		}


		p = p->right;
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

void mytouch_func(char a_i[20]){		//절대경로와 상대경로를 통해서 mytouch함수 접근
	char *line,*line2;
	char tmp[30],tmp2[100][5];
	int s=0,i=0,a=0;

	strcpy(tmp,a_i);
	line = strtok(a_i,"/");


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
			mytouch(tmp2[i]);
			cur = back2;
			strcpy(path,backup2);
			return;
		}

		mycd(tmp2[i]);

		i++;

	}

}

void mycat(char a_i[30]){		//파일 연결 몇 출력 명령어 		mycat		
	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];


	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}


	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		for(int i=0;i<mfs->inode[id]->file_size;i++)
			printf("%c",mfs->inode[id]->db->data[i]);

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);



		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<mfs->inode[id]->numS+1;i++)
			temp[i] = num[i-1];



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

void mycpto(char a_i[30],char a_i2[30]){   		//MY 파일시스템에 있는 파일을 일반 파일로 복사 mycpto 
	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	FILE *fp;

	fp = fopen(a_i2,"wb");

	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}


	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		for(int i=0;i<mfs->inode[id]->file_size;i++)
			printf("%c",mfs->inode[id]->db->data[i]);

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);


		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<mfs->inode[id]->numS+1;i++)
			temp[i] = num[i-1];


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

	int tmmp,v=0,check;
	char data[128];

	while(print != NULL){

		fwrite(&print->p->data,sizeof(print->p->data),1,fp);

		print = print->next;
	}

	fclose(fp);

	num = NULL;
	num2 = NULL;
}

void mymv(char a_i[20], char a_i2[20]){			//파일을 이동하거나 이름을 변경하는 	mymv
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


	if(pp == 999){

		printf("파일명이 정확하지 않습니다.\n");
		return;

	}else{

		ppp = mymv_func(a_i2);

		if(ppp == 999){
			//존재 x 파일명 바꾸기
			struct dtree *pppp = cur->left;
			struct dtree *pd = cur->left;
			char front[128],fback[128];
			char name[5];
			int i=0,o=0;

			sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);

			name[5] = '\0';

			while(pppp != NULL){
				pppp = pppp->right;
				o++;
			}

			pppp = cur->left;

			while( pd != NULL){

				if(!strcmp(pd->name,a_i)){

					strcpy(pd->name,name);

					for(int j=0;j<(i+2)*7+3;j++){
						front[j] = cur->p->db->data[j];
					}

					front[((i+2)*7)+3] = '\0';

					int j = (i+3)*7;
					int k = 0;

					for(int a=j;a<(o+2)*7;a++){
						fback[a-j] = cur->p->db->data[a];
					}

					fback[(o+2) * 7] = '\0';

					// front, fback 이용해서 아이노드 번호 이름 삭제

					sprintf(cur->p->db->data,"%s%-4s%s",front,name,fback);

					break;
				}

				pd = pd->right;

				i++;
			}



		}else{
			struct dtree *pd = cur->left;
			struct dtree *pdd = cur->left;
			struct dtree *pppp = cur->left;
			struct dtree *temmp= cur->left;
			struct dtree *usep = malloc(sizeof(struct dtree));;
			struct dtree *use = malloc(sizeof(struct dtree));;
			int i = 0,o=0,ii=0;
			char front[128],fback[128];
			char name[5];
			int inode;
			// 존재 o 파일 옮기기
			// 파일이면 리턴, 폴더면 폴더 들어감
			// 폴더 들어가고 아이노드 번호 쓰고 현재 폴더에 아이노드 번호 및 이름 삭제
			// 이제 파일 옮겨야 한다 -> cd로 들어간 후 맨마지막에 추가 
			// id = 아이노드 번호 tmp6 = 아이노드 이름
			// 연결 끊어준 후 폴더에 들어가서 연결 -> bback;
			while(pppp != NULL){
				pppp = pppp->right;
				o++;
			}
			pppp = cur->left;

			while(1){

				if(!strcmp(pdd->name,a_i)){

					// 앞 뒤 가져오기
					for(int j=0;j<(i+2)*7;j++){
						front[j] = cur->p->db->data[j];
					}
					front[((i+2)*7)] = '\0';

					int j = (i+3)*7;
					int k = 0;

					for(int a=j;a<(o+2)*7;a++){
						fback[a-j] = cur->p->db->data[a];
					}

					// front, fback 이용해서 아이노드 번호 이름 삭제

					sprintf(cur->p->db->data,"%s%s",front,fback);

					// 여기서 부터 끊어주자 먼저 바로 전 노드 검색


					for(int j=0;j<i-1;j++){
						temmp = temmp->right;
					}

					inode = pdd->inode;
					strcpy(name,pdd->name);


					// 트리구조 끊어준다.

					usep = pdd;
					ii = i;
					break;

				}
				pdd = pdd->right;
				i++;

			}

			while(1){

				if(!strcmp(pd->name,a_i2)){
					struct dtree *temp = pd->left;

					if(pd->p->file_type == 0){
						printf("파일 입니다.\n");
						return;
					}
					while(1){

						if(ii==0){
							if(o==1){
								cur->left = NULL;
								break;
							}
							cur->left = cur->left->right;
							break;
						}else{
							printf("inode = %d,name = %s\n",temmp->inode,temmp->name);
							temmp->right = temmp->right->right;
							break;
						}
					}

					if(temp == NULL){

						usep->right = NULL;
						usep->left = NULL;

						pd->left = usep;

					}else{
						while(temp->right != NULL)
							temp = temp->right;

						usep->right = NULL;
						usep->left= NULL;

						temp->right = usep;

					}

					// 데이터에 아이노드 이름 추가
					char all[128];

					strcpy(all,pd->p->db->data);
					sprintf(pd->p->db->data,"%s%-3d%-4s",all,inode,name);
					return;
				}
				pd = pd->right;

			}

		}

	}


}

int mymv_func(char a_i[20]){		//절대경로와 상대경로를 통해서 mymv함수로 접근
	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4],tmp6[20];
	int pp=0,ppp=0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i)){
			return 1;

			break;
		}

		pd = pd->right;
	}

	return 999;
}

void mycp(char a_i[20],char name[5]){		//MY 파일시스템에 있는 파일을 복사 mycp

	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	mycpto(a_i,"loperz");

	//파일 저장 완료
	// 다시 mycpfrom 하기

	mycpfrom("loperz",name);

	num = NULL;
	num2 = NULL;
}

void myshowfile(char a_i[20],char a_i2[20],char a_i3[20]){		//파일을 지정된 부분 출력 myshowfile
	int num11,num22;
	int loops,loopf,start,rem,remm;
	int remain = 0,remains = 0;

	num11 = atoi(a_i);
	num22 = atoi(a_i2);

	rem = num11 % 128;

	if(rem == 0){
		remm = num11 - (( (num11 / 128) - 1) * 128);
	}

	loops = num11 / 128;

	remains = num11 % 128;

	loopf = num22 / 128;

	remain = num22 % 128;

	start = num11 / 128;

	if(remain != 0)
		loopf += 1;

	if(remains == 0){
		start -= 1;
		loops -= 1;
	}

	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i3)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}

	if(mfs->inode[id]->sin == -1){

		int nn=loops;

		if(loops == 0 && loopf == 1){
			int abcd;
			int dcba;
			dcba = num11 % 129;
			abcd = num22 % 129;
			for(int i=dcba-1;i<abcd;i++)
				printf("%c",mfs->inode[id]->db->data[i]);
		}

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);

		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<103;i++)
			temp[i] = num[i-1];

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
		int nn=loops;

		for(int i=0;i<loops;i++)
			print = print->next;

		if(loops == 0 && loopf == 1){
			int abcd;
			abcd = num22 % 129;
			for(int i=0;i<abcd;i++)
				printf("%c",print->p->data[i]);
			return;
		}

		while(1){

			if(loopf == nn){
				for(int i=0;i<remain;i++){
					printf("%c",print->p->data[i]);
				}
				break;
			}
			if(loops == nn){

				if( rem == 0 ){
					printf("%c",print->p->data[remm]);
				}

				for(int i=remains-1;i<128;i++)
					printf("%c",print->p->data[i]);
			}

			if(loopf != nn && loops != nn){

				for(int i=0;i<128;i++)
					printf("%c",print->p->data[i]);

			}

			print = print->next;
			nn++;
		}

		return;
	}

	int loopQ,loopP;

	loopQ = mfs->inode[id]->num;
	loopP = mfs->inode[id]->numP;

	num = malloc(102 * sizeof(int));

	num = get_sidbit_ADD(id);

	num2 = malloc( ( ((loopQ - 1) * 102) + loopP +1 ) * sizeof(int));

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
	int nn=loops;

	for(int i=0;i<loops;i++)
		print = print->next;

	if(loops == 0 && loopf == 1){
		int abcd;
		abcd = num22 % 129;
		for(int i=0;i<abcd;i++)
			printf("%c",print->p->data[i]);
		return;
	}

	while(1){

		if(loopf == nn){
			for(int i=0;i<remain;i++){
				printf("%c",print->p->data[i]);
			}
			break;
		}
		if(loops == nn){

			if( rem == 0 ){
				printf("%c",print->p->data[remm]);
			}

			for(int i=remains-1;i<128;i++)
				printf("%c",print->p->data[i]);
		}

		if(loopf != nn && loops != nn){

			for(int i=0;i<128;i++)
				printf("%c",print->p->data[i]);

		}

		print = print->next;
		nn++;
	}

	num = NULL;
	num2 = NULL;
}

void myrm(char name[5]){		//파일을 삭제하는 명령 		myrm
	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];


	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,name)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}

	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		int dd;
		dd = mfs->inode[id]->dir;

		free(mfs->block[dd]);

		mfs->block[dd] = NULL;

		d_bit_del_func(dd);

		myrm_func2(name,id);

		struct dtree *pppp = cur->left;
		struct dtree *ppd = cur->left;
		char front[128], fback[128];
		int u=0,o=0;

		while(pppp != NULL){
			pppp = pppp->right;
			o++;
		}

		pppp = cur->left;

		while( ppd != NULL ){

			if(!strcmp(ppd->name,name)){

				for(int j=0;j<(u+2)*7;j++){
					front[j] = cur->p->db->data[j];
				}

				front[((u+2)*7)] = '\0';

				int j = (u+3)*7;
				int k = 0;

				for(int a=j;a<(o+2)*7;a++){
					fback[a-j] = cur->p->db->data[a];
				}

				fback[(o+2) * 7] = '\0';

				// front, fback 이용해서 아이노드 번호 이름 삭제

				sprintf(cur->p->db->data,"%s%s",front,fback);

				break;
			}

			ppd = ppd->right;

			u++;
		}


		i_bit_del(id);

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


		struct btree *head = NULL;
		struct btree *tail = NULL;

		b = 0;

		while(temp[b] != 0){

			free(mfs->block[temp[b]]);
			mfs->block[temp[b]] = NULL;

			d_bit_del_func(temp[b]);

			b++;
		}

		myrm_func2(name,id);

		struct dtree *pppp = cur->left;
		struct dtree *ppd = cur->left;
		char front[128], fback[128];
		int u=0,o=0;

		while(pppp != NULL){
			pppp = pppp->right;
			o++;
		}

		pppp = cur->left;

		while( ppd != NULL ){

			if(!strcmp(ppd->name,name)){

				for(int j=0;j<(u+2)*7;j++){
					front[j] = cur->p->db->data[j];
				}

				front[((u+2)*7)] = '\0';

				int j = (u+3)*7;
				int k = 0;

				for(int a=j;a<(o+2)*7;a++){
					fback[a-j] = cur->p->db->data[a];
				}

				fback[(o+2) * 7] = '\0';

				// front, fback 이용해서 아이노드 번호 이름 삭제

				sprintf(cur->p->db->data,"%s%s",front,fback);

				break;
			}

			ppd = ppd->right;

			u++;
		}


		i_bit_del(id);

		return;
	}

	int loopQ,loopP;

	loopQ = mfs->inode[id]->num;
	loopP = mfs->inode[id]->numP;

	num = malloc(102 * sizeof(int));

	num = get_sidbit_ADD(id);

	num2 = malloc( ( ((loopQ - 1) * 102) + loopP +1 ) * sizeof(int));

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

	temp[b+104] = '\0';


	struct btree *head = NULL;
	struct btree *tail = NULL;

	b = 0;


	while(temp[b] != 0){

		free(mfs->block[temp[b]]);
		mfs->block[temp[b]] = NULL;

		d_bit_del_func(temp[b]);

		b++;
	}

	myrm_func2(name,id);

	struct dtree *pppp = cur->left;
	struct dtree *ppd = cur->left;
	char front[128], fback[128];
	int u=0,o=0;

	while(pppp != NULL){
		pppp = pppp->right;
		o++;
	}

	pppp = cur->left;

	while( ppd != NULL ){

		if(!strcmp(ppd->name,name)){

			for(int j=0;j<(u+2)*7;j++){
				front[j] = cur->p->db->data[j];
			}

			front[((u+2)*7)] = '\0';

			int j = (u+3)*7;
			int k = 0;

			for(int a=j;a<(o+2)*7;a++){
				fback[a-j] = cur->p->db->data[a];
			}

			fback[(o+2) * 7] = '\0';

			// front, fback 이용해서 아이노드 번호 이름 삭제

			sprintf(cur->p->db->data,"%s%s",front,fback);

			break;
		}

		ppd = ppd->right;

		u++;
	}


	i_bit_del(id);

	num = NULL;
	num2 = NULL;

}

void myrm_func(char a_i[30]){      // 절대경로와 상대경로를 통해서 이 함수로 접근
	char *line,*line2;
	char tmp[30],tmp2[100][5];
	int s=0,i=0,a=0;

	strcpy(tmp,a_i);
	line = strtok(a_i,"/");


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
			myrm(tmp2[i]);
			cur = back2;
			strcpy(path,backup2);
			return;
		}

		mycd(tmp2[i]);

		i++;

	}


}

void myrm_func2(char a_i[20],int id){		// tree구조 끊어주기

	int s=0,k,ck=999,n=0,l,m=0,dd;
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
				if(mfs->inode[id]->file_type == 1){
					printf("파일이 아닙니다.\n");
					return;
				}
				cur->left = NULL;
				return;
			}
			t = cur->left;

			if(mfs->inode[id]->file_type == 1){
				printf("파일이 아닙니다.\n");
				return;
			}

			cur->left = cur->left->right;
			t->right = NULL;
			free(t);
			t = NULL; // 추가
			return;
		}

		for(int i=0;i<ck-3;i++){
			p = p->right;
		}

		if(mfs->inode[id]->file_type == 1){
			printf("파일이 아닙니다.\n");
			return;
		}
		back = p->right;
		p->right = p->right->right;
		back->right=NULL;
		free(back);

	}

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

void make_did(int inode){		//double indirect block 만들기
	int data;

	data = d_bit_check();

	mfs->block[data] = malloc(sizeof(struct d_block));

	mfs->inode[inode]->didb = mfs->block[data];
	mfs->inode[inode]->dou = data;

	for(int i=0;i<128;i++)
		mfs->inode[inode]->didb->data[i] = '\0';

	d_bit_insert();
}

void in_data(char a_i[20],int inode){		//direct block에 삽입
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

void sin_data(char a_i[30],int inode,int fpos){		//single indirect block이 가리키는 direct block에 삽입
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

void dsin_data(char a_i[30],int inode,int fpos){		//double indirect block이 가리키는 single indirect block이 가리키는 direct block에 삽입
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
void in_dir(char name[5],int inode){		//디렉터리안에 아이노드번호, 폴더이름, 파일이름 넣기
	char tmp[128],tmp2[128];

	sprintf(tmp,"%-7s",cur->p->db->data);

	sprintf(cur->p->db->data,"%-3d%-4s",inode,name);
	strcat(tmp,cur->p->db->data);

	sprintf(cur->p->db->data,"%-7s",tmp);

}

// ok
void mymkdir(char name[5]){			//디렉터리를 생성하는 명령어 mymkdir
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

void myls(char a_i2[30]){		//파일 목록을 출력하는 명령어 myls
	int s=0,k=0,inode;
	int width=0;
	char *temp[1024];
	int itemp[1024];
	char ttemp[1024];
	int *stemp[1024];
	char *ctemp[1024];
	char tmp[5],tmp1[4],type[2];
	struct dtree *p = cur->left;


	if(!strcmp(a_i2,"-i")){
		struct dtree *ioi = cur->left;
		while(ioi != NULL)
		{
			temp[width]=(char *)calloc(strlen(ioi->name)+1, sizeof(char));
			strcpy(temp[width], ioi->name);
			itemp[width]=ioi->inode;
			ioi=ioi->right;
			width++;
		}
		swap2(temp, itemp, width-1);
		printf(".\n..\n");
		for(int i=0; i<width; i++)
			printf("%d %s\n", itemp[i], temp[i]);

		return;

	}

	if(!strcmp(a_i2,"-l")){
		return;

	}

	if( (!strcmp(a_i2,"-li")) || (!strcmp(a_i2,"-il")) ){

		return;
	}


	struct dtree *ioi = cur->left;


	while(ioi != NULL){
		temp[width]=(char *)calloc(strlen(ioi->name)+1, sizeof(char));
		strcpy(temp[width], ioi->name);
		width++;
		ioi = ioi->right;
	}
	swap(temp, width-1);

	printf(".\n..\n");
	for(int i=0; i<width; i++)
		printf("%s\n", temp[i]);

	return;
}

void myls_func(char a_i[20],char a_i2[20]){   //절대경로와 상대경로를 통해 myls함수 접근 
	int s;
	char *line;
	char tmp;

	line = strtok(a_i,"/");

	strcpy(backup2,path);

	back2 = cur;

	if(a_i[0] == '/'){
		mycd(a_i);
	}

	if(!strcmp(a_i,"-i")){
		myls(a_i);
		return;
	}else if(!strcmp(a_i,"-l")){
		myls(a_i);
		return;
	}else if(!strcmp(a_i,"-l")){
		myls(a_i);
		return;
	}

	while(line != NULL){

		s =	mycd(line);

		if(s == 1){
			cur = back2;
			return;
		}

		line = strtok(NULL,"/");
	}

	myls(a_i2);

	cur = back2;

	strcpy(path,backup2);

	return;

}
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
void swap(char *temp[], int  width) // myls 사전식배열
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



int mycd(char *a_i){		// 작업 디렉터리를 이동하는 명령어 mycd
	int s=0,k,ck=999,n=0;
	char tmp[5],tmp2[40];
	struct dtree *p = cur->left;
	struct dtree *test = cur->left;

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
			if(test == NULL){
				return 4;
			}

			if(!strcmp(test->name,a_i)){

				if(test->p->file_type == 0){
					printf("잘못된 경로 입니다.\n");
					strcpy(path,backup2);
					return 1;
				}

				cur = test;

				sprintf(tmp2,"%s",path);

				if(path[0] == '/' && path[1] == '\0'){
					sprintf(path,"/%s",a_i);
				}else{
					sprintf(path,"%s/%s",tmp2,a_i);
				}
				break;
			}

			if(test->right == NULL){
				printf("잘못된 경로 입니다.\n");
				break;
			}
			test = test->right;
		}

	}



}

int mycd_func(char a_i[30]){		//절대경로와 상대경로를 통해 mycd함수 접근
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

		s =	mycd(line);

		if(s == 1){
			cur = back2;
			return 1;
		}

		line = strtok(NULL,"/");
	}

}


void myrmdir(char a_i[30]){			//디렉터리를 삭제하는 명령어 myrmdir
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
			t = NULL; 
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
void myrmdir_func(char a_i[30]){		//절대경로와 상대경로를 통해서 myrmdir 함수 접근
	char *line,*line2;
	char tmp[30],tmp2[100][5];
	int s=0,i=0,a=0;

	strcpy(tmp,a_i);
	line = strtok(a_i,"/");


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

void mytree(struct dtree *tr,int d){		//지정된 디렉터리부터 디렉터리 구조  출력. 지정안되면 현재 디렉터리부터 구조 출력 mytree

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

void mytree_func(struct dtree *tr,int d){		//절대경로와 상대경로를 통해서 mytree함수 접근


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

void myshowinode(char a_i[30]){			//지정된 inode에 들어있는 애용 보기 myshowinode
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

void myshowblock(char a_i[30]){			//지정된 데이터 블록에 들어있는 내용 보기	myshowblock
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

void mystate(){			//현재 파일시스템의 상태 출력 	mystate
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

void mycpfrom(char a_i[20],char name[5]){		//일반 파일을 MY 파일시스템의 파일로 복사  	mycpfrom

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

void mytouch(char name[5]){			//파일 수정 시간을 변경하는 명령어 명시된 파일이 현재 디렉터리에 없을 경우하는 크기 0바이트mytouch
	int s=0,k,ck=-1,n=0,ckin= -1;
	int inode,data;
	char tmp[5],tmp1[4];
	struct dtree *p = cur->left;


	while(p != NULL){

		if(!strcmp(p->name,name)){

			ckin = p->inode;
			break;
		}


		p = p->right;
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

void mytouch_func(char a_i[20]){		//절대경로와 상대경로를 통해서 mytouch함수 접근
	char *line,*line2;
	char tmp[30],tmp2[100][5];
	int s=0,i=0,a=0;

	strcpy(tmp,a_i);
	line = strtok(a_i,"/");


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
			mytouch(tmp2[i]);
			cur = back2;
			strcpy(path,backup2);
			return;
		}

		mycd(tmp2[i]);

		i++;

	}

}

void mycat(char a_i[30]){		//파일 연결 몇 출력 명령어 		mycat		
	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];


	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}


	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		for(int i=0;i<mfs->inode[id]->file_size;i++)
			printf("%c",mfs->inode[id]->db->data[i]);

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);



		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<mfs->inode[id]->numS+1;i++)
			temp[i] = num[i-1];



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

void mycpto(char a_i[30],char a_i2[30]){   		//MY 파일시스템에 있는 파일을 일반 파일로 복사 mycpto 
	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	FILE *fp;

	fp = fopen(a_i2,"wb");

	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}


	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		for(int i=0;i<mfs->inode[id]->file_size;i++)
			printf("%c",mfs->inode[id]->db->data[i]);

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);


		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<mfs->inode[id]->numS+1;i++)
			temp[i] = num[i-1];


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

	int tmmp,v=0,check;
	char data[128];

	while(print != NULL){

		fwrite(&print->p->data,sizeof(print->p->data),1,fp);

		print = print->next;
	}

	fclose(fp);

	num = NULL;
	num2 = NULL;
}

void mymv(char a_i[20], char a_i2[20]){			//파일을 이동하거나 이름을 변경하는 	mymv
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


	if(pp == 999){

		printf("파일명이 정확하지 않습니다.\n");
		return;

	}else{

		ppp = mymv_func(a_i2);

		if(ppp == 999){
			//존재 x 파일명 바꾸기
			struct dtree *pppp = cur->left;
			struct dtree *pd = cur->left;
			char front[128],fback[128];
			char name[5];
			int i=0,o=0;

			sprintf(name,"%c%c%c%c",a_i2[0],a_i2[1],a_i2[2],a_i2[3]);

			name[5] = '\0';

			while(pppp != NULL){
				pppp = pppp->right;
				o++;
			}

			pppp = cur->left;

			while( pd != NULL){

				if(!strcmp(pd->name,a_i)){

					strcpy(pd->name,name);

					for(int j=0;j<(i+2)*7+3;j++){
						front[j] = cur->p->db->data[j];
					}

					front[((i+2)*7)+3] = '\0';

					int j = (i+3)*7;
					int k = 0;

					for(int a=j;a<(o+2)*7;a++){
						fback[a-j] = cur->p->db->data[a];
					}

					fback[(o+2) * 7] = '\0';

					// front, fback 이용해서 아이노드 번호 이름 삭제

					sprintf(cur->p->db->data,"%s%-4s%s",front,name,fback);

					break;
				}

				pd = pd->right;

				i++;
			}



		}else{
			struct dtree *pd = cur->left;
			struct dtree *pdd = cur->left;
			struct dtree *pppp = cur->left;
			struct dtree *temmp= cur->left;
			struct dtree *usep = malloc(sizeof(struct dtree));;
			struct dtree *use = malloc(sizeof(struct dtree));;
			int i = 0,o=0,ii=0;
			char front[128],fback[128];
			char name[5];
			int inode;
			// 존재 o 파일 옮기기
			// 파일이면 리턴, 폴더면 폴더 들어감
			// 폴더 들어가고 아이노드 번호 쓰고 현재 폴더에 아이노드 번호 및 이름 삭제
			// 이제 파일 옮겨야 한다 -> cd로 들어간 후 맨마지막에 추가 
			// id = 아이노드 번호 tmp6 = 아이노드 이름
			// 연결 끊어준 후 폴더에 들어가서 연결 -> bback;
			while(pppp != NULL){
				pppp = pppp->right;
				o++;
			}
			pppp = cur->left;

			while(1){

				if(!strcmp(pdd->name,a_i)){

					// 앞 뒤 가져오기
					for(int j=0;j<(i+2)*7;j++){
						front[j] = cur->p->db->data[j];
					}
					front[((i+2)*7)] = '\0';

					int j = (i+3)*7;
					int k = 0;

					for(int a=j;a<(o+2)*7;a++){
						fback[a-j] = cur->p->db->data[a];
					}

					// front, fback 이용해서 아이노드 번호 이름 삭제

					sprintf(cur->p->db->data,"%s%s",front,fback);

					// 여기서 부터 끊어주자 먼저 바로 전 노드 검색


					for(int j=0;j<i-1;j++){
						temmp = temmp->right;
					}

					inode = pdd->inode;
					strcpy(name,pdd->name);


					// 트리구조 끊어준다.

					usep = pdd;
					ii = i;
					break;

				}
				pdd = pdd->right;
				i++;

			}

			while(1){

				if(!strcmp(pd->name,a_i2)){
					struct dtree *temp = pd->left;

					if(pd->p->file_type == 0){
						printf("파일 입니다.\n");
						return;
					}
					while(1){

						if(ii==0){
							if(o==1){
								cur->left = NULL;
								break;
							}
							cur->left = cur->left->right;
							break;
						}else{
							printf("inode = %d,name = %s\n",temmp->inode,temmp->name);
							temmp->right = temmp->right->right;
							break;
						}
					}

					if(temp == NULL){

						usep->right = NULL;
						usep->left = NULL;

						pd->left = usep;

					}else{
						while(temp->right != NULL)
							temp = temp->right;

						usep->right = NULL;
						usep->left= NULL;

						temp->right = usep;

					}

					// 데이터에 아이노드 이름 추가
					char all[128];

					strcpy(all,pd->p->db->data);
					sprintf(pd->p->db->data,"%s%-3d%-4s",all,inode,name);
					return;
				}
				pd = pd->right;

			}

		}

	}


}

int mymv_func(char a_i[20]){		//절대경로와 상대경로를 통해서 mymv함수로 접근
	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4],tmp6[20];
	int pp=0,ppp=0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i)){
			return 1;

			break;
		}

		pd = pd->right;
	}

	return 999;
}

void mycp(char a_i[20],char name[5]){		//MY 파일시스템에 있는 파일을 복사 mycp

	int s=0,k,ck=999,n=0,l,m=0,id,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	mycpto(a_i,"loperz");

	//파일 저장 완료
	// 다시 mycpfrom 하기

	mycpfrom("loperz",name);

	num = NULL;
	num2 = NULL;
}

void myshowfile(char a_i[20],char a_i2[20],char a_i3[20]){		//파일을 지정된 부분 출력 myshowfile
	int num11,num22;
	int loops,loopf,start,rem,remm;
	int remain = 0,remains = 0;

	num11 = atoi(a_i);
	num22 = atoi(a_i2);

	rem = num11 % 128;

	if(rem == 0){
		remm = num11 - (( (num11 / 128) - 1) * 128);
	}

	loops = num11 / 128;

	remains = num11 % 128;

	loopf = num22 / 128;

	remain = num22 % 128;

	start = num11 / 128;

	if(remain != 0)
		loopf += 1;

	if(remains == 0){
		start -= 1;
		loops -= 1;
	}

	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];

	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,a_i3)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}

	if(mfs->inode[id]->sin == -1){

		int nn=loops;

		if(loops == 0 && loopf == 1){
			int abcd;
			int dcba;
			dcba = num11 % 129;
			abcd = num22 % 129;
			for(int i=dcba-1;i<abcd;i++)
				printf("%c",mfs->inode[id]->db->data[i]);
		}

		return;
	}

	if(mfs->inode[id]->dou == -1){
		num = malloc(102 * sizeof(int));
		num = get_sidbit_ADD(id);

		int b=0;

		temp[0] = mfs->inode[id]->dir;

		for(int i=1;i<103;i++)
			temp[i] = num[i-1];

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
		int nn=loops;

		for(int i=0;i<loops;i++)
			print = print->next;

		if(loops == 0 && loopf == 1){
			int abcd;
			abcd = num22 % 129;
			for(int i=0;i<abcd;i++)
				printf("%c",print->p->data[i]);
			return;
		}

		while(1){

			if(loopf == nn){
				for(int i=0;i<remain;i++){
					printf("%c",print->p->data[i]);
				}
				break;
			}
			if(loops == nn){

				if( rem == 0 ){
					printf("%c",print->p->data[remm]);
				}

				for(int i=remains-1;i<128;i++)
					printf("%c",print->p->data[i]);
			}

			if(loopf != nn && loops != nn){

				for(int i=0;i<128;i++)
					printf("%c",print->p->data[i]);

			}

			print = print->next;
			nn++;
		}

		return;
	}

	int loopQ,loopP;

	loopQ = mfs->inode[id]->num;
	loopP = mfs->inode[id]->numP;

	num = malloc(102 * sizeof(int));

	num = get_sidbit_ADD(id);

	num2 = malloc( ( ((loopQ - 1) * 102) + loopP +1 ) * sizeof(int));

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
	int nn=loops;

	for(int i=0;i<loops;i++)
		print = print->next;

	if(loops == 0 && loopf == 1){
		int abcd;
		abcd = num22 % 129;
		for(int i=0;i<abcd;i++)
			printf("%c",print->p->data[i]);
		return;
	}

	while(1){

		if(loopf == nn){
			for(int i=0;i<remain;i++){
				printf("%c",print->p->data[i]);
			}
			break;
		}
		if(loops == nn){

			if( rem == 0 ){
				printf("%c",print->p->data[remm]);
			}

			for(int i=remains-1;i<128;i++)
				printf("%c",print->p->data[i]);
		}

		if(loopf != nn && loops != nn){

			for(int i=0;i<128;i++)
				printf("%c",print->p->data[i]);

		}

		print = print->next;
		nn++;
	}

	num = NULL;
	num2 = NULL;
}

void myrm(char name[5]){		//파일을 삭제하는 명령 		myrm
	int s=0,k,ck=999,n=0,l,m=0,id=-1,dd;
	char tmp[5],tmp2[100],tmp3[100],tmp4[200],tmp5[4];
	int *num,*num2,i=0,temp[1024];


	for(int i=0;i<1024;i++)
		temp[i] = 0;

	struct dtree *pd = cur->left;

	while(pd != NULL){

		if(!strcmp(pd->name,name)){
			if(pd->p->file_type == 1){
				printf("폴더 입니다.\n");
				return;
			}
			id = pd->inode;

			break;
		}

		pd = pd->right;
	}

	if(id == -1){
		printf("파일명을 정확히 입력해 주세요.\n");
		return;
	}

	if(mfs->inode[id]->sin == -1){
		// 다이렉트 블럭
		int dd;
		dd = mfs->inode[id]->dir;

		free(mfs->block[dd]);

		mfs->block[dd] = NULL;

		d_bit_del_func(dd);

		myrm_func2(name,id);

		struct dtree *pppp = cur->left;
		struct dtree *ppd = cur->left;
		char front[128], fback[128];
		int u=0,o=0;

		while(pppp != NULL){
			pppp = pppp->right;
			o++;
		}

		pppp = cur->left;

		while( ppd != NULL ){

			if(!strcmp(ppd->name,name)){

				for(int j=0;j<(u+2)*7;j++){
					front[j] = cur->p->db->data[j];
				}

				front[((u+2)*7)] = '\0';

				int j = (u+3)*7;
				int k = 0;

				for(int a=j;a<(o+2)*7;a++){
					fback[a-j] = cur->p->db->data[a];
				}

				fback[(o+2) * 7] = '\0';

				// front, fback 이용해서 아이노드 번호 이름 삭제

				sprintf(cur->p->db->data,"%s%s",front,fback);

				break;
			}

			ppd = ppd->right;

			u++;
		}


		i_bit_del(id);

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


		struct btree *head = NULL;
		struct btree *tail = NULL;

		b = 0;

		while(temp[b] != 0){

			free(mfs->block[temp[b]]);
			mfs->block[temp[b]] = NULL;

			d_bit_del_func(temp[b]);

			b++;
		}

		myrm_func2(name,id);

		struct dtree *pppp = cur->left;
		struct dtree *ppd = cur->left;
		char front[128], fback[128];
		int u=0,o=0;

		while(pppp != NULL){
			pppp = pppp->right;
			o++;
		}

		pppp = cur->left;

		while( ppd != NULL ){

			if(!strcmp(ppd->name,name)){

				for(int j=0;j<(u+2)*7;j++){
					front[j] = cur->p->db->data[j];
				}

				front[((u+2)*7)] = '\0';

				int j = (u+3)*7;
				int k = 0;

				for(int a=j;a<(o+2)*7;a++){
					fback[a-j] = cur->p->db->data[a];
				}

				fback[(o+2) * 7] = '\0';

				// front, fback 이용해서 아이노드 번호 이름 삭제

				sprintf(cur->p->db->data,"%s%s",front,fback);

				break;
			}

			ppd = ppd->right;

			u++;
		}


		i_bit_del(id);

		return;
	}

	int loopQ,loopP;

	loopQ = mfs->inode[id]->num;
	loopP = mfs->inode[id]->numP;

	num = malloc(102 * sizeof(int));

	num = get_sidbit_ADD(id);

	num2 = malloc( ( ((loopQ - 1) * 102) + loopP +1 ) * sizeof(int));

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

	temp[b+104] = '\0';


	struct btree *head = NULL;
	struct btree *tail = NULL;

	b = 0;


	while(temp[b] != 0){

		free(mfs->block[temp[b]]);
		mfs->block[temp[b]] = NULL;

		d_bit_del_func(temp[b]);

		b++;
	}

	myrm_func2(name,id);

	struct dtree *pppp = cur->left;
	struct dtree *ppd = cur->left;
	char front[128], fback[128];
	int u=0,o=0;

	while(pppp != NULL){
		pppp = pppp->right;
		o++;
	}

	pppp = cur->left;

	while( ppd != NULL ){

		if(!strcmp(ppd->name,name)){

			for(int j=0;j<(u+2)*7;j++){
				front[j] = cur->p->db->data[j];
			}

			front[((u+2)*7)] = '\0';

			int j = (u+3)*7;
			int k = 0;

			for(int a=j;a<(o+2)*7;a++){
				fback[a-j] = cur->p->db->data[a];
			}

			fback[(o+2) * 7] = '\0';

			// front, fback 이용해서 아이노드 번호 이름 삭제

			sprintf(cur->p->db->data,"%s%s",front,fback);

			break;
		}

		ppd = ppd->right;

		u++;
	}


	i_bit_del(id);

	num = NULL;
	num2 = NULL;

}

void myrm_func(char a_i[30]){      // 절대경로와 상대경로를 통해서 이 함수로 접근
	char *line,*line2;
	char tmp[30],tmp2[100][5];
	int s=0,i=0,a=0;

	strcpy(tmp,a_i);
	line = strtok(a_i,"/");


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
			myrm(tmp2[i]);
			cur = back2;
			strcpy(path,backup2);
			return;
		}

		mycd(tmp2[i]);

		i++;

	}


}

void myrm_func2(char a_i[20],int id){		// tree구조 끊어주기

	int s=0,k,ck=999,n=0,l,m=0,dd;
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
				if(mfs->inode[id]->file_type == 1){
					printf("파일이 아닙니다.\n");
					return;
				}
				cur->left = NULL;
				return;
			}
			t = cur->left;

			if(mfs->inode[id]->file_type == 1){
				printf("파일이 아닙니다.\n");
				return;
			}

			cur->left = cur->left->right;
			t->right = NULL;
			free(t);
			t = NULL; // 추가
			return;
		}

		for(int i=0;i<ck-3;i++){
			p = p->right;
		}

		if(mfs->inode[id]->file_type == 1){
			printf("파일이 아닙니다.\n");
			return;
		}
		back = p->right;
		p->right = p->right->right;
		back->right=NULL;
		free(back);

	}

}
