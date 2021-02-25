#应用层Buffer实现
###为什么non-blocking网络编程中应用层buffer是必须的？
Non-blocking IO 的核心思想是避免阻塞在read()或write或其他IO系统调用上，这样可以最大限度的复用thread-of-control，让一个线程能够服务于多个socket连接。IO线程只能阻塞在epoll_wait()上。
这样一来，应用层的buffer是必须的，每个TCP socket 都要有stateful 的 input buffer 和output buffer。

###应用层buffer的设计要点：
1.对外表现为一块连续的内存，以方便业务代码的编写
2.其size支持自动增长，以适应不同大小的消息
3.内部以 vector<char> 来保存数据，并提供相应的读写接口

###谁写谁读？
写：TCP会从socket读取数据，然后写入Buffer
读：业务代码从Buffer读