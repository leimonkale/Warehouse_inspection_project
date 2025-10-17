#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>  // ESP32 专用异步TCP库
#include <Wire.h>

// -------------------------- 基础配置（无中文，避免编码问题） --------------------------
const char* WIFI_SSID = "LAPT_21";    // WiFi名称（无中文）
const char* WIFI_PASS = "88884444";   // WiFi密码
#define UART_TX_PIN 17                 // 串口发送引脚（输出1234）
#define UART_RX_PIN 16                 // 串口接收引脚（可悬空）
#define UART_BAUD 115200               // 串口波特率
#define WEB_PORT 80                    // Web服务器端口

// -------------------------- OV2640 引脚定义（严格对应接线） --------------------------
// 控制引脚
#define OV2640_RST_GPIO  12   // 复位引脚
#define OV2640_PWDN_GPIO 13   // 电源控制引脚
// I2C引脚（配置寄存器）
#define OV2640_SDA_GPIO  21   // I2C数据
#define OV2640_SCL_GPIO  22   // I2C时钟
// 视频数据引脚（读取图像）
#define OV2640_D0_GPIO   34   // 并行数据0
#define OV2640_D1_GPIO   35   // 并行数据1
#define OV2640_D2_GPIO   32   // 并行数据2
#define OV2640_D3_GPIO   33   // 并行数据3
#define OV2640_D4_GPIO   25   // 并行数据4
#define OV2640_D5_GPIO   26   // 并行数据5
#define OV2640_D6_GPIO   27   // 并行数据6
#define OV2640_D7_GPIO   14   // 并行数据7
#define OV2640_PCLK_GPIO 15   // 像素时钟
#define OV2640_HREF_GPIO 4    // 行同步
#define OV2640_VSYNC_GPIO 2   // 场同步

// -------------------------- OV2640 核心参数 --------------------------
#define OV2640_I2C_ADDR 0x30  // 默认I2C地址
#define JPEG_BUF_SIZE   64000 // 图像缓冲区（QVGA足够）
uint8_t jpeg_buf[JPEG_BUF_SIZE];  // 存储JPEG数据

// -------------------------- 全局对象 --------------------------
AsyncWebServer server(WEB_PORT);  // Web服务器
AsyncWebSocket ws("/ws");         // WebSocket（传视频/指令）

// -------------------------- OV2640 初始化寄存器（QVGA+JPEG+30fps） --------------------------
const uint16_t ov2640_init_regs[][2] = {
  {0x0030, 0x01},  // 系统复位
  {0x0030, 0x00},  // 结束复位
  {0x0012, 0x04},  // 输出格式：JPEG
  {0x0040, 0x01},  // 分辨率：QVGA（320x240）
  {0x000C, 0x00},  // HREF极性
  {0x000D, 0x00},  // VSYNC极性
  {0x0017, 0x22},  // 帧率：~30fps
  {0xFFFF, 0xFF}   // 配置结束标记
};

// ==========================================================================================
// 1. OV2640 底层驱动（无库依赖）
// ==========================================================================================
void ov2640_i2c_init() {
  Wire.begin(OV2640_SDA_GPIO, OV2640_SCL_GPIO);
  Wire.setClock(100000);  // 100kHz I2C时钟
  delay(10);
}

void ov2640_write_reg(uint16_t reg, uint8_t data) {
  Wire.beginTransmission(OV2640_I2C_ADDR);
  Wire.write((reg >> 8) & 0xFF);  // 寄存器高8位
  Wire.write(reg & 0xFF);         // 寄存器低8位
  Wire.write(data);               // 写入数据
  Wire.endTransmission();
  delay(1);
}

void ov2640_ctrl_gpio_init() {
  pinMode(OV2640_RST_GPIO, OUTPUT);
  pinMode(OV2640_PWDN_GPIO, OUTPUT);
  
  // 复位时序
  digitalWrite(OV2640_PWDN_GPIO, LOW);
  digitalWrite(OV2640_RST_GPIO, LOW);
  delay(10);
  digitalWrite(OV2640_RST_GPIO, HIGH);
  delay(100);
}

void ov2640_video_gpio_init() {
  // 数据引脚（D0-D7）设为输入
  pinMode(OV2640_D0_GPIO, INPUT);
  pinMode(OV2640_D1_GPIO, INPUT);
  pinMode(OV2640_D2_GPIO, INPUT);
  pinMode(OV2640_D3_GPIO, INPUT);
  pinMode(OV2640_D4_GPIO, INPUT);
  pinMode(OV2640_D5_GPIO, INPUT);
  pinMode(OV2640_D6_GPIO, INPUT);
  pinMode(OV2640_D7_GPIO, INPUT);
  
  // 控制引脚设为输入
  pinMode(OV2640_PCLK_GPIO, INPUT);
  pinMode(OV2640_HREF_GPIO, INPUT);
  pinMode(OV2640_VSYNC_GPIO, INPUT);
}

bool ov2640_init() {
  ov2640_ctrl_gpio_init();
  ov2640_i2c_init();
  
  // 写入初始化寄存器
  for (int i = 0;; i++) {
    uint16_t reg = ov2640_init_regs[i][0];
    uint8_t data = ov2640_init_regs[i][1];
    if (reg == 0xFFFF && data == 0xFF) break;
    ov2640_write_reg(reg, data);
  }
  
  ov2640_video_gpio_init();
  Serial.println("OV2640初始化成功");
  return true;
}

uint32_t ov2640_read_jpeg() {
  uint32_t len = 0;
  // 等待新帧开始（VSYNC电平变化）
  while (digitalRead(OV2640_VSYNC_GPIO) == HIGH);
  while (digitalRead(OV2640_VSYNC_GPIO) == LOW);
  
  // 读取一帧数据
  while (digitalRead(OV2640_VSYNC_GPIO) == HIGH && len < JPEG_BUF_SIZE) {
    if (digitalRead(OV2640_HREF_GPIO) == HIGH) {
      while (digitalRead(OV2640_PCLK_GPIO) == LOW);  // 等待PCLK上升沿
      
      // 组合D0-D7数据
      uint8_t data = 0;
      data |= (digitalRead(OV2640_D7_GPIO) << 7);
      data |= (digitalRead(OV2640_D6_GPIO) << 6);
      data |= (digitalRead(OV2640_D5_GPIO) << 5);
      data |= (digitalRead(OV2640_D4_GPIO) << 4);
      data |= (digitalRead(OV2640_D3_GPIO) << 3);
      data |= (digitalRead(OV2640_D2_GPIO) << 2);
      data |= (digitalRead(OV2640_D1_GPIO) << 1);
      data |= (digitalRead(OV2640_D0_GPIO) << 0);
      
      jpeg_buf[len++] = data;
      while (digitalRead(OV2640_PCLK_GPIO) == HIGH);  // 等待PCLK下降沿
    }
  }
  
  // 截取完整JPEG帧（0xFFD8开头，0xFFD9结尾）
  uint8_t* start = NULL;
  uint8_t* end = NULL;
  for (uint32_t i = 0; i < len - 1; i++) {
    if (jpeg_buf[i] == 0xFF && jpeg_buf[i+1] == 0xD8) start = &jpeg_buf[i];
    if (jpeg_buf[i] == 0xFF && jpeg_buf[i+1] == 0xD9) end = &jpeg_buf[i+1];
  }
  
  if (start && end && end > start) {
    len = end - start + 1;
    memmove(jpeg_buf, start, len);
    return len;
  } else {
    return 0;
  }
}

// ==========================================================================================
// 2. WebSocket 处理
// ==========================================================================================
void on_ws_event(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket客户端 %u 连接\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket客户端 %u 断开\n", client->id());
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->opcode == WS_TEXT) {
      data[len] = '\0';
      String cmd = (char*)data;
      
      // 串口输出对应数字
      if (cmd == "forward") { Serial1.print("1"); Serial.println("输出1（前）"); }
      else if (cmd == "back") { Serial1.print("2"); Serial.println("输出2（后）"); }
      else if (cmd == "left") { Serial1.print("3"); Serial.println("输出3（左）"); }
      else if (cmd == "right") { Serial1.print("4"); Serial.println("输出4（右）"); }
    }
  }
}

// JPEG转Base64发送到网页
void send_jpeg_to_web(uint8_t *buf, uint32_t len) {
  const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String base64_str = "";
  
  for (uint32_t i = 0; i < len; i += 3) {
    uint32_t triple = 0;
    for (int j = 0; j < 3; j++) {
      if (i + j < len) triple |= (buf[i+j] << (16 - j*8));
    }
    base64_str += base64_table[(triple >> 18) & 0x3F];
    base64_str += base64_table[(triple >> 12) & 0x3F];
    base64_str += (i + 1 < len) ? base64_table[(triple >> 6) & 0x3F] : '=';
    base64_str += (i + 2 < len) ? base64_table[triple & 0x3F] : '=';
  }
  
  ws.textAll(base64_str);
}

// ==========================================================================================
// 3. 网页HTML
// ==========================================================================================
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 OV2640 Control</title>
  <style>
    body { text-align: center; font-family: Arial; margin: 20px; }
    #video { border: 3px solid #333; border-radius: 8px; max-width: 100%; }
    button { font-size: 24px; width: 150px; height: 80px; margin: 15px; 
             background: #4CAF50; color: white; border: none; border-radius: 8px; cursor: pointer; }
    button:hover { background: #45a049; }
  </style>
</head>
<body>
  <h1>OV2640 Live Video</h1>
  <img id="video" src="" alt="Live Video">
  
  <h2>Direction Control</h2>
  <div><button onclick="sendCmd('forward')">Forward (1)</button></div>
  <div>
    <button onclick="sendCmd('left')">Left (3)</button>
    <button onclick="sendCmd('right')">Right (4)</button>
  </div>
  <div><button onclick="sendCmd('back')">Back (2)</button></div>

  <script>
    let ws = new WebSocket('ws://' + window.location.host + '/ws');
    ws.onmessage = e => { document.getElementById('video').src = 'data:image/jpeg;base64,' + e.data; };
    ws.onclose = () => { setTimeout(() => window.location.reload(), 3000); };
    function sendCmd(cmd) { ws.send(cmd); }
  </script>
</body>
</html>
)rawliteral";

// ==========================================================================================
// 4. 主函数
// ==========================================================================================
void setup() {
  // 初始化串口
  Serial.begin(115200);
  Serial1.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
  delay(100);
  Serial.println("Serial initialized");

  // 初始化OV2640
  if (!ov2640_init()) {
    Serial.println("OV2640 init failed! Check wiring");
    while (1) delay(1000);
  }

  // 连接WiFi
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());

  // 配置Web服务
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send_P(200, "text/html", INDEX_HTML);
  });
  ws.onEvent(on_ws_event);
  server.addHandler(&ws);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  ws.cleanupClients();  // 清理无效客户端

  // 读取并发送视频（~10fps）
  static unsigned long last_frame = 0;
  if (millis() - last_frame > 100) {
    uint32_t jpeg_len = ov2640_read_jpeg();
    if (jpeg_len > 0) send_jpeg_to_web(jpeg_buf, jpeg_len);
    else Serial.println("No valid JPEG frame");
    last_frame = millis();
  }

  delay(10);
}