#include<stdio.h>
#include"mpi.h"
#include "stdlib.h"
#define gather_size 10
int main(int argc,char **argv)
{
  int rank,proc_size;
  int bcast_value;//广播的值
  int send_gather_num;//收集每个进程的单个值
  int send_gather_list[gather_size];//收集每个进程的多个值
  int *gather_num_recv;//单个值的收集结果
  int *gather_list_recv;//多个值的收集结果
//gatherv
  int gatherv[gather_size];//每个进程还准备gather_size个值
  int *displs;//存放各个进程收集到的数据在收集结果中的存放位置（相对于首地址的偏移）
  int *every_proc_count;//存放从每个进程收集数字的数目。
  int *gatherv_recv;//gatherv的收集结果


  MPI_Init(&argc,&argv);
  printf("before boradcast,bcast_value is %d, in %p\n",bcast_value,&bcast_value);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&proc_size);
  if(rank==0){
    scanf("%d",&bcast_value);//输入要广播给各个进程的值。
  }
  MPI_Bcast(&bcast_value,1,MPI_INT,0,MPI_COMM_WORLD);//广播
  printf("after boradcast,bcast_value is %d, in %p\n",bcast_value,&bcast_value);//各进程打印广播后的bcast_value,广播完成。


  send_gather_num=bcast_value*rank;//收集的单个值等于刚才广播的值乘本身进程号
  for(int i=0;i<gather_size;i++){
    send_gather_list[i]=send_gather_num+i;//收集的多个值从上面的send_gather_num开始递增。
    gatherv[i]=rank;//gatherv收集的多个值都是本身的进程号
  }

//gatherv的收集方法

//各进程已准备好被收集的数据,申请收集结果存放的空间。
  if(rank==0){
    gather_num_recv=(int *)malloc(proc_size*sizeof(int));//proc_size个整数
    gather_list_recv=(int *)malloc(proc_size*gather_size*sizeof(int));//收集proc_size个长度为gather_size的整数数组，需要proc_size*gathersize的整数空间。

    every_proc_count=(int *)malloc(proc_size*sizeof(int));//准备gatherv的收集数目数组空间
    displs=(int *)malloc(proc_size*sizeof(int));//偏移位置数组
    int malloc_size=0;
    for(int i=0;i<proc_size;i++){
      malloc_size+=i+1;
    }//每个进程收集进城号+1个数字，需要的总空间累加。
    gatherv_recv=(int *)malloc(malloc_size*sizeof(int));//gatherv收集结果的空间
    //定义收集数目和偏移位置
    int location=0;
    for(int j=0;j<proc_size;j++){
      every_proc_count[j]=j+1;//从第i个进程收集i+1个数字。
      location+=j;
      displs[j]=location;//每个进程收集到的数字在结果数组中的首地址偏移量。
    }
    printf("start to gather.\n");
  }

  //收集
  MPI_Gather(&send_gather_num,1,MPI_INT,gather_num_recv,1,MPI_INT,0,MPI_COMM_WORLD);//进程0作为根进程
  MPI_Gather(send_gather_list,gather_size,MPI_INT,gather_list_recv,gather_size,MPI_INT,0,MPI_COMM_WORLD);//注意数组名是指向数组第一个元素的指针，跟上一句有区别。

  MPI_Gatherv(gatherv,rank+1,MPI_INT,gatherv_recv,every_proc_count,displs,MPI_INT,0,MPI_COMM_WORLD);//
  //测试结果
  if(rank==0)
  {
    for(int j=0;j<proc_size;j++){
      printf("-----------------------------------------------------\n");
      printf("gathered %d from proc %d.\n",gather_num_recv[j],j);
      printf("gathered list :");
      for(int k=0;k<gather_size;k++){
        printf("%d,",gather_list_recv[j*gather_size+k]);
      }
      printf("\n");
    }
    //int res_size=sizeof(gatherv_recv)/sizeof(gatherv_recv[0]);
    int res_size=proc_size;
    for(int dummy=0;dummy<proc_size;dummy++){
      res_size+=dummy;//不知道该怎么拿到数组长度,再写个循环吧。。
    }
    for(int m=0;m<res_size;m++){
      printf("%d, ",gatherv_recv[m]);
    }
    printf("\n");
    printf("the number dedcated from which processor that root gathered.\n");
  }
  MPI_Finalize();
  return 0;
}
