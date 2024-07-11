//实现简单线程池操作
/* 
1.创建一定数量线程池
2.终止所有线程并回收资源
3.添加任务到消息队列
*/
#include <iostream>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <windows.h>

using namespace std;
#define MAX_THREAD 20

class ThreadPool
{
public:
    enum taskPriorityE{LOW,MID,HIGH};
    typedef std::function<void()> TaskType;
    typedef std::pair<taskPriorityE,TaskType> TaskPair;

    void AddTask(const TaskType&);      //添加任务
    void AddTask(const TaskPair&);      //添加优先级任务
    int  GetTaskNum();
    int  GetThreadPoolNum(){return m_threads_size;};
    ThreadPool(int threads = 10);
    ~ThreadPool();

private:
    struct TaskPriorityCmp{
        bool operator()(const TaskPair& a,const TaskPair&b)const{
            return a.first > b.first;
        }
    };

    std::vector<std::thread* > m_threads;       //线程池
    std::priority_queue<TaskPair,std::vector<TaskPair>,TaskPriorityCmp> m_tasks; 

    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_stop; 
    int m_threads_size;

    ThreadPool(const ThreadPool&);
    const ThreadPool& operator=(const ThreadPool&);
    void Start();
    void Stop();
    void ThreadTaskLoop();
    TaskType GetTask();
};

ThreadPool::ThreadPool(int threads):m_threads_size(threads),m_mutex(),m_condition(),m_stop(true)
{
    Start();
}

void ThreadPool::Start()
{
    assert(m_threads.empty());
    assert(m_stop);
    m_stop = false;
    if(m_threads_size > MAX_THREAD)
    {
        m_threads_size = MAX_THREAD;
    }

    m_threads.reserve(m_threads_size);
    for(int i = 0; i < m_threads_size;i++)
    {
        m_threads.push_back(new std::thread(std::bind(&ThreadPool::ThreadTaskLoop,this)));
    }
}

ThreadPool::~ThreadPool()
{
    Stop();
}

void ThreadPool::Stop()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_stop = true;
    m_condition.notify_all();
    for(auto it = m_threads.begin();it != m_threads.end();it++)
    {   
        (*it)->join();
        delete *it;
    }
    m_threads.clear();
}

//线程任务队列获取相应任务
void ThreadPool::ThreadTaskLoop()
{
    while(m_stop == false)
    {
        TaskType task = GetTask();
        if(task)
        {
            task();     //执行这个任务
        }
    }
    
}

//从任务队列和获取任务
ThreadPool::TaskType ThreadPool::GetTask()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_tasks.empty() && !m_stop)
    {
        m_condition.wait(lock);
    }

    TaskType task;
    int size = m_tasks.size();

    if(!m_tasks.empty()&&!m_stop)
    {
        task = m_tasks.top().second;
        m_tasks.pop();
    }
    return task;
}

void ThreadPool::AddTask(const TaskType& task)     //添加任务队列
{
    std::unique_lock<std::mutex> lock(m_mutex);
    TaskPair taskpair(MID,task);
    m_tasks.emplace(taskpair);
    m_condition.notify_one();
}

void ThreadPool::AddTask(const TaskPair& taskpair)  //添加优先级任务
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_tasks.emplace(taskpair);
    m_condition.notify_one();
}

int ThreadPool::GetTaskNum()
{
    return m_tasks.size();
}

void testfunction()
{
    while (1)
    {
        std::cout<<"test thread pid:"<<std::this_thread::get_id()<<endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    ThreadPool mtp(8);
    for(int i =0;i < 5;i++)
    {
        mtp.AddTask(testfunction);
    }

    while (1){
    };
    
    return 0;
}