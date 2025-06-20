/*
  This handler is inspired by Unofficial Python 
  userspace driver for the low cost USB analyzer 
  "Canalyst-II" by Chuangxin Technology.
  
  https://github.com/projectgus/python-canalystii

  Copyright (c) 2025, Fikri Bachtiar
*/

#ifndef CANALYST_H
#define CANALYST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "libusb.h"

#ifdef __cplusplus
extern "C" {
#endif

// Constants from protocol.py
#define USB_ID_VENDOR  0x04D8
#define USB_ID_PRODUCT 0x0053

// Command opcodes
#define COMMAND_INIT           0x1
#define COMMAND_START          0x2
#define COMMAND_STOP           0x3
#define COMMAND_CLEAR_RX_BUFFER 0x5
#define COMMAND_MESSAGE_STATUS 0x0A
#define COMMAND_CAN_STATUS     0x0B

// Endpoint mapping
#define CHANNEL_TO_COMMAND_EP(ch) ((ch) == 0 ? 2 : 4)
#define CHANNEL_TO_MESSAGE_EP(ch) ((ch) == 0 ? 1 : 3)

// Message flags
#define SEND_TYPE_NORETRY 1
#define SEND_TYPE_ECHO    2

#pragma pack(push, 1)
typedef struct {
    uint32_t can_id;
    uint32_t timestamp;
    int8_t time_flag;
    int8_t send_type;
    uint8_t remote;
    uint8_t extended;
    int8_t data_len;
    uint8_t data[8];
} Canalyst_CANMessage;

typedef struct {
    int8_t count;
    Canalyst_CANMessage messages[3];
} Canalyst_CANMessageBuffer;

typedef struct {
    uint32_t command;
    uint32_t padding[15];
} Canalyst_SimpleCommand;

typedef struct {
    uint32_t command;
    uint32_t acc_code;
    uint32_t acc_mask;
    uint32_t unknown0;
    uint32_t filter;
    uint32_t unknown1;
    uint32_t timing0;
    uint32_t timing1;
    uint32_t mode;
    uint32_t unknown2;
    uint32_t padding[6];
} Canalyst_InitCommand;

typedef struct {
    uint32_t command;
    uint32_t rx_pending;
    uint16_t tx_pending;
    uint16_t unknown;
    uint32_t padding[13];
} Canalyst_MessageStatusResponse;

typedef struct {
    uint32_t command;
    uint32_t err_interrupt;
    uint32_t reg_mode;
    uint32_t reg_status;
    uint32_t reg_al_capture;
    uint32_t reg_ec_capture;
    uint32_t reg_ew_limit;
    uint32_t reg_re_counter;
    uint32_t reg_te_counter;
    uint32_t padding[7];
} Canalyst_CANStatusResponse;
#pragma pack(pop)

// Bitrate timing table
typedef struct {
    int bitrate;
    uint8_t timing0;
    uint8_t timing1;
} Canalyst_BitrateTiming;

static const Canalyst_BitrateTiming TIMINGS[] = {
    {5000, 0xBF, 0xFF},
    {10000, 0x31, 0x1C},
    {20000, 0x18, 0x1C},
    {33330, 0x09, 0x6F},
    {40000, 0x87, 0xFF},
    {50000, 0x09, 0x1C},
    {66660, 0x04, 0x6F},
    {80000, 0x83, 0xFF},
    {83330, 0x03, 0x6F},
    {100000, 0x04, 0x1C},
    {125000, 0x03, 0x1C},
    {200000, 0x81, 0xFA},
    {250000, 0x01, 0x1C},
    {400000, 0x80, 0xFA},
    {500000, 0x00, 0x1C},
    {666000, 0x80, 0xB6},
    {800000, 0x00, 0x16},
    {1000000, 0x00, 0x14},
    {0, 0, 0} // Terminator
};

// Device structure with libusb handle
typedef struct {
    libusb_device_handle *dev;
    int initialized[2];
    int started[2];
    bool libusb_initialized;
    uint8_t command_eps[2];  // Endpoints for channels 0 and 1
    uint8_t message_eps[2];  // Endpoints for channels 0 and 1
} Canalyst_Device;

char *canalyst_get_name();

bool canalyst_dev_scan(Canalyst_Device *device);

bool canalyst_dev_init(Canalyst_Device *device, int bitrate);
bool canalyst_dev_close(Canalyst_Device *device);

bool canalyst_dev_frame_send(Canalyst_Device *device, const Canalyst_CANMessage *msg);
int canalyst_dev_frame_read(Canalyst_Device *device, Canalyst_CANMessage **msg, int *count);

#ifdef __cplusplus
}
#endif

#endif /* CANALYST_H */