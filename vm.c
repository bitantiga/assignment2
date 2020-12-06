#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define IN_TXT "addresses.txt"
#define OUT_TXT "out.txt"
#define BIN_TXT "BACKING_STORE.bin"
typedef unsigned int uint;
typedef int bool;
int main(){
    FILE *f;
    FILE *outf; 
    uint addresses[2000];

    int len;    
    //文件处理
    outf=fopen(OUT_TXT,"w+");
    if((f=fopen(IN_TXT,"r"))==NULL)
        return 0;
    int ad_count=0;
    while(fscanf(f, "%d", &addresses[ad_count]) != EOF) {
        ad_count++;
    }
    fclose(f);
    uint TLB[2][16];
    for(int i=0;i<16;i++){
        TLB[0][i]=-1;
        TLB[1][i]=-1;
    }
    uint map[256];
    char pmemory[256][256];
    for(int i=0;i<256;i++){
        for(int j=0;j<256;j++){
            pmemory[i][j]=-1;
            pmemory[i][j]=-1;
        }
        map[i]=-1;
    }
    uint f8=0xFF00;
    uint b8=0XFF;
    char frame;
    int pm_num=0;
    for(int i=0;i<=ad_count;i++){
        uint vaddress=(addresses[i]&f8)>>8;//读取虚拟地址
        uint offset=addresses[i]&b8;//读取偏移量
        bool is_TLB=0;
        for(int i=0;i<16;i++){
            if(TLB[0][i]==vaddress){//本题没有使用TLB
                frame=pmemory[TLB[1][i]][offset];
                is_TLB=1;
                break;
            }
            if(TLB[0][i]==-1){
            }
        }
        if(is_TLB==0){
            if(map[vaddress]==-1){
                printf("页缺失");
                FILE *f_bin=fopen(BIN_TXT,"rb");//读取文件
                fseek(f_bin,256*pm_num,0);//定位 file num begin
                char *buffer=(char*)malloc(257);//开空间
                fread(buffer,1,256,f_bin);//读入数据
                for(int j=0;j<256;j++)
                    pmemory[pm_num][j]=buffer[j];//存入内存
                free(buffer);//释放空间
                fclose(f_bin);//关闭文件
                map[vaddress]=pm_num;//更新map
                frame=buffer[offset];//输出帧
                pm_num++;
            }
            else{
                frame=map[vaddress];
            }

        }
    }
	FILE *wfp;
    wfp = fopen("out.txt","w+");
    for(int i = 0 ;i<ad_count;i++){//输出文件
        uint vaddress;
        uint offset;
        uint paddress;
        char value;
        vaddress=(addresses[i]&f8)>>8;
        offset=(addresses[i]&b8);
        paddress=map[vaddress]*256+offset;
        value=pmemory[map[vaddress]][offset];
        printf("va:%d,pa:%d,value:%d\n",addresses[i],paddress,value);

        char writes[100] = "Virtual address: ";
        char VirtualAddressC[10];
        char PhysicalAddressC[10];
        char ValueS[10];
        sprintf(VirtualAddressC,"%d",addresses[i]);
        sprintf(PhysicalAddressC,"%d",paddress);
        sprintf(ValueS,"%d",value);
        strcat(writes,VirtualAddressC);
        strcat(writes," Physical address: ");
        strcat(writes,PhysicalAddressC);
        strcat(writes," Value: ");
        strcat(writes,ValueS);
        strcat(writes,"\n");
        fputs(writes,wfp);
    }


}