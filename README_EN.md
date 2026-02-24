# libUartCommProtocolV2

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

A lightweight, portable UART communication protocol stack implementing TCP-like reliable transmission with guaranteed delivery, ordering, and deduplication. Also supports UDP-like unreliable transmission.

## Features

- **TCP-like Reliable Transmission**: Guarantees message delivery, ordering, and deduplication
- **UDP-like Unreliable Transmission**: Fast, best-effort delivery without acknowledgment
- **Platform Independent**: No system-specific dependencies, fully portable
- **Minimal Footprint**: Suitable for embedded systems with limited resources
- **Thread-Safe**: Optional semaphore support for concurrent access
- **CRC16 Checksum**: Data integrity verification
- **Automatic Retransmission**: Configurable retry mechanism for reliable mode

## Protocol Frame Format

```
|--6byte-|-1byte-|-1byte-|-2byte-|-2byte-|-2byte-|-2byte-|-N byte--|
|"uArTcP"|  seq  |  ctrl |  cmd  | crc16 |  len  |cs(len)|payload |
```

- **sync**: Magic bytes "uArTcP" for frame synchronization
- **seq**: Sequence number for ordering and deduplication
- **ctrl**: Control flags (ACK, ACKED, NACK)
- **cmd**: Command identifier [1-10000]
- **crc16**: CRC16 checksum of entire frame
- **len**: Payload length
- **cs(len)**: CRC16 checksum of payload length
- **payload**: Message data

## Quick Start

### 1. Register Platform Hooks

```c
#include "uni_communication.h"

CommProtocolHooks hooks = {
    .malloc_fn  = malloc,
    .free_fn    = free,
    .realloc_fn = realloc,
    .msleep_fn  = my_sleep_ms,
    
    // Optional: for better performance
    .sem_alloc_fn     = my_sem_alloc,
    .sem_destroy_fn   = my_sem_destroy,
    .sem_init_fn      = my_sem_init,
    .sem_post_fn      = my_sem_post,
    .sem_wait_fn      = my_sem_wait,
    .sem_timedwait_fn = my_sem_timedwait
};

CommProtocolRegisterHooks(&hooks);
```

### 2. Initialize Protocol Stack

```c
int uart_write(char *buf, unsigned int len) {
    // Your UART write implementation
    return write(uart_fd, buf, len);
}

void on_packet_received(CommPacket *packet) {
    printf("Received cmd=%d, len=%d\n", packet->cmd, packet->payload_len);
    // Process packet->payload
}

CommProtocolInit(uart_write, on_packet_received);
```

### 3. Receive UART Data

```c
// In your UART receive thread/interrupt
void uart_rx_handler(unsigned char *data, int len) {
    CommProtocolReceiveUartData(data, len);
}
```

### 4. Send Data

```c
#define CMD_HEARTBEAT 100

typedef struct {
    uint32_t timestamp;
    uint16_t counter;
} heartbeat_t;

heartbeat_t hb = {.timestamp = get_time(), .counter = 1};

// Reliable transmission (TCP-like)
int ret = CommProtocolPacketAssembleAndSend(CMD_HEARTBEAT, (char*)&hb, sizeof(hb), 1);

// Unreliable transmission (UDP-like)
ret = CommProtocolPacketAssembleAndSend(CMD_HEARTBEAT, (char*)&hb, sizeof(hb), 0);
```

## API Reference

### Core Functions

#### `CommProtocolRegisterHooks(CommProtocolHooks *hooks)`
Register platform-specific functions for memory allocation, synchronization, and sleep.

**Parameters:**
- `hooks`: Pointer to hooks structure

**Required hooks:**
- `malloc_fn`, `free_fn`, `realloc_fn`, `msleep_fn`

**Optional hooks (recommended):**
- Semaphore functions for thread safety

---

#### `CommProtocolInit(CommWriteHandler write_handler, CommRecvPacketHandler recv_handler)`
Initialize the protocol stack.

**Parameters:**
- `write_handler`: UART write function callback
- `recv_handler`: Packet receive callback

**Returns:** 0 on success, -1 on failure

---

#### `CommProtocolPacketAssembleAndSend(CommCmd cmd, char *payload, CommPayloadLen payload_len, int mode)`
Send a packet through the protocol stack.

**Parameters:**
- `cmd`: Command ID [1-10000]
- `payload`: Message data (NULL if no payload)
- `payload_len`: Payload length (0 if no payload)
- `mode`: 1 for reliable (TCP-like), 0 for unreliable (UDP-like)

**Returns:**
- `0`: Success
- `E_UNI_COMM_ALLOC_FAILED`: Memory allocation failed
- `E_UNI_COMM_PAYLOAD_TOO_LONG`: Payload exceeds max size (8KB)
- `E_UNI_COMM_PAYLOAD_ACK_TIMEOUT`: ACK timeout in reliable mode

---

#### `CommProtocolReceiveUartData(unsigned char *buf, int len)`
Feed received UART data into the protocol stack for parsing.

**Parameters:**
- `buf`: Received data buffer
- `len`: Data length

---

#### `CommProtocolFinal(void)`
Cleanup and release all resources. Should rarely be called in embedded systems.

## Configuration

Edit `uni_communication.c` to adjust:

```c
#define PROTOCOL_BUF_SUPPORT_MAX_SIZE (8192)  // Max packet size
#define WAIT_ACK_TIMEOUT_MSEC         (200)   // ACK timeout
#define TRY_RESEND_TIMES              (5)     // Retry count
```

## Platform Porting Examples

See `example/` directory:
- `linux_posix_demo.c` - Linux/POSIX systems
- `rt_thread_demo.c` - RT-Thread RTOS
- `8051_demo.c` - 8051 microcontroller

## Thread Safety

- **With semaphore hooks**: Fully thread-safe for concurrent send/receive
- **Without semaphore hooks**: User must ensure external synchronization

## Performance

At 921600 bps with 512-byte payload:
- Bandwidth utilization: ~80% (90 KB/s)
- Window size: 1 (stop-and-wait ARQ)

## Error Handling

```c
int ret = CommProtocolPacketAssembleAndSend(cmd, data, len, 1);
switch (ret) {
    case 0:
        // Success
        break;
    case E_UNI_COMM_ALLOC_FAILED:
        // Out of memory
        break;
    case E_UNI_COMM_PAYLOAD_TOO_LONG:
        // Payload > 8KB
        break;
    case E_UNI_COMM_PAYLOAD_ACK_TIMEOUT:
        // No ACK received after retries
        break;
}
```

## Important Notes

⚠️ **Do NOT call `CommProtocolPacketAssembleAndSend` inside the receive callback** - use async mechanism (separate thread/task) to avoid deadlock.

⚠️ **Command ID range**: Use [1-10000], other values are reserved.

⚠️ **Max payload size**: 8192 bytes (configurable).

## Building

```bash
# Include in your project
gcc -I./protocol -c protocol/uni_communication.c -o uni_communication.o

# Link with your application
gcc your_app.c uni_communication.o -o your_app
```

## License

GPL v2 - See [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## Author

- **Junlon** - junlon2006@163.com

## Changelog

### v2.0
- Refactored protocol stack
- Added payload length CRC16 verification
- Improved memory management
- Enhanced portability
