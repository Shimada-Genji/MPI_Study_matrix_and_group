#include<stdio.h>
#include"mpi.h"
#include"stdlib.h"
#include <unistd.h>
#define gather_size 10
#define dummy 1


int main(int argc,char **argv)
{
  int proc_nums,rank;
  int send_buff[gather_size];//allgather的发送缓冲区
  int *recv_buff;//allgather的接收缓冲区

  int v_send_buff[gather_size];//allgatherv的发送缓冲区
  int *v_recv_buff;//allgatherv的接收缓冲区
  int *sendcounts;//收集数目数组
  int *displs;//存放位置偏移数组
  int malloc_size;//allgatherv的接收缓冲区大小

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&proc_nums);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  //定义收集数目数组，存放位置偏移数组
  sendcounts=(int *)malloc(proc_nums*sizeof(int));
  displs=(int *)malloc(proc_nums*sizeof(int));
  int location=0;
  for(int j=0;j<proc_nums;j++){
    sendcounts[j]=j+1;//从第i个进程收集i+1个数字。
    location+=j;
    displs[j]=location;//每个进程收集到的数字在结果数组中的首地址偏移量。
  }
    malloc_size=location+proc_nums;

  //printf("processor %d malloc_size is %d,first count is %d,first loc %d,second loc %d ",rank,malloc_size,sendcounts[0],displs[0],displs[1]);
  //每个进程都申请接收数组空间
  recv_buff=(int *)malloc(proc_nums*gather_size*sizeof(int));
  v_recv_buff=(int *)malloc(malloc_size*sizeof(int));
  //定义allgather与allgahterv过程中，每个进程发送缓冲区的内容
  for(int p=0;p<gather_size;p++){send_buff[p]=rank;}
  for(int q=0;q<rank+1;q++){v_send_buff[q]=rank;}
  //全收集
  MPI_Allgather(send_buff,gather_size,MPI_INT,recv_buff,gather_size,MPI_INT,MPI_COMM_WORLD);
  MPI_Allgatherv(v_send_buff,sendcounts[rank],MPI_INT,v_recv_buff,sendcounts,displs,MPI_INT,MPI_COMM_WORLD);//每个进程都向其余进程收集对方进程号+1个数据
  //测试
  for(int h=0;h<proc_nums;h++){
    sleep(dummy);//打印顺序太乱，不知道怎么办，停一秒再继续。嘻嘻。
    if(rank==h){
      printf("------------------------------------------\n");
    printf("processor %d has gathered:",rank);
    for(int w=0;w<gather_size*proc_nums;w++){
      printf("%d,",recv_buff[w]);
    }
    printf("\n");
    for(int f=0;f<malloc_size;f++){
      printf("%d,",v_recv_buff[f]);
    }
    printf("\n");
  }
  }
  MPI_Finalize();
  return 0;
}
