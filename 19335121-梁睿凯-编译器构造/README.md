# 一、文件目录说明

report目录下保存设计报告。

src目录下保存源代码。

test目录下保存测试样例程序。

bin目录下保存可执行程序（运行环境：ubuntu22.04)



## 二、程序复现说明

- linux环境下

  ```shell
  cd src
  make && make run
  ```

  文件名输入例子：

  ```shell
  ../test/test1
  ```

- windows环境下

  使用g++或clang++编译compiler.cpp，运行。

由于程序所有输出均在终端显示，故无测试输出样例文件。