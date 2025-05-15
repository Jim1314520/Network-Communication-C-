#include <iostream>
#include <string>
#include <pthread.h>
struct test
{
  int num;
  int age;
};

void* callback(void*  arg){
  for(int i = 0; i < 5; i++){
    std::cout << "子线程： i =  " << i << std::endl; 
  }
  std::cout << "子线程： " << pthread_self() << std::endl;
  struct test *t = (struct test*) arg;
  t->age  = 18;
  t->num = 23;
  pthread_exit(&t);
  return nullptr;
}
int main(){
  struct test t;
  pthread_t tid;
  pthread_create(&tid, nullptr, callback, &t);
  // for(int i = 0; i < 5; i++){
  //   std::cout << "主线程： i =  " << i << std::endl; 
  // }
  std::cout << "主线程： " << pthread_self() << std::endl;
  // pthread_exit(nullptr);
   void* ptr;

   pthread_join(tid, &ptr);  //  第二个参数 二级指针指向接收子线程传出的数据
  // struct test* pt = (struct test*) ptr;
   std::cout << t.age << " " << t.num << std::endl;
  return 0;
}