#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 通过路径获取文件名
char* fmtname(char *path)
{
    char *p; 
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    return p;
}

// 在path及其子目录下查找名为fileName的文件
void find(char* path, char*fileName)
{
    char buf[512], *p; // 缓冲区及其指针
    int fd; // 文件句柄
    struct dirent de; // 目录结构
    struct stat st; // 文件信息结构

    if((fd = open(path, 0)) < 0) // 打开path目录文件
    {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0) // 获取文件信息
    {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
    case T_FILE: // 如果是文件则将文件名与目标文件名比较
        if(!strcmp(fmtname(path), fileName)) printf("%s\n", path);
        break;

    case T_DIR: // 如果是目录文件则递归查找目录内的所有文件
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) // 目录长度超过最大限制
        {
            printf("ls: path too long\n");
            break;
        }   
        strcpy(buf, path); // 将路径拷贝到缓冲区
        p = buf+strlen(buf); // 将指针指向缓冲区尾
        *p++ = '/'; // 路径格式
        while(read(fd, &de, sizeof(de)) == sizeof(de)) // 读到子文件
        {
            if(de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, "..")) // 过滤无效目录和.，..两个目录
                continue;
            memmove(p, de.name, DIRSIZ); // 将子文件名拷贝到缓冲区
            p[DIRSIZ] = 0; // 加尾符'\0'
            if(stat(buf, &st) < 0) // 获取子目录信息
            {
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            find(buf, fileName); // 子目录有效递归查找
    }
    break;
  }
  close(fd); // 关闭文件
}


int main(int argc, char* argv[])
{
    if(argc > 3 || argc < 3) // 检查参数个数
    {
        printf("The usage like \"find path file_name\" ...\n");
        exit(1);
    }
    find(argv[1], argv[2]); // 寻找文件
    exit(0);
}