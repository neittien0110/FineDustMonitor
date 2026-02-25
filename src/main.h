#ifndef MAIN_H
#define MAIN_H

// Thông tin phiên bản
#define FIRMWARE_VERSION "2.0.1"
#define DEVICE_NAME      "AIR MONITOR"

// Các cấu hình ngưỡng hiển thị (Nếu cần dùng chung)
#define MAX_SAMPLES      16
#define MAX_AQI_VALUE    64

// Quản lý wifi
extern bool wifiStatus; // Trạng thái kết nối: true = connected

// --- NÚT BẤM -----
#define CFG_BUTTON D7

#endif