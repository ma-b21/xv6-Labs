# Lab 1
## sleep
### 主要思路
- 检查参数个数
  - 通过对argc进行判断，sleep操作argc的值应为2.
- 系统调用sleep()
  - 函数原型为sleep(int),因此要先将argv[1]参数转换为int类型再进行系统调用sleep().

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
### 参考链接
- [Linux grep 命令](https://www.runoob.com/linux/linux-comm-grep.html)
- [xargs原理及用法详解](https://www.cnblogs.com/liujunjun/p/11977906.html)