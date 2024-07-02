#include <iostream>
#include <thread>
#include <windows.h>
#include <mutex>
#include <condition_variable>
#include <queue>
//多线程中互斥锁与条件变量一起使用，保证同一时刻只有一个线程可以访问共享资源，来避免访问共享资源产生的竞态问题
//wait wait_for都是阻塞性，前者一直等待条件满足，后者等待一定时间。
/*使用注意点：
1.正确初始化条件变量。
2.在持有互斥锁的情况下才能调用条件变量的等待函数，避免竞态情况。
3.条件变量的唤醒操作应当在解锁互斥锁后进行，避免竞态情况。
4.条件变量的等待应该总是在循环中进行，因为它会释放互斥锁并进入休眠状态，如果在等待前条件已经满足，可能会导致资源浪费。
5.线程唤醒后应该再次检查条件是否真的满足，避免其他线程影响导致虚假唤醒的情况。*/

using namespace std;
std::mutex mtx;
std::condition_variable cv;
std::queue<int> products;

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
            products.pop();
            cout<<"void consumerThread() ->"<<this_thread::get_id()<<":"<<product<<endl;
            lock.unlock();
            cv.notify_all();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void testThread1()
{
    std::unique_lock<std::mutex> lock(mtx);
    for(int i = 0;i < 10;i++)
    {
        Sleep(1000);
        cout<<"testThread1 :"<<i<<endl;
    }
}

void testThread2()
{
    std::unique_lock<std::mutex> lock(mtx);
    for(int i = 0;i < 10;i++)
    {
        Sleep(1000);
        cout<<"testThread2 :"<<i<<endl;
    }
}

int main(int argc,char *argv[])
{
    if(argc > 1)
    {
        int rangeMax = std::stoi(argv[1]);
    }
 
    std::srand(std::time(0));

    /*std::thread Thread1(testThread1);
    std::thread Thread2(testThread2);
    
    Thread1.join();
    Thread2.join();*/

    std::thread producer(producerThread,1),producerx(producerThread,2);
    std::thread consumer(consumerThread,1),consumerx(consumerThread,2);

    producer.detach();
    producerx.detach();
    consumer.detach();
    consumerx.detach();

    while(1);
    return 0;
}
