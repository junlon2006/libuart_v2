/**************************************************************************
 * Copyright (C) 2020-2020  Junlon2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **************************************************************************
 *
 * Description : uni_communication.h
 * Author      : junlon2006@163.com
 * Date        : 2020.04.21
 *
 **************************************************************************/
#ifndef UNI_COMMUNICATION_H_
#define UNI_COMMUNICATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Compiler-specific packed attribute */
#if defined(__GNUC__) || defined(__clang__)
#define PACKED __attribute__((packed))
#elif defined(_MSC_VER)
#define PACKED
#pragma pack(push, 1)
#else
#define PACKED
#endif

/* Protocol configuration */
#define COMM_CMD_MIN            1
#define COMM_CMD_MAX            10000
#define COMM_MAX_PAYLOAD_SIZE   8192

/* Type definitions */
typedef uint16_t            CommCmd;
typedef uint16_t            CommPayloadLen;
typedef int                 (*CommWriteHandler)(char *buf, unsigned int len);

/**
 * 协议栈解析输出结构体
 */
typedef struct {
  CommCmd        cmd;         /**< 消息类型，全局唯一，请使用[1, 10000]闭区间的值，其他值不可用 */
  CommPayloadLen payload_len; /**< 消息参数长度 */
  char*          payload;     /**< 消息体 */
} PACKED CommPacket;

/**
 * Error codes returned by protocol functions
 */
typedef enum {
  E_UNI_COMM_SUCCESS              = 0,      /**< Operation successful */
  E_UNI_COMM_ALLOC_FAILED         = -10001, /**< Memory allocation failed */
  E_UNI_COMM_BUFFER_PTR_NULL      = -10002, /**< Buffer pointer is NULL */
  E_UNI_COMM_PAYLOAD_TOO_LONG     = -10003, /**< Payload exceeds max size */
  E_UNI_COMM_PAYLOAD_ACK_TIMEOUT  = -10004, /**< ACK timeout in reliable mode */
  E_UNI_COMM_NOT_INITIALIZED      = -10005, /**< Protocol not initialized */
  E_UNI_COMM_INVALID_PARAM        = -10006, /**< Invalid parameter */
} CommProtocolErrorCode;

/**
 * Platform-specific function hooks for portability
 * Register these functions to port the protocol to different platforms
 */
typedef struct {
  /* Memory management functions (REQUIRED) */
  void* (*malloc_fn)(unsigned long size);             /**< Allocate memory */
  void  (*free_fn)(void *ptr);                        /**< Free memory */
  void* (*realloc_fn)(void *ptr, unsigned long size); /**< Reallocate memory */

  /* Semaphore functions (OPTIONAL, recommended for thread safety) */
  void* (*sem_alloc_fn)(void);                         /**< Allocate semaphore handle */
  void  (*sem_destroy_fn)(void *sem);                  /**< Destroy semaphore */
  int   (*sem_init_fn)(void *sem, unsigned int value); /**< Initialize semaphore */
  int   (*sem_post_fn)(void *sem);                     /**< Post/signal semaphore */
  int   (*sem_wait_fn)(void *sem);                     /**< Wait on semaphore */
  int   (*sem_timedwait_fn)(void *sem, unsigned int timeout_msecond); /**< Timed wait on semaphore */

  /* Sleep function (REQUIRED) */
  int (*msleep_fn)(unsigned int msecond); /**< Sleep for milliseconds */
} CommProtocolHooks;

typedef void (*CommRecvPacketHandler)(CommPacket *packet);

/**
 * @brief Register platform-specific hooks for protocol portability
 * 
 * Must be called before CommProtocolInit(). Registers memory management,
 * synchronization, and sleep functions required by the protocol stack.
 * 
 * @param[in] hooks Pointer to hooks structure with platform functions
 * @note Required hooks: malloc_fn, free_fn, realloc_fn, msleep_fn
 * @note Optional hooks: semaphore functions (for thread safety)
 */
void CommProtocolRegisterHooks(CommProtocolHooks *hooks);

/**
 * @brief Initialize the protocol stack
 * 
 * Initializes internal state, allocates resources, and registers callbacks.
 * Must be called after CommProtocolRegisterHooks().
 * 
 * @param[in] write_handler UART write function callback for sending data
 * @param[in] recv_handler  Callback invoked when a packet is received
 * @return 0 on success, -1 on failure
 * @note write_handler will be called from protocol context
 * @note recv_handler must NOT call CommProtocolPacketAssembleAndSend directly
 */
int CommProtocolInit(CommWriteHandler write_handler, CommRecvPacketHandler recv_handler);

/**
 * @brief Cleanup and release all protocol resources
 * 
 * Frees all allocated memory and resets internal state.
 * Should rarely be called in embedded systems.
 */
void CommProtocolFinal(void);

/**
 * @brief Assemble and send a packet through the protocol stack
 * 
 * Supports both reliable (TCP-like) and unreliable (UDP-like) transmission.
 * In reliable mode, the function blocks until ACK is received or timeout.
 * 
 * @param[in] cmd         Command ID [1-10000], globally unique message type
 * @param[in] payload     Message payload data, NULL if no payload
 * @param[in] payload_len Payload length in bytes, 0 if no payload
 * @param[in] mode        1 = reliable (TCP-like), 0 = unreliable (UDP-like)
 * @return Error code (see CommProtocolErrorCode)
 * @retval 0 Success
 * @retval E_UNI_COMM_ALLOC_FAILED Memory allocation failed
 * @retval E_UNI_COMM_PAYLOAD_TOO_LONG Payload exceeds max size
 * @retval E_UNI_COMM_PAYLOAD_ACK_TIMEOUT ACK timeout (reliable mode only)
 * @note Thread-safe if semaphore hooks are registered
 * @warning Do NOT call from recv_handler callback
 */
int CommProtocolPacketAssembleAndSend(CommCmd cmd, char *payload,
                                      CommPayloadLen payload_len,
                                      int mode);

/**
 * @brief Feed received UART data into the protocol stack for parsing
 * 
 * Call this function from your UART receive interrupt/thread to process
 * incoming data. The protocol will parse frames and invoke recv_handler
 * callback when complete packets are received.
 * 
 * @param[in] buf Received UART data buffer
 * @param[in] len Length of received data in bytes
 * @note Can be called with partial frame data, protocol handles buffering
 * @note Thread-safe if semaphore hooks are registered
 */
void CommProtocolReceiveUartData(unsigned char *buf, int len);

#ifdef __cplusplus
}
#endif

#if defined(_MSC_VER) && !defined(PACKED)
#pragma pack(pop)
#endif

#endif  /* UNI_COMMUNICATION_H_ */
