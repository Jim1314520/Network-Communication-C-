#include "threadpool.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
void taskFunc(void* arg){
  int num = *(int*)arg;
  printf("thread %lu is working, number = %d\n",
  pthread_self(), num); // 执行任务
  sleep(1);
  delete (int*) arg;
}


int main(){
  // 创建线程池
  ThreadPool* pool = threadPoolCreate(3, 8, 100);
  
  for(int i = 0; i < 100; i++){
    int* num = new int;
    *num = i + 100;
    threadPoolAdd(pool, taskFunc, num);
  }
 
    sleep(30);
 

  ThreadPoolDestroy(pool);
}