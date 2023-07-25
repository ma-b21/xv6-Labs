#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    if(argc < 2) // 检查参数个数
    {
        fprintf(2, "Need more arguments...");
        exit(1);
    }

    char *nargv[16]; // xargs命令要调用的参数
    int index = 0;
    for(int i = 1; i < argc; ++i)
    {
        nargv[index ++] = argv[i];
    }

    char buf[512]; // 从标准输入流中读取数据
    char* buff = buf; // 用于后续分割buf中的不同行
    char* p = buf; // 用于将输入接到buf的尾部
    int readBuf;
    while((readBuf = read(0, p, 16)) > 0) // 读入标准输出的内容
    {
        p += readBuf;
        *p = '\0';
        if(strlen(buf) > 512)
        {
            fprintf(2, "too many arguments");
            exit(1);
        }
    }

    char* nargvMem[32]; // 第i行的输出为nargvMen[i]
    int nindex = 0;
    for(char* q = buf; q != p; ++q) // 对buf根据不同行进行分割
    {
        if(*q == '\n')
        {
            *q = '\0';
            nargvMem[nindex ++] = buff;
            buff = q + 1;
        }
    }

    for(int i = 0; i < nindex; ++i) // 用nargv进行执行exec()操作，通过更改nargv[index]的值实现不同行输出的执行
    {
        nargv[index ++] = nargvMem[i];
        nargv[index] = 0;
        if(fork() == 0)
        {
            exec(nargv[0], nargv);
            exit(0);
        }
        else
        {
            index--;
            wait(0);
        }
    }

    exit(0);
}