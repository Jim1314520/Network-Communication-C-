#include <iostream>
#include <unistd.h>
#include<pthread.h>
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
    pthread_mutex_lock(&mutex);
    node* newnode = new node;
    newnode->number = rand() % 1000;
    newnode->next = head;
    head = newnode;
    std::cout <<"生产者: " << pthread_self() << "number: " << newnode->number << std::endl;
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond); // 唤醒消费者
    sleep(rand() % 3);
  }
}
// 消费者
void* consumer(void* arg){
  while(1){
    pthread_mutex_lock(&mutex);
    while(head == nullptr) {
      //阻塞线程
      pthread_cond_wait(&cond, &mutex); // 唤醒后直接从这里开始执行程序，这里使用if不行
    }
    node* newnode = head;
    std::cout <<"消费者： " << pthread_self() << "number : " << newnode->number << std::endl;
    head = head->next;
    delete newnode;
    pthread_mutex_unlock(&mutex);
    sleep(rand() % 3);
  }
}

int main(){
  pthread_mutex_init(& mutex, nullptr);
  pthread_cond_init(&cond, nullptr);
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
  pthread_cond_destroy(&cond);

}