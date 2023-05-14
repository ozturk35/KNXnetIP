/**
 * \file KnxTelegram.h
 * 
 * \brief KNX Telegram
 * 
 * This file contains the implementation of Knx Telegram module
 * 
 * \version 1.0.0
 * 
 * \author Ibrahim Ozturk
 * 
 * Copyright 2023 Ibrahim Ozturk
 * All rights exclusively reserved for Ibrahim Ozturk,
 * unless expressly agreed to otherwise.
*/

#ifndef KNXTELEGRAM_H
#define KNXTELEGRAM_H

/*==================[inclusions]============================================*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

/*==================[macros]================================================*/

/**  Knx Telegram description
 * - Length : 9 - 23 bytes
 *
 * - Structure :
 *      - Header (6 bytes):
 *        Byte 0 | Control Field
 *        Byte 1 | Source Address High byte
 *        Byte 2 | Source Address Low byte
 *        Byte 3 | Destination Address High byte
 *        Byte 4 | Destination Address Low byte
 *        Byte 5 | Routing field
 *      - Payload (from 2 up to 16 bytes):
 *        Byte 6 | Commmand field High
 *        Byte 7 | Command field Low + 1st payload data (6bits)
 *        Byte 8 up to 21 | payload bytes (optional)
 *      - Checksum (1 byte)
 *
 * - Fields details :
 *      -Control Field : "FFR1 PP00" format with
 *         FF = Frame Format (10 = Std Length L_DATA service, 00 = extended L_DATA service, 11 = L_POLLDATA service)
 *          R = Repeatflag (1 = not repeated, 0 = repeated)
 *         PP = Priority (00 = system, 10 = alarm, 01 = high, 11 = normal)
 *      -Routing Field  : "TCCC LLLL" format with
 *          T = Target Addr type (1 = group address/muticast, 0 = individual address/unicast)
 *        CCC = Counter
 *       LLLL = Payload Length (1-15)
 *      -Command Field : "00XX XXCC CCDD DDDD" format with
 *         XX = Not used
 *         CC = command (0000 = Value Read, 0001 = Value Response, 0010 = Value Write, 1010 = Memory Write)
 *         DD = Payload Data (1st payload byte)
 *
 * - Transmit timings :
 *     -Tbit = 104us, Tbyte=1,35ms (13 bits per character)
 *     -from 20ms for 1 byte payload telegram (Bus temporisation + Telegram transmit + ACK)
 *     -up to 40ms for 15 bytes payload (Bus temporisation + Telegram transmit + ACK)
 *
*/

#define KNX_TELEGRAM_HEADER_SIZE        (6U)
#define KNX_TELEGRAM_PAYLOAD_MAX_SIZE  (16U)
#define KNX_TELEGRAM_MIN_SIZE           (9U)
#define KNX_TELEGRAM_MAX_SIZE          (23U)
#define KNX_TELEGRAM_LENGTH_OFFSET      (8U) /* Offset between payload length and telegram length */

/* Control Field (CTRL) values & masks */ 
#define CONTROL_FIELD_DEFAULT_VALUE         (0xBCU) /* Standard FF; No Repeat; Normal Priority */
#define CONTROL_FIELD_FRAME_FORMAT_MASK     (0xC0U)
#define CONTROL_FIELD_STANDARD_FRAME_FORMAT (0x80U)
#define CONTROL_FIELD_REPEATED_MASK         (0x20U)
#define CONTROL_FIELD_SET_REPEATED(x)       (x &= 0xDFU)
#define CONTROL_FIELD_PRIORITY_MASK         (0x0CU)
#define CONTROL_FIELD_PATTERN_MASK          (0x13U)
#define CONTROL_FIELD_VALID_PATTERN         (0x10U)

/* Length Field values & masks */
#define LENGTH_FIELD_DEFAULT_VALUE            (0xE1U) /* Multicast(Target Group @), Routing Counter = 6, Length = 1 */
#define LENGTH_FIELD_TARGET_ADDRESS_TYPE_MASK (0x80U)
#define LENGTH_FIELD_COUNTER_MASK             (0x70U)
#define LENGTH_FIELD_PAYLOAD_LENGTH_MASK      (0x0FU)

/* Command Field values & masks */
#define COMMAND_FIELD_HIGH_COMMAND_MASK (0x03U)
#define COMMAND_FIELD_LOW_COMMAND_MASK  (0xC0U)
#define COMMAND_FIELD_LOW_DATA_MASK     (0x3FU)
#define COMMAND_FIELD_PATTERN_MASK      (0xC0U)
#define COMMAND_FIELD_VALID_PATTERN     (0x00U)

/*==================[type definitions]======================================*/
typedef enum {
    KNX_PRIORITY_SYSTEM_VALUE  = 0x00U,
    KNX_PRIORITY_HIGH_VALUE    = 0x04U,
    KNX_PRIORITY_ALARM_VALUE   = 0x08U,
    KNX_PRIORITY_NORMAL_VALUE  = 0x0CU
} Knx_PriorityType;

typedef enum {
    KNX_COMMAND_VALUE_READ     = 0x00U,
    KNX_COMMAND_VALUE_RESPONSE = 0x01U,
    KNX_COMMAND_VALUE_WRITE    = 0x02U,
    KNX_COMMAND_MEMORY_WRITE   = 0x0AU,
} Knx_CommandType;

typedef enum {
    KNX_TELEGRAM_VALID = 0 ,
    KNX_TELEGRAM_INVALID_CONTROL_FIELD,
    KNX_TELEGRAM_UNSUPPORTED_FRAME_FORMAT,
    KNX_TELEGRAM_INCORRECT_PAYLOAD_LENGTH,
    KNX_TELEGRAM_INVALID_COMMAND_FIELD,
    KNX_TELEGRAM_UNKNOWN_COMMAND,
    KNX_TELEGRAM_INCORRECT_CHECKSUM
} KnxTelegram_ValidityType;

typedef struct{
    union {
        uint8_t bytes[KNX_TELEGRAM_MAX_SIZE]; // bytes 0 to 22
        struct {
            uint8_t controlField; /* Control Field */
            uint16_t sourceAddr; /* Source Address */
            uint16_t destAddr; /* Destination Address */
            uint8_t length; /* Length */
            uint16_t cmd; /* Command */
            uint8_t payloadChecksum[KNX_TELEGRAM_PAYLOAD_MAX_SIZE-1]; /* bytes 8 to 22 */
      } l_data;
    };
} KnxTelegram_Type;

#endif /* #ifndef KNXTELEGRAM_H */

/*==================[end of file]===========================================*/
