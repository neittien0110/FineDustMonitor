#ifndef MAIN_H
#define MAIN_H

#define _DEBUG_

// Thông tin phiên bản
#define FIRMWARE_VERSION "2.2"
#define DEVICE_NAME      "AIR MONITOR"

// Các cấu hình ngưỡng hiển thị (Nếu cần dùng chung)
#define MAX_SAMPLES      16
#define MAX_AQI_VALUE    64

// Thông tin địa chỉ MAC giả để vượt captive
#define MAC_SPOOFING_ADDRESS "24:24:B7:D0:B6:1C"

// Quản lý wifi
extern bool wifiStatus; // Trạng thái kết nối: true = connected

// --- NÚT BẤM -----
#define CFG_BUTTON D7

#endif