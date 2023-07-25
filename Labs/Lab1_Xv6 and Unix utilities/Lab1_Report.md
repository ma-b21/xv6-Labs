# Lab 1
## sleep
### 主要思路
- 检查参数个数
  - 通过对argc进行判断，sleep操作argc的值应为2.
- 系统调用sleep()
  - 函数原型为sleep(int),因此要先将argv[1]参数转换为int类型再进行系统调用sleep().
- 代码如下：
  ```c
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
  ```

## pingpong
### 主要思路
- 建立管道
  - 因为管道通信为单向，因此需要两条管道.
- 建立子进程
  - 注意用fork函数建立子进程后在子进程中其返回值为0，父进程中其返回值为子进程PID，若建立失败返回值为负.
- 读写操作
  - 注意write函数为非阻塞式，而read函数为阻塞式
- 题目相关
  - 本题中评分时输出格式要求与样例输出一致，而非题目描述，即"pid:"后面有一空格.
- 代码如下：
  ```c
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
  ```
### 参考链接
- [进程间通信，管道pipe函数详解](https://www.cnblogs.com/robbychan/p/3786886.html#:~:text=%E5%87%BD%E6%95%B0%20pipe%E6%88%91%E4%BB%AC%E7%94%A8%E4%B8%AD%E6%96%87%E5%8F%AB%E5%81%9A%E7%AE%A1%E9%81%93%E3%80%82%20%E4%BB%A5%E4%B8%8B%E8%AE%B2%E8%A7%A3%E5%9D%87%E6%98%AF%E5%9F%BA%E4%BA%8ELinux%E4%B8%BA%E7%8E%AF%E5%A2%83%EF%BC%9A%E7%AE%80%E4%BB%8B%20%E6%89%80%E9%9C%80%E5%A4%B4%E6%96%87%E4%BB%B6%20%23include%20%E5%87%BD%E6%95%B0%E5%8E%9F%E5%9E%8B%20int%20pipe%28int,%E5%87%BD%E6%95%B0%E4%BC%A0%E5%85%A5%E5%80%BC%20fd%3A%E7%AE%A1%E9%81%93%E7%9A%84%E4%B8%A4%E4%B8%AA%E6%96%87%E4%BB%B6%E6%8F%8F%E8%BF%B0%E7%AC%A6%EF%BC%8C%E4%B9%8B%E5%90%8E%E5%B0%B1%E6%98%AF%E5%8F%AF%E4%BB%A5%E7%9B%B4%E6%8E%A5%E6%93%8D%E4%BD%9C%E8%BF%99%E4%B8%A4%E4%B8%AA%E6%96%87%E4%BB%B6%E6%8F%8F%E8%BF%B0%E7%AC%A6%20%E8%BF%94%E5%9B%9E%E5%80%BC%20%E6%88%90%E5%8A%9F%200%20%E5%A4%B1%E8%B4%A5%20-1%E4%BB%80%E4%B9%88%E6%98%AF%E7%AE%A1%E9%81%93%20%E7%AE%A1%E9%81%93)
- [Linux中fork函数详解](https://blog.csdn.net/kxjrzyk/article/details/81603049)

## primes
### 主要实现思路
- 主函数
  - 主函数先建立一个与子进程通信的管道，将2-35的数据都写入管道中，接着建立子进程，子进程递归调用函数，该递归函数负责筛出质数.
- 递归函数
  - 声明：`void primes(int fToc[2])`,其中`fToc[2]`是与父进程间的通信管道。
  - 功能：负责读取父进程筛选后剩下的数字，函数体内首先读取一个整数，若读取成功则该数为本次递归基质数，若失败则说明筛选结束。 读取成功后继续对管道进行读取，筛选出不是基质数倍数的数字，创建新管道进行写入，然后建立子进程继续递归执行此函数，此时父进程等待子进程结束。
- 代码如下：
  ```c
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
  ```
### 参考链接
- [父进程等待子进程结束 waitpid wait](https://blog.csdn.net/yangbodong22011/article/details/49741027)

## find
### 主要思路
- 检查参数
  - 参数个数不为3个时输出错误
- `find`
  - 当前`path`为文件时直接对比文件名与目标文件名
  - 当前`path`为目录时遍历其子文件，递归进行搜索
### 实现难点
- 获取文件信息
  - `fstat(int, struct stat*)`,`int`为文件句柄，`struct stat`为存文件信息的结构
- 遍历子目录
  - 利用`path + "/" + 子文件名`得到新`path`进行递归
  - 子文件名通过`read(fd, &de, sizeof(de))`得到`de.name`,其中`fd`为文件句柄，`de`为建立的目录结构
  - 注意在遍历时要跳过`.,..`以及无效目录
- 代码如下：
  ```c
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
  ```
### 参考链接
- [MIT 6.S081课程Lab Util Find全网最详细思路分享/个人学习笔记](https://zhuanlan.zhihu.com/p/508045883)

## xargs
### 主要思路
- 检查参数
  - 若参数个数小于2则异常退出
- 建立新字符串数组作为exec执行参数
  - 新字符串数组`nargv`,其初值等于`argv`数组的`1`-`（argc - 1）`项
  - 从标准输入流中读取数据
  - 将上述数据通过`\n`分开，存入字符串数组中
  - 遍历字符串数组，分别将其当作`nargv`中的下一个元素，将`nargv`的末元素置为0，建立子进程以`nargv`作为参数执行`exec`函数
- 代码如下：
  ```c
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
  ```
### 参考链接
- [Linux grep 命令](https://www.runoob.com/linux/linux-comm-grep.html)
- [xargs原理及用法详解](https://www.cnblogs.com/liujunjun/p/11977906.html)