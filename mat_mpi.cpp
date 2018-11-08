#include<stdio.h>
#include"mpi.h"
#include<time.h>

#define A_ROW 500
#define A_CLO_B_ROW 600
#define B_CLO 700
#define SHUT_DOWN_TAG -1

int main(void)
{
  //初始化
  int mat1[A_ROW][A_CLO_B_ROW];
  int mat2[A_CLO_B_ROW][B_CLO];
  for(int i=0;i<A_CLO_B_ROW;i++)
  {
    for(int j=0;j<A_ROW;j++){
      //mat1[j][i]=j*10+i;
      mat1[j][i]=1;
    }
    for(int k=0;k<B_CLO;k++){
      //mat2[i][k]=i*10+k;
      mat2[i][k]=1;
    }
  }
  //result
  int res[A_ROW][B_CLO];
  int res_chuan[A_ROW][B_CLO];
  for (int e=0;e<A_ROW;e++){
    for(int f=0;f<B_CLO;f++){
      res_chuan[e][f]=0;
    }
  }
  //记时
  clock_t start,end;
  double c_time;
  double start_time,end_time;
  double m_time;
  //chuanxing
  printf("start cacluate use 1 process!\n");
  start=clock();
  for(int ii =0;ii<A_ROW;ii++){
    for(int jj=0;jj<B_CLO;jj++){
      for(int kk=0;kk<A_CLO_B_ROW;kk++)
      {
        res_chuan[ii][jj]+=mat1[ii][kk]*mat2[kk][jj];
      }
    }
  }
  end=clock();
  c_time=(double)(end-start)/CLOCKS_PER_SEC;
  printf("res[0][0] is %d , spent %f s\n",res_chuan[0][0],c_time);
  printf("start multiple process !\n");
  //并行
  int l,m,n,send_row;
  int task_num;
  int rank,size;
  MPI_Status status;
  MPI_Request request;
  int vec[B_CLO];
  int vect[A_CLO_B_ROW];
  MPI_Init(NULL,NULL);
  start_time=MPI_Wtime();
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  task_num=(A_ROW+size-2)/(size-1);
  for(l=0;l<task_num;l++)//发送接收task_num轮
  {
    if(rank==0)
    {
      send_row=l*(size-1);
      for(m=0;m<size-1;m++)//发送给剩下size-1个进程
      {
        send_row=m+l*(size-1);
        if(send_row<A_ROW)//行号小于矩阵1的行数
        {

          for(int o=0;o<A_CLO_B_ROW;o++){
            vect[o]=mat1[send_row][o];//该行数据放入缓冲区vect中
          }
          MPI_Send(vect,A_CLO_B_ROW,MPI_INT,m+1,send_row,MPI_COMM_WORLD);//发送给对应的进程
          //printf("master has send %d row to process %d \n",send_row,m+1);
        }//本轮发送结束
      }
      //开始本轮接收
      for(int rec=0;rec<size-1;rec++){
        MPI_Recv(vec,B_CLO,MPI_INT,MPI_ANY_SOURCE,rec+l*(size-1),MPI_COMM_WORLD,&status);
        //printf("master has receive res %d row from %d process\n",rec+l*(size-1),status.MPI_SOURCE);
		      for(int s=0;s<B_CLO;s++)
          {
        		res[status.MPI_TAG][s]=vec[s];
		      }
        if(status.MPI_TAG==A_ROW-1){
          end_time=MPI_Wtime();
          m_time=end_time-start_time;
          printf("%d !\n",res[0][0]);
          printf("time:%f s \n",m_time);
          printf("speed_up= %f for %d process!\n",c_time/m_time,size);
          MPI_Abort(MPI_COMM_WORLD,99);
        }
      }
    }
  else//其余进程
  {
      MPI_Recv(vect,A_CLO_B_ROW,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);//接收自己的数据
      if(status.MPI_TAG==SHUT_DOWN_TAG){
        MPI_Barrier(MPI_COMM_WORLD);
        //printf("process exit!",rank);
      }
      else{
      //printf("process %d has recieve %d row!\n",rank,status.MPI_TAG);
      for(int p=0;p<B_CLO;p++){
        int elem=0;
        for(int r=0;r<A_CLO_B_ROW;r++){
          elem+=vect[r]*mat2[r][p];
        }
        vec[p]=elem;
      }//横向量写入到vec
      MPI_Send(vec,B_CLO,MPI_INT,0,rank+l*(size-1)-1,MPI_COMM_WORLD);//发送
    //  printf("process has send %d\n",rank+l*(size-1)-1);
  }
}
}
  MPI_Finalize();
  printf("%d",res[0][0]);
}
