#ifndef _THREADPOOL_H // 这是什么意思
#define _THREADPOOL_H

// 创建初始化的线程池
struct ThreadPool* threadPoolCreate(int min, int max, int queueSize);


// 销毁线程池

// 给线程池添加任务

// 获取线程池工作中的个数

// 获取线程池中存活的个数

void* worker(void* arg);
void* manager(void* arg);

#endif