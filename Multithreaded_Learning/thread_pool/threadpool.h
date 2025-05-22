#ifndef _THREADPOOL_H // 防止同一头文件被多次包含导致的重复定义错误
#define _THREADPOOL_H

// 创建初始化的线程池
struct ThreadPool* threadPoolCreate(int min, int max, int queueSize);
// 销毁线程池
int ThreadPoolDestroy(ThreadPool * pool);
// 给线程池添加任务
void threadPoolAdd(ThreadPool* pool, void(*func)(void*), void* arg);
// 获取线程池工作中的个数
int threadPoolBusyNum(ThreadPool* pool);
// 获取线程池中存活的个数
int threadPoolAliveNum(ThreadPool* pool);
// 工作线程执行的函数
void* worker(void* arg);
// 工作线程执行的函数
void* manager(void* arg);
void threadExit(ThreadPool* pool);
#endif