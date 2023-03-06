# 网络流程

## 网络编程涉及的api

---
1.服务器        
- socket()  
- bind()  
- connect()  
- send()
- recv()
- close()  
---
2.客户端
- socket()  
- bind()
- listen()
- clientfd=accept()
- send()
- recv()
- close()  

3.udp包使用sendto(),recvfrom()




## tcp过程

---
### 连接建立
![image](G:\cpp(git上所传文件图片)\110022.jpg)


- 服务器端的链接从接收到syn开始，到time_wait结束。其中包含两个队列，半连接与全连接队列。
- 全连接队列通过五元组（源ip，源端口，目的ip，目的端口，协议）标识连接是否在半连接队列。
- 一台主机同时tcp：listen(8888)，udp：recvfrom(8888)可以同时使用，因为五元组的协议不同。
- 一方bind(9999)connect(8888),另一方bind(8888)connect(9999)，可实现tcp的p2p，两方客户端的通信。

同时建立
![image](G:\cpp(git上所传文件图片)\110024.png)
---
### 传输过程  

- 使用延迟ack保证tcp包的有序性
- 重传  
  *RTT（Round Trip Time）：往返时延，也就是数据包从发出去到收到对应 ACK 的时间。*  
  *RTO（Retransmission Time Out）：重传超时，超时时间。*    
  超时重传，快速重传（三次一样ACK则重传），在前两者基础上有SACK重传（SACK标识应重传的包序号），D-SACK重传
  （D-SACK标识已经接受过的包序号）  

---
  
### 断开过程
![image](G:\cpp(git上所传文件图片)\110026.png)
- 服务器收到客户端的fin包后可能也有数据需要传输，等待传输完毕再发出fin包所以共计四次交互。
- wait2MSL原因：1.wait原因：服务器的fin+ack包可能重传；2.wait2MSL原因：使本阶段报文消失，保证下一阶段没有旧包的干扰。

同时关闭
![image](G:\cpp(git上所传文件图片)\110023.png)