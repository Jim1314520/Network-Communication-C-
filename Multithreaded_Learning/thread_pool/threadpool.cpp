#include "threadpool.h"
#include <pthread.h>
#include <iostream>
#include <string.h>
// 任务结构体
struct Task{
  void(*function) (void* arg);
  void* arg;
};
// 线程池结构体
struct ThreadPool{
  // 任务对列
  Task * taskQ;
  int queueCapacity; // 容量
  int queueSize; // 当前任务个数
  int queueFront; // 队头--->取出数据
  int queueRear; // 队尾 --->取出数据

  pthread_t managerId; // 管理者线程
  pthread_t *threadIds; // 工作的线程
  int minNum; // 最小线程数量
  int maxNum; // 最大线程数量
  int busyNum; // 忙的线程个数
  int liveNum; // 存货的线程个数
  int exitNum; // 需要销毁的线程数
  pthread_mutex_t mutexPool; // 锁整个线程池
  pthread_mutex_t mutexBusy; // 锁busyNum变量
  pthread_cond_t notFull; // 任务对列是否满
  pthread_cond_t notEmpty; // 任务队列是否空

  int shutdown; // 是否需要销毁线程池， 销毁为1 ，不销毁为0

};

ThreadPool* threadPoolCreate(int min, int max, int queueSize){
  ThreadPool * pool = new ThreadPool ;
  do{
  if(pool == nullptr){
    std::cout << "new threadpool fail ...." << std::endl;
    break; 
  }
  pool->threadIds = new pthread_t ;
  if(pool->threadIds == nullptr) {
    std::cout << "new threadIds fail ...." << std::endl;
    break; 
  }
  memset(pool->threadIds, 0, sizeof(pthread_t) * max);
  pool->minNum = min;
  pool->maxNum = max;
  pool->busyNum = 0;
  pool->liveNum = min; // 和最小个数相同
  pool->exitNum = 0;
  if(pthread_mutex_init(&pool->mutexPool, nullptr) != 0 || 
    pthread_mutex_init(&pool->mutexBusy, nullptr) != 0 ||
    pthread_cond_init(&pool->notEmpty, nullptr) != 0 ||
    pthread_cond_init(&pool->notFull, nullptr)) {
    std::cout << "mutex or cond init fail ...." << std::endl;
    break; 
  }
  // 任务队列
  pool->taskQ = new Task;
  pool->queueCapacity = queueSize;
  pool->queueSize = 0;
  pool->queueFront = 0;
  pool->queueRear = 0;

  pool->shutdown = 0;
  // 创建线程
  pthread_create(&pool->managerId, nullptr, manager, pool);
  for(int i = 0; i < min; i++){
    pthread_create(&pool->threadIds[i], nullptr, worker, pool);
  }

  return pool;
  }while(0);
  // 释放资源
  if(pool->threadIds) delete pool->threadIds;
  if(pool) delete pool;
  if(pool->taskQ) delete pool->taskQ;
  return nullptr;
}

void* woker(void* arg){
  ThreadPool *pool = (ThreadPool*) arg;
  while(1){
    pthread_mutex_lock(&pool->mutexPool);
    // 当前队列是否为空
    while(pool->queueSize == 0 && !pool->shutdown){
      // 阻塞工作线程
      pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);

    }
    // 判断线程池是否关闭
    if(pool->shutdown){
      pthread_mutex_unlock(&pool->mutexPool);
      pthread_exit(nullptr);
    }
    //从任务队列中取出一个任务
    Task task;
    task.function = pool->taskQ[pool->queueFront].function;
    task.arg = pool->taskQ[pool->queueFront].arg;
    //移动头节点
    pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
    pool->queueCapacity--;
    // 解锁
    pthread_mutex_unlock(&pool->mutexPool);
    std::cout << "thread start working...." << std::endl;
    pthread_mutex_lock(&pool->mutexBusy);
    pool->busyNum++;
    pthread_mutex_unlock(&pool->mutexBusy);
    task.function(task.arg);
    delete task.arg;
    task.arg = nullptr;
    std::cout << "thread end working...." << std::endl;
    pthread_mutex_lock(&pool->mutexBusy);
    pool->busyNum--;
    pthread_mutex_unlock(&pool->mutexBusy);
  }
void* manager(void* arg){
  ThreadPool* pool = (ThreadPool*) arg;
  while(!pool->shutdown){
    
  }
}


}