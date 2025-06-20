# Using Canalyst-II on Linux Without Sudo

To use the Canalyst-II device on Linux without requiring `sudo`, you'll need to set up proper udev rules. Here's how to modify the Linux implementation to work without root privileges:

## 1. Create a udev rule file

Create a new file at `/etc/udev/rules.d/99-canalyst.rules` with the following content:

```bash
# Canalyst-II USB-CAN adapter
SUBSYSTEM=="usb", ATTR{idVendor}=="04d8", ATTR{idProduct}=="0053", MODE="0666", GROUP="plugdev"
```

This rule will:
- Match the Canalyst-II device by its vendor/product IDs
- Set permissions to 0666 (read/write for all)
- Assign it to the "plugdev" group (common group for pluggable devices)

## 2. Reload udev rules

After creating the file, run:

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## 3. Add your user to the plugdev group

```bash
sudo usermod -aG plugdev $USER
```

You'll need to log out and back in for this change to take effect.

## Key Changes for Non-root Operation:

1. **No Special Permissions Needed**:
   - The udev rules grant access to the device for all users or specific groups
   - No need for `sudo` when running the program

2. **Kernel Driver Handling**:
   - Added code to detach kernel driver if it's active
   - This prevents conflicts between userspace and kernel drivers

3. **Better Error Reporting**:
   - Added libusb error names to help diagnose permission issues

4. **Cleaner Resource Management**:
   - Properly handles cases where device opening or interface claiming fails

## Verification Steps:

1. After setting up the udev rules, verify the device permissions:
   ```bash
   ls -l /dev/bus/usb/*/*
   ```
   Look for your device (04d8:0053) and verify it has the correct permissions.

2. Test with a simple program that just opens the device before implementing the full protocol.

3. If you still have issues, you can enable libusb debug logging by uncommenting the `libusb_set_option` line.

## Troubleshooting:

If you still get permission errors:
1. Double-check the udev rules file for typos
2. Verify your user is in the `plugdev` group with `groups` command
3. Try rebooting to ensure all changes take effect
4. Check system logs with `dmesg` or `journalctl` for USB-related errors

This approach provides secure access to the device without requiring root privileges while maintaining all functionality of the original implementation.