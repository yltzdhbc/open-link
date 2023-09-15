# Open-Link

Open-Link是一个具有野心的项目
其被设计出来主要为了解决设备间通信和OTA问题
包含以下部分
- Open Protocol
- Open Bootloader
- Uploader

## Open Protocol
开放协议

### 主要特性
- 无关硬件：定义在应用层，可以运行在UART、CAN、SPI、I2C、以太网、WIFI等任意物理链路中
- 路由机制：同一个设备可以绑定多个端口（如一个CAN，一个UART），协议通过链路中设备唯一ID来识别对方，如果通过CAN收到的消息识别到接收者并非自己，则会通过UART接口转发，实现消息的路由。
- 主动应答：可开启ACK，每个报文具有唯一的seq，可自动实现丢包补发，解决传输丢包问题。



## Open Bootloader(开发中)
简单易于移植的Bootloader

### 主要特性
- 支持 Open Protocol
- 底层抽象


## Uploader
上位机工具

目前是一个命令行，基于python，可以通过串口实现对固件的下载
### 主要特性
- 下载固件
- 易于使用
