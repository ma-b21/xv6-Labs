#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char* argv[])
{
    int fToc[2], cTof[2]; // 分别建立父进程到子进程,子进程到父进程的管道.
    pipe(fToc);
    pipe(cTof);
    int pid = fork(); // 建立子进程
    if(pid < 0)
    {
        fprintf(2, "error in fork !"); // 若进程建立失败会返回负值
    }
    else if(pid > 0) // 父进程中fork返回值为正
    {
        close(fToc[0]); // 关闭子进程读端
        close(cTof[1]); // 关闭子进程写端
        write(fToc[1], "ping", 4); // 向子进程发送1位数据
        close(fToc[1]); // 关闭父进程写端
        char* readBuf = (char*)malloc(sizeof(char) * 4); // 申请读入数据流空间
        int fatherPID = getpid(); // 获取父进程PID
        read(cTof[0], readBuf, 4); // 从子进程中接收数据（阻塞式）
        close(cTof[0]); //关闭父进程读端
        fprintf(2, "%d: received %s\n", fatherPID, readBuf); // 按实验要求格式输出
        free(readBuf); // 释放读入数据流空间
    }
    else // 子进程中fork返回值为0
    {
        close(fToc[1]); // 关闭父进程写端
        close(cTof[0]); // 关闭父进程读端
        char* readBuf = (char*)malloc(sizeof(char) * 4); // 申请读入数据流空间
        int childPID = getpid(); // 获取子进程PID
        read(fToc[0], readBuf, 4); // 从父进程中接收数据（阻塞式）
        close(fToc[0]); // 关闭子进程读端
        fprintf(2, "%d: received %s\n", childPID, readBuf); // 按实验要求格式输出
        write(cTof[1], "pong", 4); // 向父进程发送1位数据
        close(cTof[1]); // 关闭子进程写端
        free(readBuf); // 释放读入数据流空间
    }
    exit(0);
}