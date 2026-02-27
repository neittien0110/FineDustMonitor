#include <LittleFS.h>
#include "WifiManager.h"
#include "WiFiSelfEnroll.h"     // Quản lý đăng kí tài khoản WiFi bằng self hosting
#include "WiFiEnrollBySerial.h" // Quản lý đăng kí tài khoản WiFi bằng lệnh serial
#include "configmanager.h"
#include "main.h"

bool wifiStatus = false;
unsigned long lastWifiCheck = 0;

/**
 * @brief Hàm kiểm tra xem mạng thực sự có Internet hay không. 
 * Chỉ thỉnh thoảng mới dùng để đỡ tốn năng lực.
 * @return true 
 * @return false 
 */
bool isInternetReady() {
    if (WiFi.status() != WL_CONNECTED) return false;
    
    // Thử phân giải tên miền của MQTT Broker hoặc Google
    IPAddress remote_ip;
    if (WiFi.hostByName("toolhub.app", remote_ip) == 1) {
        return true; // Có DNS nghĩa là có Internet thực
    }
    return false;
}

void CheckAndEstablishWiFiConnection(unsigned long interval)
{
    // 1. Nếu WiFi bị vô hiệu hóa
    if (!configMgr.params.wifiEnabled)
    {
        if (wifiStatus)
        { // Nếu đang kết nối thì ngắt đi
            WiFi.disconnect();
            wifiStatus = false;
            Serial.println(F("WiFi Disabled. Disconnected."));
        }
        return; // Thoát luôn, không kiểm tra interval hay kết nối lại
    }

    // 2. Logic kiểm tra WiFi bình thường khi wifiEnabled == true
    unsigned long currentMillis = millis();
    wifiStatus = (WiFi.status() == WL_CONNECTED);
#if defined(_DEBUG_)    
    //Serial.print("Trạng thái wifi - CheckAndEstablishWiFiConnection: ");
    //Serial.println(wifiStatus?"Ok":"Dis");    
#endif

    if (currentMillis - lastWifiCheck >= interval || lastWifiCheck == 0)
    {
        lastWifiCheck = currentMillis;

        if (!wifiStatus)
        {
            WiFi.mode(WIFI_STA);
            WiFi.begin(configMgr.params.ssid.c_str(), configMgr.params.password.c_str());
            Serial.println(F("Attempting to reconnect WiFi..."));
        }
    }
}

void WakeupWiFi()
{
#ifdef _DEBUG_
    Serial.println("[WiFi] Waking up WiFi...");
#endif

#if defined(ARDUINO_ARCH_ESP8266)
    // Đánh thức ESP8266 khỏi chế độ forceSleep
    WiFi.forceSleepWake();
    delay(1);
#endif

    // Khởi động lại chế độ Station
    WiFi.mode(WIFI_STA);

    // Ghi nhớ đã bật wifi
    configMgr.setWifiEnabled(true);
}

void ShutdownWiFi()
{
#ifdef _DEBUG_
    Serial.println("[WiFi] Shutting down WiFi to save power...");
#endif

    WiFi.disconnect(true); // Ngắt kết nối và xóa cấu hình tạm thời
    WiFi.mode(WIFI_OFF);   // Tắt chip Radio RF

#if defined(ARDUINO_ARCH_ESP8266)
    WiFi.forceSleepBegin(); // Lệnh đặc biệt cho ESP8266 để ngủ sâu hơn
#endif

    // Ghi nhớ đã tắt wifi
    configMgr.setWifiEnabled(false);
}

/** Quản lý cấu hình wifi động*/
WiFiSelfEnroll *MyWiFi = nullptr;

bool RegisterWiFi(WIFI_REGISTRATION_METHODS method)
{
    switch (method)
    {
    case WIFI_REGISTRATION_METHODS::SELF_STATION:
    {
        if (MyWiFi == nullptr)
        {
            MyWiFi = new WiFiSelfEnroll();
        }
        // Kích hoạt trạm phát AP để user vào cài đặt
        MyWiFi->setup();
        return true;
    }
    case WIFI_REGISTRATION_METHODS::SERIAL_INTERFACE:
    {
        return EnrollBySerial(); // Gọi hàm từ module mới
    }
    }
    return false;
}

extern "C" {
  #include <user_interface.h>
}

/**
 * @brief Thay đổi địa chỉ MAC của ESP8266
 * @param newMacStr Chuỗi MAC định dạng "XX:XX:XX:XX:XX:XX", viết ở hệ 16
 * @note Phải thiết lập trước khi kết nối WiFi, và chỉ cần làm 1 lần. Khi WiFi bị mất và cần kết nôi lại, không cần set lại MAC.
 */
void SetNewMac(const char* newMacStr) {
    // 1. Kiểm tra xem chuỗi có rỗng hoặc null không
    if (newMacStr == NULL || strlen(newMacStr) == 0) {
        Serial.println("[MAC] No new MAC provided. Using default.");
        return;
    }

    // 2. Chuyển đổi chuỗi "AA:BB..." thành mảng 6 byte
    uint8_t mac[6];
    int values[6];
    
    // Sử dụng sscanf để tách chuỗi hex
    if (sscanf(newMacStr, "%x:%x:%x:%x:%x:%x", 
        &values[0], &values[1], &values[2], 
        &values[3], &values[4], &values[5]) == 6) 
    {
        for (int i = 0; i < 6; i++) {
            mac[i] = (uint8_t)values[i];
        }

        // 3. Thực hiện gán (Phải gọi trước WiFi.begin())
        WiFi.mode(WIFI_STA); 
        if (wifi_set_macaddr(STATION_IF, mac)) {
            Serial.printf("[MAC] Changed to: %s\n", newMacStr);
        } else {
            Serial.println("[MAC] Set failed!");
        }
    } else {
        Serial.println("[MAC] Invalid format! Use XX:XX:XX:XX:XX:XX");
    }
}