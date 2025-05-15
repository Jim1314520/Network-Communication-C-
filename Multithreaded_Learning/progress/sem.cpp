#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include<pthread.h>
// 生产者的信号量
sem_t semp;
// 消费者的信号量
sem_t semc;

pthread_cond_t cond;
pthread_mutex_t mutex;
//链表的节点类型
struct node{
  int number;
  struct node* next;
};
//头节点
struct node* head = nullptr;
//生产者
void* producer(void* arg){
  while(1){
    sem_wait(&semp);
    pthread_mutex_lock(&mutex);
    node* newnode = new node;
    newnode->number = rand() % 1000;
    newnode->next = head;
    head = newnode;
    std::cout <<"生产者: " << pthread_self() << "number: " << newnode->number << std::endl;
    pthread_mutex_unlock(&mutex);
    sem_post(&semc);
    sleep(rand() % 3);
  }
}
// 消费者
void* consumer(void* arg){
  while(1){
    sem_wait(&semc);
    pthread_mutex_lock(&mutex); // 注意加锁位置
    node* newnode = head;
    std::cout <<"消费者： " << pthread_self() << "number : " << newnode->number << std::endl;
    head = head->next;
    delete newnode;
    pthread_mutex_unlock(&mutex);
    sem_post(&semp);
    sleep(rand() % 3);
  }
}

int main(){
  sem_init(&semp, 0, 5);// 初始化生产者
  sem_init(&semc, 0, 0);

  pthread_mutex_init(& mutex, nullptr);

  
  pthread_t t1[5], t2[5];
  for(int i = 0; i < 5; i++){
    pthread_create(&t1[i], nullptr, producer, nullptr);

  }
  for(int i = 0; i < 5; i++){ 
    pthread_create(&t2[i], nullptr, consumer, nullptr);
  }

  for(int i = 0; i < 5; i++){
    pthread_join(t1[i], nullptr);
    pthread_join(t2[i], nullptr);
  }

  pthread_mutex_destroy(&mutex);
  sem_destroy(&semp);
  sem_destroy(&semc);

}