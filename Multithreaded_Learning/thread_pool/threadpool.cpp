#include "threadpool.h"
#include <pthread.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
const int NUMBER = 2;
// 任务结构体
struct Task{
  void(*function) (void* arg); // 任务函数指针
  void* arg; // 任务参数
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
  int liveNum; // 存活的线程个数
  int exitNum; // 需要销毁的线程数
  pthread_mutex_t mutexPool; // 锁整个线程池
  pthread_mutex_t mutexBusy; // 锁busyNum变量
  pthread_cond_t notFull; //  当队列满时阻塞提交者
  pthread_cond_t notEmpty; // 当队列空时阻塞工作线程

  int shutdown; // 是否需要销毁线程池， 销毁为1 ，不销毁为0

};

ThreadPool* threadPoolCreate(int min, int max, int queueSize){
  ThreadPool * pool = new ThreadPool ;
  do{
  if(pool == nullptr){
    std::cout << "new threadpool fail ...." << std::endl;
    break; 
  }
  pool->threadIds = new pthread_t[max] ;
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
    pthread_cond_init(&pool->notFull, nullptr) != 0) {
    std::cout << "mutex or cond init fail ...." << std::endl;
    break; 
  }
  // 任务队列
  pool->taskQ = new Task[queueSize];
  if (pool->taskQ == nullptr) { // 必须添加这个检查
    std::cout << "new taskQ fail ...." << std::endl;
    break;
}
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
  if(pool->threadIds) delete [] pool->threadIds;
  if(pool->taskQ) delete [] pool->taskQ;
  if(pool) delete pool;
  return nullptr;
}

void* worker(void* arg){
  ThreadPool *pool = (ThreadPool*) arg;
  while(1){
    pthread_mutex_lock(&pool->mutexPool);
    // 当前队列是否为空
    while(pool->queueSize == 0 && !pool->shutdown){
      // 阻塞工作线程
      pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);
      // 判断是不是要销毁线程
      if(pool->exitNum > 0){
        pool->exitNum --;
        if(pool->liveNum > pool->minNum){
          pool->liveNum --;
          pthread_mutex_unlock(&pool->mutexPool);
          threadExit(pool);
        }
        
      }

    }
    // 判断线程池是否关闭
    if(pool->shutdown){
      pthread_mutex_unlock(&pool->mutexPool);
      threadExit(pool);
    }
    //从任务队列中取出一个任务
    Task task;
    task.function = pool->taskQ[pool->queueFront].function;
    task.arg = pool->taskQ[pool->queueFront].arg;
    //移动头节点
    pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
    pool->queueSize--;
    // 解锁
    pthread_cond_signal(&pool->notFull);
    pthread_mutex_unlock(&pool->mutexPool);

    std::cout << "thread start working...." << pthread_self() << std::endl;
    pthread_mutex_lock(&pool->mutexBusy);
    pool->busyNum++;
    pthread_mutex_unlock(&pool->mutexBusy);
    task.function(task.arg);
    
    task.arg = nullptr;
    std::cout << "thread end working...." << pthread_self() << std::endl;
    pthread_mutex_lock(&pool->mutexBusy);
    pool->busyNum--;
    pthread_mutex_unlock(&pool->mutexBusy);
    }
    return nullptr;
  }
void* manager(void* arg){
  ThreadPool* pool = (ThreadPool*) arg;
  while(!pool->shutdown){
    // 每个3s检查一次
    sleep(3);
    // 取出线程池中的任务数量和当前线程的数量
    pthread_mutex_lock(&pool->mutexPool);
    int queueSize = pool->queueSize;
    int liveNum = pool->liveNum;
    pthread_mutex_unlock(&pool->mutexPool);
    // 取出忙的线程数量
    pthread_mutex_lock(&pool->mutexBusy);
    int busyNum = pool->busyNum;
    pthread_mutex_unlock(&pool->mutexBusy);
    //添加线程

    //任务的个数> 存活的线程个数 && 存活的线程数 < 最大线程数
    if(queueSize > liveNum && liveNum < pool->maxNum){
      pthread_mutex_lock(&pool->mutexPool);
      int counter = 0;
      for(int i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; i++){
        if (pthread_equal(pool->threadIds[i], (pthread_t)0)) {          // 槽位空闲
          if (pthread_create(&pool->threadIds[i], nullptr, worker, pool) == 0) {
              counter++;
              pool->liveNum++;
          }
        }

      }
      pthread_mutex_unlock(&pool->mutexPool);
    }
    // 销毁线程

    // 忙的线程*2 < 存活的线程数 && 存活的线程> 最小线程数
    if(busyNum * 2 < liveNum && liveNum > pool->minNum){
      pthread_mutex_lock(&pool->mutexPool);
      pool->exitNum = NUMBER;
      pthread_mutex_unlock(&pool->mutexPool);
      // 让工作的线程自杀
      for(int i = 0; i < NUMBER; i++){
        pthread_cond_signal(&pool->notEmpty);
      }
    }
  }
  return nullptr;
}

void threadExit(ThreadPool* pool){
  pthread_t tid = pthread_self();
  for(int i = 0; i < pool->maxNum; i++){
    if(pthread_equal(pool->threadIds[i], tid)){
      pool->threadIds[i] = 0;
      std::cout << "threadExit() called " << tid << std::endl;
      break;
    }
  }
  pthread_exit(nullptr);
}

void threadPoolAdd(ThreadPool* pool, void(*func)(void*), void* arg){
  pthread_mutex_lock(&pool->mutexPool);
  while(pool->queueSize == pool->queueCapacity && !pool->shutdown){
    //阻塞生产者线程
    pthread_cond_wait(&pool->notFull, &pool->mutexPool);
  }
  if(pool->shutdown){
    pthread_mutex_unlock(&pool->mutexPool);
    return;
  }
  // 添加任务
  pool->taskQ[pool->queueRear].function = func;
  pool->taskQ[pool->queueRear].arg = arg;
  pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
  pool->queueSize++;
  pthread_cond_signal(&pool->notEmpty);
  pthread_mutex_unlock(&pool->mutexPool);
}

int threadPoolBusyNum(ThreadPool* pool){
  pthread_mutex_lock(&pool->mutexBusy);
  int busyNum = pool->busyNum;
  pthread_mutex_unlock(&pool->mutexBusy);
  return busyNum;

}
int threadPoolAliveNum(ThreadPool* pool){
  pthread_mutex_lock(&pool->mutexPool);
  int liveNum = pool->liveNum;
  pthread_mutex_unlock(&pool->mutexPool);
  return liveNum;
}
int ThreadPoolDestroy(ThreadPool * pool){
  if(pool == nullptr){
    return -1;
  }
  // 关闭线程池
  pool->shutdown = 1;
  //阻塞回收管理者线程
  pthread_join(pool->managerId, nullptr);
  // 唤醒阻塞消费者线程
  for(int i = 0; i < pool->liveNum; i++){
    pthread_cond_signal(&pool->notEmpty);
  }
 for(int i = 0; i < pool->maxNum; i++) {
    if (!pthread_equal(pool->threadIds[i], (pthread_t)0)) {
      pthread_join(pool->threadIds[i], nullptr);
    }
  }
  
  //释放堆内存
  if(pool->taskQ){
    delete [] pool->taskQ;
  }
  if(pool->threadIds){
    delete [] pool->threadIds;
  }

  pthread_mutex_destroy(&pool->mutexBusy);
  pthread_mutex_destroy(&pool->mutexPool);
  pthread_cond_destroy(&pool->notEmpty);
  pthread_cond_destroy(&pool->notFull);
  delete pool;
  pool = nullptr;
  return 0;

}