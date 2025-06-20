/*
  This handler is inspired by Unofficial Python 
  userspace driver for the low cost USB analyzer 
  "Canalyst-II" by Chuangxin Technology.
  
  https://github.com/projectgus/python-canalystii

  Copyright (c) 2025, Fikri Bachtiar
*/

#include "canalyst.h"

#ifdef _WIN32
#include <windows.h>
#define sleep_ms(ms) Sleep(ms)
#else
#include <unistd.h>
#define sleep_ms(ms) usleep((ms) * 1000)
#endif

// Helper function to find timing values
static int get_timing_values(int bitrate, uint8_t *timing0, uint8_t *timing1) {
    for (int i = 0; TIMINGS[i].bitrate != 0; i++) {
        if (TIMINGS[i].bitrate == bitrate) {
            *timing0 = TIMINGS[i].timing0;
            *timing1 = TIMINGS[i].timing1;
            return 0;
        }
    }
    return -1;
}

static int canalyst_scan(Canalyst_Device *device) {
    // Initialize libusb
    int r = libusb_init(NULL);
    if (r < 0) {
        fprintf(stderr, "Failed to initialize libusb\n");
        return -1;
    }
    device->libusb_initialized = true;

    libusb_device **list;
    ssize_t cnt = libusb_get_device_list(NULL, &list);
    if (cnt < 0) {
        fprintf(stderr, "Failed to get USB device list\n");
        return -1;
    }

    ssize_t found = 0;
    for (ssize_t i = 0; i < cnt; i++) {
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(list[i], &desc) == 0) {
            if (desc.idVendor == USB_ID_VENDOR && desc.idProduct == USB_ID_PRODUCT) {
                found++;
            }
        }
    }

    libusb_free_device_list(list, 1);
    
    return found;
}

static int canalyst_init(Canalyst_Device *device, int device_index) {
    device->initialized[0] = 0;
    device->initialized[1] = 0;
    device->started[0] = 0;
    device->started[1] = 0;

    int r = libusb_init(NULL);
    if (r < 0) {
        fprintf(stderr, "Failed to initialize libusb\n");
        return -1;
    }
    device->libusb_initialized = true;

    // Enable debug logging if needed
    // libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);

    libusb_device **list;
    ssize_t cnt = libusb_get_device_list(NULL, &list);
    if (cnt < 0) {
        fprintf(stderr, "Failed to get USB device list\n");
        return -1;
    }

    ssize_t found = 0;
    libusb_device *dev = NULL;
    for (ssize_t i = 0; i < cnt; i++) {
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(list[i], &desc) == 0) {
            if (desc.idVendor == USB_ID_VENDOR && desc.idProduct == USB_ID_PRODUCT) {
                if (found == device_index) {
                    dev = list[i];
                    break;
                }
                found++;
            }
        }
    }

    if (dev == NULL) {
        fprintf(stderr, "Canalyst-II device not found\n");
        libusb_free_device_list(list, 1);
        return -1;
    }

    // Open device without requiring root
    r = libusb_open(dev, &device->dev);
    if (r < 0) {
        fprintf(stderr, "Failed to open device (error: %s). Make sure udev rules are set up.\n",
                libusb_error_name(r));
        libusb_free_device_list(list, 1);
        return -1;
    }

    libusb_free_device_list(list, 1);

    // Try to detach kernel driver if active
    if (libusb_kernel_driver_active(device->dev, 0)) {
        r = libusb_detach_kernel_driver(device->dev, 0);
        if (r < 0) {
            fprintf(stderr, "Warning: Could not detach kernel driver: %s\n",
                    libusb_error_name(r));
        }
    }

    // Claim interface
    r = libusb_claim_interface(device->dev, 0);
    if (r < 0) {
        fprintf(stderr, "Failed to claim interface: %s\n", libusb_error_name(r));
        libusb_close(device->dev);
        return -1;
    }

    // Map endpoints to channels (these are typical for Canalyst-II devices)
    device->message_eps[0] = 0x01;
    device->command_eps[0] = 0x02;
    device->message_eps[1] = 0x03;
    device->command_eps[1] = 0x04;

    return 0;
}

static void canalyst_close(Canalyst_Device *device) {
    if (device->dev) {
        if(device->initialized[0] || device->initialized[1]) {
            libusb_release_interface(device->dev, 0);
            libusb_close(device->dev);
        }
        device->dev = NULL;
    }
    if (device->libusb_initialized) {
        libusb_exit(NULL);
        device->libusb_initialized = false;
    }
}

static int send_command(Canalyst_Device *device, int channel, void *command, size_t cmd_len, 
                       void *response, size_t resp_len) {
    int ep = CHANNEL_TO_COMMAND_EP(channel);
    int actual;
    int r = libusb_bulk_transfer(device->dev, ep, (unsigned char *)command, cmd_len, &actual, 1000);
    if (r < 0 || actual != cmd_len) {
        fprintf(stderr, "Command write failed: %s\n", libusb_error_name(r));
        return -1;
    }

    if (response != NULL) {
        ep |= 0x80; // IN endpoint
        r = libusb_bulk_transfer(device->dev, ep, (unsigned char *)response, resp_len, &actual, 1000);
        if (r < 0 || actual != resp_len) {
            fprintf(stderr, "Response read failed: %s\n", libusb_error_name(r));
            return -1;
        }
    }
    return 0;
}

static int canalyst_init_channel(Canalyst_Device *device, int channel, int bitrate, 
                                uint8_t timing0, uint8_t timing1, int start) {
    if (bitrate == 0 && (timing0 == 0 || timing1 == 0)) {
        fprintf(stderr, "Either bitrate or both timing0/timing1 must be specified\n");
        return -1;
    }

    if (bitrate != 0) {
        if (get_timing_values(bitrate, &timing0, &timing1) < 0) {
            fprintf(stderr, "Unsupported bitrate: %d\n", bitrate);
            return -1;
        }
    }

    Canalyst_InitCommand cmd = {
        .command = COMMAND_INIT,
        .acc_code = 0x1,
        .acc_mask = 0xFFFFFFFF,
        .filter = 0x1,
        .timing0 = timing0,
        .timing1 = timing1,
        .unknown2 = 0x1
    };

    if (send_command(device, channel, &cmd, sizeof(cmd), NULL, 0) < 0) {
        return -1;
    }

    device->initialized[channel] = 1;
    
    if (start) {
        Canalyst_SimpleCommand start_cmd = { .command = COMMAND_START };
        if (send_command(device, channel, &start_cmd, sizeof(start_cmd), NULL, 0) < 0) {
            return -1;
        }
        device->started[channel] = 1;
    }

    return 0;
}

static int canalyst_start_channel(Canalyst_Device *device, int channel) {
    if (!device->initialized[channel]) {
        fprintf(stderr, "Channel %d not initialized\n", channel);
        return -1;
    }

    Canalyst_SimpleCommand cmd = { .command = COMMAND_START };
    if (send_command(device, channel, &cmd, sizeof(cmd), NULL, 0) < 0) {
        return -1;
    }
    device->started[channel] = 1;
    return 0;
}

static int canalyst_stop_channel(Canalyst_Device *device, int channel) {
    if (!device->initialized[channel]) {
        fprintf(stderr, "Channel %d not initialized\n", channel);
        return -1;
    }

    Canalyst_SimpleCommand cmd = { .command = COMMAND_STOP };
    if (send_command(device, channel, &cmd, sizeof(cmd), NULL, 0) < 0) {
        return -1;
    }
    device->started[channel] = 0;
    return 0;
}

static int canalyst_clear_rx_buffer(Canalyst_Device *device, int channel) {
    Canalyst_SimpleCommand cmd = { .command = COMMAND_CLEAR_RX_BUFFER };
    return send_command(device, channel, &cmd, sizeof(cmd), NULL, 0);
}

static int canalyst_get_message_status(Canalyst_Device *device, int channel, Canalyst_MessageStatusResponse *status) {
    Canalyst_SimpleCommand cmd = { .command = COMMAND_MESSAGE_STATUS };
    return send_command(device, channel, &cmd, sizeof(cmd), status, sizeof(*status));
}

static int canalyst_flush_tx_buffer(Canalyst_Device *device, int channel, int timeout_ms) {
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    Canalyst_MessageStatusResponse status;
    do {
        if (canalyst_get_message_status(device, channel, &status) < 0) {
            return -1;
        }
        
        if (status.tx_pending == 0) {
            return 0;
        }
        
        sleep_ms(10); // 10ms
        clock_gettime(CLOCK_MONOTONIC, &now);
    } while (timeout_ms == 0 || 
             ((now.tv_sec - start.tv_sec) * 1000 + (now.tv_nsec - start.tv_nsec) / 1000000 < timeout_ms));
    
    return -1; // Timeout
}

static int canalyst_receive(Canalyst_Device *device, int channel, Canalyst_CANMessage **messages, int *count) {
    if (!device->initialized[channel] || !device->started[channel]) {
        fprintf(stderr, "Channel %d not ready\n", channel);
        return -1;
    }

    Canalyst_MessageStatusResponse status;
    if (canalyst_get_message_status(device, channel, &status) < 0) {
        return -1;
    }

    if (status.rx_pending == 0) {
        *count = 0;
        return 0;
    }

    // Calculate buffer size
    int rx_buffer_num = (status.rx_pending + 2) / 3 + 1;
    int rx_buffer_size = rx_buffer_num * sizeof(Canalyst_CANMessageBuffer);
    Canalyst_CANMessageBuffer *buffers = malloc(rx_buffer_size);
    if (!buffers) {
        return -1;
    }

    int ep = CHANNEL_TO_MESSAGE_EP(channel) | 0x80;
    int actual;
    int r = libusb_bulk_transfer(device->dev, ep, (unsigned char *)buffers, rx_buffer_size, &actual, 1000);
    if (r < 0) {
        free(buffers);
        fprintf(stderr, "Receive failed: %s\n", libusb_error_name(r));
        return -1;
    }

    int num_buffers = actual / sizeof(Canalyst_CANMessageBuffer);
    int total_messages = 0;
    
    // Count total messages first
    for (int i = 0; i < num_buffers; i++) {
        total_messages += buffers[i].count;
    }

    if (total_messages == 0) {
        free(buffers);
        *count = 0;
        return 0;
    }

    *messages = malloc(total_messages * sizeof(Canalyst_CANMessage));
    if (!*messages) {
        free(buffers);
        return -1;
    }

    // Copy messages to output array
    int msg_idx = 0;
    for (int i = 0; i < num_buffers; i++) {
        for (int j = 0; j < buffers[i].count; j++) {
            (*messages)[msg_idx++] = buffers[i].messages[j];
        }
    }

    *count = total_messages;
    free(buffers);
    return 0;
}

static int canalyst_send(Canalyst_Device *device, int channel, Canalyst_CANMessage *messages, int count, int flush_timeout_ms) {
    if (!device->initialized[channel] || !device->started[channel]) {
        fprintf(stderr, "Channel %d not ready\n", channel);
        return -1;
    }

    int tx_buffer_num = (count + 2) / 3;
    Canalyst_CANMessageBuffer *buffers = calloc(tx_buffer_num, sizeof(Canalyst_CANMessageBuffer));
    if (!buffers) {
        return -1;
    }

    for (int i = 0; i < count; i++) {
        int buf_idx = i / 3;
        buffers[buf_idx].messages[i % 3] = messages[i];
        buffers[buf_idx].count++;
    }

    int ep = CHANNEL_TO_MESSAGE_EP(channel);
    int actual;
    int r = libusb_bulk_transfer(device->dev, ep, (unsigned char *)buffers, 
                               tx_buffer_num * sizeof(Canalyst_CANMessageBuffer), &actual, 1000);
    if (r < 0) {
        free(buffers);
        fprintf(stderr, "Send failed: %s\n", libusb_error_name(r));
        return -1;
    }

    free(buffers);
    
    if (flush_timeout_ms >= 0) {
        return canalyst_flush_tx_buffer(device, channel, flush_timeout_ms);
    }

    return 0;
}

char *canalyst_get_name()
{
    return "USBCAN/CANalyst-II";
}

bool canalyst_dev_scan(Canalyst_Device *device)
{
    if(canalyst_scan(device) > 0) {
        return true; // Device found
    } else {
        fprintf(stderr, "No Canalyst-II devices found\n");
        return false; // No device found
    }
}

bool canalyst_dev_init(Canalyst_Device *device, int bitrate)
{
    if(canalyst_init(device, 0) < 0) {
        fprintf(stderr, "Failed to initialize Canalyst-II device\n");
        return false; // Initialization failed
    }
    if(canalyst_init_channel(device, 0, bitrate, 0, 0, 1) < 0) {
        fprintf(stderr, "Failed to initialize channel 0\n");
        canalyst_close(device);
        return false; // Channel initialization failed
    }
    if(canalyst_start_channel(device, 0) < 0) {
        fprintf(stderr, "Failed to start channel 0\n");
        canalyst_close(device);
        return false; // Channel start failed
    }

    return true; // Initialization successful
}

bool canalyst_dev_close(Canalyst_Device *device)
{
    canalyst_close(device);
    return true; // Close successful
}

bool canalyst_dev_frame_send(Canalyst_Device *device, const Canalyst_CANMessage *msg)
{
    if (!device->initialized[0] || !device->started[0]) {
        fprintf(stderr, "Channel 0 not initialized or started\n");
        return false; // Channel not ready
    }

    Canalyst_CANMessage messages[] = {*msg};
    if (canalyst_send(device, 0, messages, 1, 100) < 0) {
        fprintf(stderr, "Failed to send CAN message\n");
        return false; // Send failed
    }
    return true; // Send successful
}

int canalyst_dev_frame_read(Canalyst_Device *device, Canalyst_CANMessage **msg, int *count)
{
    if (!device->initialized[0] || !device->started[0]) {
        fprintf(stderr, "Channel 0 not initialized or started\n");
        return -1; // Channel not ready
    }

    Canalyst_CANMessage *messages = NULL;
    int msg_count = 0;
    if (canalyst_receive(device, 0, &messages, &msg_count) < 0) {
        fprintf(stderr, "Failed to receive CAN messages\n");
        return -1; // Receive failed
    }

    *msg = messages;
    *count = msg_count;
    return 0; // Receive successful
}