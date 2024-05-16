# starting section
---
1. 借助 rust-sbi 触发 M-mode 的操作
2. 启动 rust-sbi 后,通过 _entry 初始化 CPU 的栈
3. 进入 main 函数,开始进行 S-mode 下的初始化的内容

# 输入输出相关的函数
-> consoleinit()->初始化用于输入输出的函数指针, 初始化管理控制台输入输出的自旋锁
-> consoleread()->devsw\[CONSOLE\].read 函数指针保存, 用于从控制台读取信息
-> consolewrite()->devsw\[CONSOLE\].write 指针保存, 用于向控制台写入内容
-> 对于 user 的输入输出需要借助 either_copyin, either_copyout, 将 src 在 kernel 和 user 之间传送

# process management
---
## process struct info :
1. pid -> 进程号
2. struct trapframe -> trap 过程中需要保存的信息
3. struct Context -> 进程的上下文信息
4. user page table -> user 页表信息
5. size -> 进程的 memory size
6. ...
