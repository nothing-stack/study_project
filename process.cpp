//踪迹进程知识点
//1.创建和结束进程
//2.进程间通信方式

/*用户空间内存段*/
/*栈段：局部变量和函数调用上下文。默认为8MB，当然系统也提供了参数，可自定义【 ulimit -a 】
*待分配区域
*堆段：malloc(),mmap()分配与释放
*BSS段：未初始化的静态变量和全局变量
*数据段：已初始化的静态常量和全局变量
*代码段：二进制可执行文件
*保留段：不可访问地址。（NULL）
*/
#include <iostream>
#include <windows.h>

using namespace std;

int main()
{
    cout<<"the test fork process start"<<endl;
    
    STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
 
    // 这里的"C:\\path\\to\\program.exe"是新进程要运行的程序路径
    TCHAR lpCommandLine[] = TEXT("test.exe");
    int ret = CreateProcess(NULL,lpCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (ret > 0 ) {
        while(1)
        {
            std::cout << "子进程test" << std::endl;
        }

        WaitForSingleObject(pi.hProcess, INFINITE); //等待进程退出，回收资源
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        std::cerr << "创建进程失败." << ret << std::endl;
    }

    return 0;
}