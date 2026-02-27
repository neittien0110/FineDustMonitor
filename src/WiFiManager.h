#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
    #include <WiFi.h>
#elif  defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
#endif

/// @brief the default wifi SSID
#define WIFI_SSID_NAME "SOICT_CORE_BOARD"
/// @brief the default wifi password
#define WIFI_SSID_PASS "12345678"

// Cấu hình thời gian kiểm tra WiFi (1 phút)
#define WIFI_CHECK_INTERVAL 60000 

enum WIFI_REGISTRATION_METHODS {
    SELF_STATION,
    SERIAL_INTERFACE,
};
 
/**
 * @brief Kết nối lại WiFi nếu cần. Nhưng sẽ bỏ qua nếu @wifiEnabled = false, 
 *        hoặc chưa quá khoảng thời gian interval
 * @param interval 
 * @see wifiEnabled 
 */
void CheckAndEstablishWiFiConnection(unsigned long interval = WIFI_CHECK_INTERVAL);

/**
 * @brief Kiểm tra khả năng kết nối internet thực sự. Hàm bị blocking và có khả năng 
 *        chiếm nhiều thời gian. Hạn chế sử dụng
 * @return true 
 * @return false 
 */
bool isInternetReady();

void WakeupWiFi();
void ShutdownWiFi();

/**
 * @brief Đăng ký thông tin WiFi dựa trên phương thức được chọn.
 * @param method Phương thức đăng ký (SELF_STATION: Web Captive Portal, SERIAL_INTERFACE: Qua Serial)
 * @return bool Trả về kết quả thành công hoặc thất bại của quá trình đăng ký.
 */
bool RegisterWiFi(WIFI_REGISTRATION_METHODS method = WIFI_REGISTRATION_METHODS::SELF_STATION);

/**
 * @brief Thay đổi địa chỉ MAC của ESP8266
 * @param newMacStr Chuỗi MAC định dạng "XX:XX:XX:XX:XX:XX", viết ở hệ 16
 * @note Phải thiết lập trước khi kết nối WiFi, và chỉ cần làm 1 lần. Khi WiFi bị mất và cần kết nôi lại, không cần set lại MAC.
 */
void SetNewMac(const char* newMacStr);

#endif