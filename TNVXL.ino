// Blynk config
#define BLYNK_TEMPLATE_ID "TMPL6VsJDyxpQ"
#define BLYNK_TEMPLATE_NAME "V"
#define BLYNK_AUTH_TOKEN "cC3YVVGGhZg9P6mFrYc9A3OmsGzI8sNH"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <esp_timer.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Wifi config
char ssid[] = "";
char pass[] = "";

// LDR
#define LDR_PIN 3

// LED RGB
#define RED_PIN 4
#define GREEN_PIN 5
#define BLUE_PIN 6

// DHT11
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Button
#define BUTTON_PIN 1
volatile int mode = 1; // 1 = Full, 2 = Sleep, 3 = Warning
// Debouncing
volatile unsigned long lastPressed = 0;
const unsigned long debounceDelay = 200;

// OLED SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SDA 19
#define SCL 18
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Timer (esp_timer)
esp_timer_handle_t periodicTimer;
volatile bool printSensor = false;

// Callback esp_timer
void onTimer(void *arg)
{
  printSensor = true;
}

void IRAM_ATTR changeMode()
{
  unsigned long currentTime = millis();
  if (currentTime - lastPressed > debounceDelay)
  {
    mode++;
    if (mode > 3)
      mode = 1;
    lastPressed = currentTime;
  }
}

// BLYNK Button
BLYNK_WRITE(V3)
{
  int value = param.asInt();
  if (value == 1)
  {
    mode++;
    if (mode > 3)
      mode = 1;
    Serial.print("Chuyen mode (tu app): ");
    Serial.println(mode);
  }
}

void setup()
{
  Serial.begin(115200);
  delay(500);
  dht.begin();

  // WiFi + Blynk
  WiFi.begin(ssid, pass);
  Serial.print("Connecting WiFi");
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected!");
    Blynk.config(BLYNK_AUTH_TOKEN);

    // Kết nối Blynk 10 giây
    if (Blynk.connect(10000))
    {
      Serial.println("Blynk connected!");
    }
    else
    {
      Serial.println("Blynk connection failed, running offline mode.");
    }
  }
  else
  {
    Serial.println();
    Serial.println("WiFi connection failed! Running offline mode.");
  }

  // LED RGB
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Button + Interrupt
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), changeMode, FALLING);

  // OLED I2C
  Wire.begin(SDA, SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("Khong tim thay OLED!"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Smart Env Monitor");
  display.display();
  delay(1000);

  // Tạo và start timer period 1 giây bằng esp_timer
  const esp_timer_create_args_t timer_args = {
      .callback = &onTimer,
      .arg = NULL,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "periodic_sensor"};

  if (esp_timer_create(&timer_args, &periodicTimer) != ESP_OK)
  {
    Serial.println("esp_timer_create failed");
  }
  else
  {
    // 1,000,000 ms = 1 second
    esp_timer_start_periodic(periodicTimer, 1000000);
  }
}

void setColor(int r, int g, int b)
{
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED && Blynk.connected())
  {
    Blynk.run();
  }

  float h = dht.readHumidity();
  float tC = dht.readTemperature();
  int ldrValue = analogRead(LDR_PIN);
  int lightpercent = map(ldrValue, 0, 4095, 100, 0);

  if (isnan(h) || isnan(tC))
  {
    Serial.println("Khong doc duoc DHT11!");
  }

  if (printSensor)
  {
    printSensor = false;
    Serial.print("Nhiet do: ");
    Serial.print(tC);
    Serial.print(" *C, Do am: ");
    Serial.print(h);
    Serial.print(" %, Anh sang: ");
    Serial.print(lightpercent);
    Serial.println(" %");
    Blynk.virtualWrite(V0, tC);
    Blynk.virtualWrite(V1, h);
    Blynk.virtualWrite(V2, lightpercent);
  }

  // OLED
  display.clearDisplay();
  display.setTextSize(1);

  if (mode == 1)
  {
    setColor(0, 0, 0);
    display.setCursor(0, 0);
    display.println("Smart Env Monitor");
    display.setCursor(0, 16);
    display.print("Temp: ");
    display.print(tC, 1);
    display.println(" C");
    display.setCursor(0, 32);
    display.print("Humi: ");
    display.print(h, 1);
    display.println(" %");
    display.setCursor(0, 48);
    display.print("Light: ");
    display.print(lightpercent, 1);
    display.println(" %");
  }

  else if (mode == 2)
  {
    display.clearDisplay();
    display.setCursor((128 - 80) / 2, 0);
    display.println("SLEEP MODE");

    display.setTextSize(1);
    display.setCursor(10, 25);
    display.print("Light: ");
    display.print(lightpercent);
    display.println(" %");

    display.setTextSize(1);
    display.setCursor(10, 40);
    display.print("STATUS: ");
    if (lightpercent < 10)
    {
      display.println("DARK");
      setColor(0, 0, 102);
    }
    else
    {
      display.println("BRIGHT");
      setColor(0, 0, 0);
    }
  }
  else if (mode == 3)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor((128 - 80) / 2, 0);
    display.println("WARNING MODE");

    // Hiển thị nhiệt độ
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.print("T: ");
    display.print(tC, 1);
    display.print("C");

    // RGB
    if (tC < 25)
    { // Xanh lam
      setColor(0, 0, 255);
    }
    else if (tC < 26)
    { // Xanh lam nhạt
      setColor(102, 178, 255);
    }
    else if (tC < 27)
    { // Xanh ngọc
      setColor(0, 255, 255);
    }
    else if (tC < 28)
    { // Xanh lục
      setColor(0, 255, 128);
    }
    else if (tC < 29)
    { // Xanh vàng
      setColor(128, 255, 0);
    }
    else if (tC < 30)
    { // Vàng
      setColor(255, 255, 0);
    }
    else if (tC < 31)
    { // Cam
      setColor(255, 128, 0);
    }
    else if (tC < 35)
    { // Đỏ
      setColor(255, 0, 0);
    }
    else
    { // >= 35°C
      static unsigned long lastBlink = 0;
      static bool ledState = false;
      unsigned long now = millis();
      if (now - lastBlink >= 200)
      {
        lastBlink = now;
        ledState = !ledState;
        if (ledState)
        {
          setColor(255, 0, 0);
        }
        else
        {
          setColor(0, 0, 0);
        }
      }
    }

    // Thông báo trạng thái
    display.setTextSize(1);
    display.setCursor(10, 50);
    if (tC < 25)
    {
      display.print("Status: COOL");
    }
    else if (tC < 28)
    {
      display.print("Status: NORMAL");
    }
    else if (tC < 31)
    {
      display.print("Status: WARM");
    }
    else if (tC < 35)
    {
      display.print("Status: HOT!");
    }
    else
    {
      display.print("Status: OVERHEAT!");
    }
  }
  display.display();
  delay(200);
}