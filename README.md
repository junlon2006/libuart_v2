# libUartCommProtocolV2

[English](README_EN.md) | 中文

一个轻量级、可移植的串口通信协议栈，实现类TCP可靠传输，保证消息必达、有序、不重复。同时支持类UDP不可靠传输。

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![CI](https://github.com/junlon2006/libuart_v2/workflows/CI/badge.svg)](https://github.com/junlon2006/libuart_v2/actions)

## 特性

- ✅ **类TCP可靠传输**：保证消息必达、有序、不重复
- ✅ **类UDP不可靠传输**：快速、尽力而为的传输
- ✅ **平台无关**：无系统依赖，完全可移植
- ✅ **占用空间小**：适合资源受限的嵌入式系统
- ✅ **线程安全**：可选的信号量支持并发访问
- ✅ **CRC16校验**：数据完整性验证
- ✅ **自动重传**：可配置的重试机制

## 快速开始

### 1. 注册平台钩子函数

```c
#include "uni_communication.h"

CommProtocolHooks hooks = {
    .malloc_fn  = malloc,
    .free_fn    = free,
    .realloc_fn = realloc,
    .msleep_fn  = my_sleep_ms,
    
    // 可选：提升性能
    .sem_alloc_fn     = my_sem_alloc,
    .sem_destroy_fn   = my_sem_destroy,
    .sem_init_fn      = my_sem_init,
    .sem_post_fn      = my_sem_post,
    .sem_wait_fn      = my_sem_wait,
    .sem_timedwait_fn = my_sem_timedwait
};

CommProtocolRegisterHooks(&hooks);
```

### 2. 初始化协议栈

```c
int uart_write(char *buf, unsigned int len) {
    // 你的串口写实现
    return write(uart_fd, buf, len);
}

void on_packet_received(CommPacket *packet) {
    printf("收到 cmd=%d, len=%d\n", packet->cmd, packet->payload_len);
    // 处理 packet->payload
}

CommProtocolInit(uart_write, on_packet_received);
```

### 3. 接收串口数据

```c
// 在串口接收线程/中断中
void uart_rx_handler(unsigned char *data, int len) {
    CommProtocolReceiveUartData(data, len);
}
```

### 4. 发送数据

```c
#define CMD_HEARTBEAT 100

typedef struct {
    uint32_t timestamp;
    uint16_t counter;
} heartbeat_t;

heartbeat_t hb = {.timestamp = get_time(), .counter = 1};

// 可靠传输（类TCP）
int ret = CommProtocolPacketAssembleAndSend(CMD_HEARTBEAT, (char*)&hb, sizeof(hb), 1);

// 不可靠传输（类UDP）
ret = CommProtocolPacketAssembleAndSend(CMD_HEARTBEAT, (char*)&hb, sizeof(hb), 0);
```

## 编译

### 使用 Make

```bash
make                # 编译库
make examples       # 编译示例
make clean          # 清理
```

### 使用 CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## 协议帧格式

```
|--6byte-|-1byte-|-1byte-|-2byte-|-2byte-|-2byte-|-2byte-|-N byte--|
|"uArTcP"|  seq  |  ctrl |  cmd  | crc16 |  len  |cs(len)|payload |
```

## API 文档

详细文档请参考 [README_EN.md](README_EN.md)

## 平台移植示例

- `example/linux_posix_demo.c` - Linux/POSIX系统
- `example/rt_thread_demo.c` - RT-Thread RTOS
- `example/8051_demo.c` - 8051单片机

## 性能

在921600波特率下，512字节负载：
- 带宽利用率：约80%（90 KB/s）
- 窗口大小：1（停等ARQ）

## 注意事项

⚠️ **不要在接收回调中调用发送函数** - 使用异步机制（独立线程/任务）避免死锁

⚠️ **命令ID范围**：使用 [1-10000]，其他值保留

⚠️ **最大负载**：8192字节（可配置）

## 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md)

## 许可证

GPL v2 - 详见 [LICENSE](LICENSE) 文件

## 作者

- **Junlon** - junlon2006@163.com
