# Canalyst-II USB-CAN Interface Protocol Documentation

## Overview
This document describes the reverse-engineered USB protocol for the Canalyst-II USB-CAN interface. The protocol allows communication with the device's dual CAN channels through USB bulk transfers.

## Device Identification
- **Vendor ID**: 0x04D8 (Microchip)
- **Product ID**: 0x0053
- **Product String**: "Chuangxin Tech USBCAN/CANalyst-II"

## Endpoint Configuration
The device uses multiple bulk endpoints:

| Channel | Command OUT EP | Command IN EP | Message OUT EP | Message IN EP |
|---------|---------------|--------------|---------------|--------------|
| 0       | 0x02          | 0x82         | 0x01          | 0x81         |
| 1       | 0x04          | 0x84         | 0x03          | 0x83         |

## Protocol Structure
All messages use little-endian format with 1-byte packing.

### Common Structures
```c
#pragma pack(push, 1)
typedef struct {
    uint32_t command;
    uint32_t padding[15];  // Total size = 64 bytes
} SimpleCommand;
#pragma pack(pop)
```

## Command Opcodes
| Command | Value | Description |
|---------|-------|-------------|
| INIT    | 0x01  | Initialize CAN channel |
| START   | 0x02  | Start CAN channel |
| STOP    | 0x03  | Stop CAN channel |
| CLEAR_RX_BUFFER | 0x05 | Clear receive buffer |
| MESSAGE_STATUS | 0x0A | Get message queue status |
| CAN_STATUS | 0x0B | Get CAN controller status |
| PREINIT | 0x13 | Unknown initialization (observed but undocumented) |

## Message Formats

### 1. CAN Message Format
```c
typedef struct {
    uint32_t can_id;      // CAN identifier
    uint32_t timestamp;   // Timestamp in 100μs units
    int8_t time_flag;     // Always 1 (observed)
    int8_t send_type;     // Bitmask of SEND_TYPE_* flags
    uint8_t remote;       // 1 if remote frame
    uint8_t extended;     // 1 if extended ID
    int8_t data_len;      // Data length (0-8)
    uint8_t data[8];      // CAN data
} CANMessage;
```

**Flags for send_type**:
- `SEND_TYPE_NORETRY` (0x01): Don't retry if transmission fails
- `SEND_TYPE_ECHO` (0x02): Echo transmitted messages back

### 2. Message Buffer
```c
typedef struct {
    int8_t count;         // Number of messages (0-3)
    CANMessage messages[3]; // CAN messages
} CANMessageBuffer;
```

### 3. INIT Command
```c
typedef struct {
    uint32_t command;     // 0x01 (COMMAND_INIT)
    uint32_t acc_code;    // Acceptance code (typically 0x1)
    uint32_t acc_mask;    // Acceptance mask (typically 0xFFFFFFFF)
    uint32_t unknown0;    // Unknown (typically 0x0)
    uint32_t filter;      // 0x1=SingleFilter, 0x0=DualFilter
    uint32_t unknown1;    // Unknown (typically 0x0)
    uint32_t timing0;     // BTR0 timing value
    uint32_t timing1;     // BTR1 timing value
    uint32_t mode;        // Unknown (0x0=normal, 0x1 may crash)
    uint32_t unknown2;    // Unknown (typically 0x1)
    uint32_t padding[6];  // Total size = 64 bytes
} InitCommand;
```

### 4. Status Responses
#### Message Status Response
```c
typedef struct {
    uint32_t command;     // 0x0A
    uint32_t rx_pending;  // Number of received messages waiting
    uint16_t tx_pending;  // Number of messages pending transmission
    uint16_t unknown;     // Unknown (possibly error flag)
    uint32_t padding[13]; // Total size = 64 bytes
} MessageStatusResponse;
```

#### CAN Status Response
```c
typedef struct {
    uint32_t command;        // 0x0B
    uint32_t err_interrupt;  // Error interrupt status
    uint32_t reg_mode;       // CAN controller mode register
    uint32_t reg_status;     // CAN controller status register
    uint32_t reg_al_capture; // Arbitration lost capture
    uint32_t reg_ec_capture; // Error code capture
    uint32_t reg_ew_limit;   // Error warning limit
    uint32_t reg_re_counter; // Receive error counter
    uint32_t reg_te_counter; // Transmit error counter
    uint32_t padding[7];     // Total size = 64 bytes
} CANStatusResponse;
```

## Bit Timing Values
Common bitrate timing values (BTR0/BTR1):

| Bitrate (bps) | BTR0 | BTR1 |
|---------------|------|------|
| 10,000        | 0x31 | 0x1C |
| 20,000        | 0x18 | 0x1C |
| 50,000        | 0x09 | 0x1C |
| 100,000       | 0x04 | 0x1C |
| 125,000       | 0x03 | 0x1C |
| 250,000       | 0x01 | 0x1C |
| 500,000       | 0x00 | 0x1C |
| 800,000       | 0x00 | 0x16 |
| 1,000,000     | 0x00 | 0x14 |

## Communication Flow

### Initialization Sequence
1. Send `InitCommand` with desired bit timing
2. Send `SimpleCommand` with `COMMAND_START`

### Sending Messages
1. Pack 1-3 CAN messages into a `CANMessageBuffer`
2. Send to message endpoint (0x01 or 0x03)
3. Optionally check status with `COMMAND_MESSAGE_STATUS`

### Receiving Messages
1. Check status with `COMMAND_MESSAGE_STATUS`
2. If messages available, read from message IN endpoint (0x81 or 0x83)
3. Parse `CANMessageBuffer` structure(s)

## Error Conditions
- **Initialization Failure**: May occur with invalid timing values
- **Mode Setting**: Setting `mode=0x1` in `InitCommand` may crash device
- **Buffer Overflow**: Sending >3 messages per buffer may corrupt communication

## Example Code Snippets

### Initializing Channel 0 at 500kbps
```c
InitCommand init = {
    .command = COMMAND_INIT,
    .acc_code = 0x1,
    .acc_mask = 0xFFFFFFFF,
    .filter = 0x1,
    .timing0 = 0x00,
    .timing1 = 0x1C,
    .unknown2 = 0x1
};
send_command(device, 0, &init, sizeof(init), NULL, 0);

SimpleCommand start = { .command = COMMAND_START };
send_command(device, 0, &start, sizeof(start), NULL, 0);
```

### Sending a CAN Message
```c
CANMessage msg = {
    .can_id = 0x123,
    .data_len = 8,
    .data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
};

CANMessageBuffer buf = {
    .count = 1,
    .messages = {msg}
};

#if PLATFORM_WINDOWS
WinUsb_WritePipe(winusb_handle, 0x01, (PUCHAR)&buf, sizeof(buf), &transferred, NULL);
#elif PLATFORM_LINUX
libusb_bulk_transfer(dev, 0x01, (unsigned char *)&buf, sizeof(buf), &transferred, 1000);
#endif
```

## Notes
1. The protocol always uses 64-byte packets, padding as needed
2. All multi-byte values are little-endian
3. The device appears to buffer up to 3 CAN messages per USB packet
4. Some fields' exact purposes remain unknown (marked as unknown)

This documentation represents the current understanding of the protocol based on reverse engineering. Some behaviors may vary with different firmware versions.