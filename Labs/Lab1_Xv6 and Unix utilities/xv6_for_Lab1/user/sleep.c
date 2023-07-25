#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    if(argc < 2 || argc > 2){
        fprintf(2, "Usage like \"sleep clockTime\"\n"); // 实验要求参数为一个整数
        exit(1); // 异常退出
    }

    int times = atoi(argv[1]); // 调用系统调用sleep参数为int类型
    sleep(times);
    exit(0);
}