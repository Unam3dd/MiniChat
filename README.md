# Minichat

## Description

The aim of this project is to compare the use and performance of system calls for multiplexing.

in this project i'm redoing 3 minichat servers one with select, poll and epoll with (advantages/disadvantages)

I have not yet tested libuv, libev, or liburing (io_uring) nor have I tried kqueue, which is relatively close to epoll. In the future, this project is not a professional project, so it will not be updated over time unless I get the urge.

I haven't tested thread pools coupled with epoll either.

## C10k Problem

The C10k problem is the problem of optimizing network sockets to handle a large number of clients at the same time.[1] The name C10k is a numeronym for concurrently handling ten thousand connections.[2] Handling many concurrent connections is a different problem from handling many requests per second: the latter requires high throughput (processing them quickly), while the former does not have to be fast, but requires efficient scheduling of connections.

The problem of socket server optimisation has been studied because a number of factors must be considered to allow a web server to support many clients. This can involve a combination of operating system constraints and web server software limitations. According to the scope of services to be made available and the capabilities of the operating system as well as hardware considerations such as multi-processing capabilities, a multi-threading model or a single threading model can be preferred. Concurrently with this aspect, which involves considerations regarding memory management (usually operating system related), strategies implied relate to the very diverse aspects of I/O management.[2]
