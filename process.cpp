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

using namespace std;

int main()
{
    return 0;
}

//