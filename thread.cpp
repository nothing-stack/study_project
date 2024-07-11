#include <iostream>
#include <thread>
#include <windows.h>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <semaphore.h>
//多线程中互斥锁与条件变量一起使用，保证同一时刻只有一个线程可以访问共享资源，来避免访问共享资源产生的竞态问题
//wait,wait_for都是阻塞性，前者一直等待条件满足，后者等待一定时间。
//系统资源注意点：创建线程，操作系统便会为其分配一个栈空间，如果线程数量越多，所需的栈空间就越大，虚拟内存占用就越多。

/*使用注意点：
1.正确初始化条件变量。
2.在持有互斥锁的情况下才能调用条件变量的等待函数，避免竞态情况。
3.条件变量的唤醒操作应当在解锁互斥锁后进行，避免竞态情况。
4.条件变量的等待应该总是在循环中进行，因为它会释放互斥锁并进入休眠状态，如果在等待前条件已经满足，可能会导致资源浪费。
5.线程唤醒后应该再次检查条件是否真的满足，避免其他线程影响导致虚假唤醒的情况。*/

using namespace std;
std::mutex mtx;
std::condition_variable cv;
//消费队列
std::queue<int> products;
//处理消息队列
std::queue<int> Messages;

void producerThread(int id)
{
    //std::lock_guard<std::mutex> lock(mtx); //利用代码作用域的析构函数，防止死锁问题。
    while (true)
    {
        for(int i=0; i < 5; i++ )
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock,[]{return products.size()<5;});
            products.push(i);
            cout<<"void producerThread() ->"<<this_thread::get_id() <<":"<< i<<endl;
            lock.unlock();
            cv.notify_all();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void consumerThread(int id)
{
    //std::lock_guard<std::mutex> lock(mtx);
    while(true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if(products.size()== 5)
        {
            cout<<"product queue is full!"<<endl;
        }

        if(cv.wait_for(lock,std::chrono::seconds(1),[]{return !products.empty();}))
        {
            int product = products.front();
            products.pop();//进行消费
            cout<<"void consumerThread() ->"<<this_thread::get_id()<<":"<<product<<endl;
            lock.unlock();
            cv.notify_all();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


//消息队列进行数据传递
bool stop  = false;

void sendThread1()
{
    for(int i = 0; i < 10; i++)
    {
        std::unique_lock<std::mutex> lock(mtx);
        Messages.push(i);
        cout<<"send date :"<<i<<endl;
        cv.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    {
        std::lock_guard<std::mutex> lock(mtx); //防止干扰
        stop = true;
    }

    //消息发送完，唤醒所有等待接受者
    cv.notify_all();
    
}

void recvThread2()
{
    while(1){
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock,[]{return !Messages.empty() || stop;});
        if(stop && Messages.empty()){
            break; //停止处理
        }

        int msg = Messages.front();
        Messages.pop();
        lock.unlock();
        cout<<"received msg:"<< msg <<endl;
    }
}

/*
线程间的通信方式：
1.利用共享内存实现通信，利用线程锁处理同步问题
2.消息队列和管道
3.简单交互可使用条件变量
4.信号量

注意：
线程同步：确保线程之间能有序访问共享资源，防止数据竞争和确保数据一致性。
线程通信：线程间相互交换信息和数据，来协调各自行为。
*/


//信号量简单使用和理解
int shared_resource = 0;
sem_t semaphore;

void increase_sem_thread()
{
    while(1)
    {
        sem_wait(&semaphore);//获取信号量
        shared_resource++;
        cout<<"the increase resource:"<<shared_resource<<endl;
        sem_post(&semaphore);//释放信号量
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void reduce_sem_thread()
{
    while(1)
    {
        if(shared_resource)
        {
            sem_wait(&semaphore);
            shared_resource--;
            cout<<"the reduce resource:"<<shared_resource<<endl;
            sem_post(&semaphore);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int argc,char *argv[])
{
    if(argc > 1)
    {
        int rangeMax = std::stoi(argv[1]);
    }
 
    std::srand(std::time(0));

    /*std::thread producer(producerThread,1);
    std::thread consumer(consumerThread,1);

    producer.detach();
    consumer.detach();*/
  
    /*std::thread sendThread(sendThread1);
    std::thread recvThread(recvThread2);

    sendThread.detach();
    recvThread.detach();*/

    std::thread producer(increase_sem_thread);
    std::thread consumer(reduce_sem_thread);

    producer.join();
    consumer.join();
    //producer.detach();
    //consumer.detach();

    while(1);

    return 0;
}
