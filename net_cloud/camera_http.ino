#include <CRtspSession.h>
#include <CStreamer.h>
#include <OV2640.h>
#include <OV2640Streamer.h>
#include <platglue-arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>

// WiFi配置
const char* ssid = "LAPT_21";
const char* password = "88884444";

// RTSP服务器配置
#define RTSP_PORT 554                  // 标准RTSP端口
#define FRAME_INTERVAL 150             // 降低帧率（150ms≈6fps），减轻ESP32压力
#define MAX_CLIENTS 1                  // 仅支持1个客户端（避免多连接冲突）

// 摄像头对象
OV2640 cam;

// RTSP服务器和会话对象（简化管理，避免复杂指针生命周期问题）
WiFiServer rtspServer(RTSP_PORT);
CRtspSession* rtspSession = nullptr;
OV2640Streamer* streamer = nullptr;
WiFiClient client;  // 直接用全局对象管理客户端，避免栈指针失效

// 摄像头初始化函数（保持原逻辑，确保兼容性）
bool initCamera() {
  // 使用AI Thinker预设配置（库原生支持，避免引脚错误）
  camera_config_t config = esp32cam_aithinker_config;
  
  // 降低图像参数，减少数据量（关键！避免传输超时）
  config.frame_size = FRAMESIZE_QQVGA; // 160x120（比QVGA更小，传输更快）
  config.jpeg_quality = 18;            // 0-63，值越大压缩率越高（体积越小）
  config.fb_count = 1;                 // 单缓冲区，减少内存占用
  config.xclk_freq_hz = 16000000;      // 降低时钟频率，减少硬件负载
  
  // 初始化摄像头
  esp_err_t err = cam.init(config);
  if (err != ESP_OK) {
    Serial.printf("摄像头初始化失败: 0x%x\n", err);
    return false;
  }
  
  // 调整摄像头参数（简化设置，避免兼容性问题）
  sensor_t *s = esp_camera_sensor_get();
  s->set_brightness(s, 0);    // 亮度默认
  s->set_contrast(s, 0);      // 对比度默认
  s->set_whitebal(s, 1);      // 自动白平衡（必开，避免偏色）
  
  Serial.println("摄像头初始化成功");
  return true;
}

// 网络初始化函数（保持原逻辑）
bool initWiFi() {
  Serial.print("连接WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // 等待连接（延长超时时间至15秒）
  uint8_t retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 30) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi连接失败");
    return false;
  }
  
  Serial.println("\nWiFi连接成功");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());
  return true;
}

// 清理会话资源（简化版，避免未定义成员访问）
void cleanupSession() {
  if (rtspSession != nullptr && rtspSession->m_stopped) {
    // 直接释放会话和流对象（不访问内部socket，避免成员错误）
    delete streamer;
    delete rtspSession;
    client.stop(); // 关闭客户端连接
    streamer = nullptr;
    rtspSession = nullptr;
    Serial.println("客户端断开，资源已清理");
  }
}

void setup() {
  // 初始化串口（波特率115200，用于调试）
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("ESP32-CAM RTSP服务器启动中...");
  
  // 初始化摄像头（失败则死循环）
  if (!initCamera()) {
    while (1) {
      delay(1000);
      Serial.println("等待摄像头初始化...");
    }
  }
  
  // 初始化网络（失败则死循环）
  if (!initWiFi()) {
    while (1) {
      delay(1000);
      Serial.println("等待WiFi连接...");
    }
  }
  
  // 启动RTSP服务器
  rtspServer.begin();
  Serial.printf("RTSP服务器启动，端口: %d\n", RTSP_PORT);
  Serial.printf("RTSP流地址: rtsp://%s:%d/mjpeg/1\n", 
               WiFi.localIP().toString().c_str(), RTSP_PORT);
}

void loop() {
  // 先清理已断开的会话
  cleanupSession();
  
  // 处理新客户端连接（仅允许1个客户端，避免冲突）
  if (rtspSession == nullptr) {
    client = rtspServer.accept();
    if (client) {
      Serial.print("新客户端连接: ");
      Serial.println(client.remoteIP().toString());
      
      // 关键：用客户端对象地址创建SOCKET（匹配库的构造函数参数）
      SOCKET clientSocket = &client;
      
      // 创建流和会话对象（严格匹配库的接口）
      streamer = new OV2640Streamer(clientSocket, cam);
      String hostPort = WiFi.localIP().toString() + ":" + String(RTSP_PORT);
      streamer->setURI(hostPort, "mjpeg", "1");
      
      rtspSession = new CRtspSession(clientSocket, streamer);
      Serial.println("客户端连接成功，开始推流");
    }
  }
  
  // 处理RTSP请求和推流（仅当会话存在时执行）
  if (rtspSession != nullptr && !rtspSession->m_stopped) {
    // 处理客户端请求（超时时间设为10ms，避免阻塞）
    rtspSession->handleRequests(10);
    
    // 按帧率推流（控制发送频率，避免数据堆积）
    static uint32_t lastFrameTime = 0;
    uint32_t currentTime = millis();
    if (currentTime - lastFrameTime >= FRAME_INTERVAL) {
      rtspSession->broadcastCurrentFrame(currentTime);
      lastFrameTime = currentTime;
    }
  }
  
  // 短暂延迟，降低CPU占用
  delay(1);
}