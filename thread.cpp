#include <iostream>
#include <thread>
#include <windows.h>
#include <mutex>
#include <condition_variable>
#include <queue>
//线程与条件变量使用，避免访问共享资源竞争问题

using namespace std;
std::mutex mtx;
std::condition_variable cv;
std::queue<int> products;

void producerThread(int id)
{
    //std::lock_guard<std::mutex> lock(mtx); //利用代码作用域的析构函数，防止死锁问题。
    for(int i=0; i < 5; i++ )
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock,[]{return products.size()<5;});
        products.push(i);
        cout<<"void producerThread() ->"<< id <<":"<< i<<endl;
        lock.unlock();
        cv.notify_all();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void consumerThread(int id)
{
    //std::lock_guard<std::mutex> lock(mtx);
    while(true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if(cv.wait_for(lock,std::chrono::seconds(1),[]{return !products.empty();}))
        {
            int product = products.front();
            products.pop();
            cout<<"void consumerThread() ->"<< id <<":"<< product<<endl;
            lock.unlock();
            cv.notify_all();
        }
        else
        {
            break;
        }
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

    std::thread Thread1(testThread1);
    std::thread Thread2(testThread2);
    
    Thread1.join();
    Thread2.join();

    /*std::thread producer(producerThread,1),producerx(producerThread,2);
    std::thread consumer(consumerThread,1),consumerx(consumerThread,2);

    producer.join();
    producerx.join();
    consumer.join();
    consumerx.join();*/
    return 0;
}
