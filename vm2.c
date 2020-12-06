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
    int TLB_f_num=0;
    int TLB_nf_num=0;
    int page_f_num=0;
    int page_nf_num=0;
    int page_full=0;
    int TLB_full=0;
    int is_TLB_grade=-1;
    int is_page=0; 
    int is_in_page=0;
    int is_turn_page=0;  
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
    FILE *wfp;
    wfp = fopen("out.txt","w+");
    uint map[128];
    char pmemory[128][256];
    for(int i=0;i<256;i++){
        for(int j=0;j<128;j++){
            pmemory[j][i]=-1;
            pmemory[j][i]=-1;
        }
        map[i]=-1;
    }
    uint f8=0xFF00;
    uint b8=0XFF;
    char frame;
    int pm_num=0;
    for(int i=0;i<=ad_count;i++){

        uint vaddress=(addresses[i]&f8)>>8;
        uint offset=addresses[i]&b8;
        bool is_TLB=0;

        for(int i=0;i<16;i++){

            if(TLB[0][i]==vaddress){//在TLB中找到了

                frame=pmemory[TLB[1][i]][offset];

                is_TLB=1;
                TLB_f_num++;

                break;
            }            
            if(TLB[0][i]==-1){//有空的TLB
                is_TLB_grade=i;
                break;
            }
        }
        if(is_TLB==0){//TLB里面没有

            TLB_nf_num++;
            for(int i = 0;i<128;i++){//在页表里面找
                if(map[i]==vaddress){//hit
                    page_f_num++;
                    is_page=1;
                    break;
                }
                if(map[i]==-1){//有空的页表
                    FILE *f_bin=fopen(BIN_TXT,"rb");//读取文件
                    fseek(f_bin,256*vaddress,0);//定位 file num begin
                    char *buffer=(char*)malloc(257);//开空间
                    fread(buffer,1,256,f_bin);//读入数据
                    for(int j=0;j<256;j++){
                        //printf("%d\n",pm_num);
                        pmemory[i][j]=buffer[j];//存入内存
                    }
                    free(buffer);//释放空间
                    fclose(f_bin);//关闭文件
                    map[i]=vaddress;//更新map
                    frame=buffer[offset];//输出帧
                    page_nf_num++;
                    //printf("pmnum%d\n",pm_num);
                    is_page=1;
                    if(is_TLB_grade+1){//如果TLB有空位
                        TLB[0][is_TLB_grade]=vaddress;
                        TLB[1][is_TLB_grade]=i;
                        //printf("is_tlb%d\n",is_TLB_grade);
                        is_TLB_grade=-1;
                    }
                    break;
                }
            }
            if(is_page==0){//not hit
                FILE *f_bin=fopen(BIN_TXT,"rb");//读取文件
                fseek(f_bin,256*vaddress,0);//定位 file num begin
                char *buffer=(char*)malloc(257);//开空间
                fread(buffer,1,256,f_bin);//读入数据
                for(int j=0;j<256;j++)
                    pmemory[page_full][j]=buffer[j];//存入内存
                free(buffer);//释放空间
                fclose(f_bin);//关闭文件
                map[page_full]=vaddress;//更新map
                frame=buffer[offset];//输出帧
                //printf("pfull%d\n",page_full);
                page_full++;//page_full用于循环，事实上，这里的页表是从低往高读入的，所以从低往高更新就可以实现FIFO了
                if(page_full==128)//循环到了一轮的尽头，回到起始的位置
                    page_full=0;
                page_nf_num++;
                is_page=1;
                is_turn_page=1;
            }
            if(is_TLB_grade==-1){
                TLB[0][TLB_full]=vaddress;
                if(is_turn_page)
                    TLB[1][TLB_full]=page_full;
                TLB_full++;//TLB的FIFO
                if(TLB_full==16){
                    TLB_full=0;
                }
            }

        }
        is_turn_page=0;
        is_in_page=0;
        is_TLB=0;
    }

for(int i = 0 ;i<ad_count;i++){
        uint vaddress;
        uint offset;
        uint paddress;
        char value;
        vaddress=(addresses[i]&f8)>>8;
        offset=(addresses[i]&b8);
        printf("va:%d,pa:%d,value:%d\n",addresses[i],paddress,value);
    }
    printf("页表命中:%d,页表未命中:%d,TLB命中:%d,TLB未命中:%d",page_f_num,page_nf_num,TLB_f_num,TLB_nf_num);
}