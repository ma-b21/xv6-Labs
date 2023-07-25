#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 用来递归的创建子进程及管道
void primes(int fToc[2])
{
    int count = 0;           // 未被筛的数字的个数
    close(fToc[1]);          // 与父进程管道的写端
    int numbers[34] = { 0 }; // 储存未被筛的数字
    int base;                // 本次递归筛选的基质数
    int readSuccess = 0;     // 数据读取成功标志
    readSuccess = read(fToc[0], &base, sizeof(int)); // 读取基质数
    
    if(readSuccess == 0)  exit(0); // 没有基质数，即管道内为空，子进程退出
    
    fprintf(2, "prime %d\n", base); // 按照题述格式输出本次递归基质数

    // 继续读取数据
    readSuccess = read(fToc[0], &numbers[count], sizeof(int));
    while(readSuccess > 0)
    {
        if(numbers[count] % base != 0) count++;
        readSuccess = read(fToc[0], &numbers[count], sizeof(int));
    }
    close(fToc[0]); // 关闭读端
    
    int nfToc[2]; // 创建与子进程通信的新管道
    pipe(nfToc);

    if(fork() == 0) // 子进程继续递归筛质数
    {
        primes(nfToc);
    }
    else // 父进程关闭管道读端，并写入本次递归未筛选的数字，之后等待子进程退出
    {
        close(nfToc[0]);
        write(nfToc[1], &numbers, count * sizeof(int));
        close(nfToc[1]);
        wait(0);
        exit(0);
    }
}

int main(int argc, char* argv[])
{
    int number[34];  // 存2-35的数字
    for(int i = 0; i < 34; ++i)
    {
        number[i] = i + 2;
    }
    
    
    int fToc[2]; // 建立与子进程间通信管道
    pipe(fToc);


    if(fork() == 0) // 子进程递归筛质数
    {
        primes(fToc);
    }
    else // 父进程关闭管道读端，并写入本次递归未筛选的数字，之后等待子进程退出 
    {
        close(fToc[0]);
        write(fToc[1], &number, 34 * sizeof(int));
        close(fToc[1]);
        wait(0);
        exit(0);
    }
    exit(0);
}