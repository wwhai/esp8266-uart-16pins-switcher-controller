#include <Arduino.h>
//
//
//接线关系:
// GPIO4   ->  Pin4
// GPIO5   ->  Pin5
// 协议：|指令(0:开|1:关)|PIN编号列表(1-16)|//
// 例如开 1 2 3号开关: 0x00 0x01 0x02 0x03
//
//
enum
{
  GPIO0 = 0,
  GPIO4 = 1,
  GPIO5 = 2,

};
//
//
//
const size_t HEADER_LEN = 6;
// 开: C T R L 0x00 0x01 0x02 # ==> 43 54 52 4C 01 01 02 35
// 关: C T R L 0x01 0x01 0x02 # ==> 43 54 52 4C 01 01 02 35
//
//
// 第一个为 0,没意义,主要是为了实现C的数组下标1开始, 和板子对的上号
uint8_t switcher[3] = {0, 4, 5};
//
//  按理来说这里的状态得保存到EPROM里面去，但是现在我怕是没时间搞，现在只能搞demo了，等以后时间有了再说。
//
void readConfig()
{
  // TODO
}
void writeConfig()
{
  // TODO
}
void initGpio()
{
  // 初始化GPIO为输出模式
  pinMode(switcher[GPIO4], OUTPUT);
  pinMode(switcher[GPIO5], OUTPUT);
  digitalWrite(switcher[GPIO4], LOW);
  digitalWrite(switcher[GPIO5], LOW);
}
//
//
//
char terminator = '#';
// CTRL011#
const int bufferLength = 9;
char serialBuffer[bufferLength];

void flushBuffer()
{
  for (size_t i = 0; i < bufferLength; i++)
  {
    serialBuffer[i] = 0;
  }
  while (Serial.read() >= 0)
  {
  }
}
void showBanner()
{
  Serial1.println();
  Serial1.println();
  Serial1.println("-----------------------------------------------");
  Serial1.println("|     Esp8266 uart switch controller <'v'>    |");
  Serial1.println("|     Version: 0.0.1 Author: wwhai            |");
  Serial1.println("-----------------------------------------------");
  Serial1.println();
  Serial1.println();
}
/**
 * 同步状态
 * */
void syncState()
{
  for (size_t i = 1; i < 3; i++)
  {
    delay(10);
    Serial.print(digitalRead(switcher[i]));
  }
  Serial.print("#");
  Serial.flush();
}
void setup()
{
  Serial.begin(115200);
  initGpio();
  flushBuffer();
}

void loop()
{
  if (Serial.available())
  {
    delay(400);
    size_t len = Serial.readBytesUntil(terminator, serialBuffer, bufferLength);
    char header[5];
    sprintf(header, "%c%c%c%c", serialBuffer[0], serialBuffer[1], serialBuffer[2], serialBuffer[3]);
    // 获取状态
    if (strcmp(header, "....") == 0)
    {
      syncState();
      flushBuffer();
    }
    // 控制指令
    if (strcmp(header, "CTRL") == 0)
    {
      char cmd = serialBuffer[4];
      if (cmd == 0x00)
      {
        Serial1.printf("OFF:");
        //  要去了 '#'
        for (size_t i = HEADER_LEN - 1; i < len - 1; i++)
        {
          int index = (int)serialBuffer[i];
          if (index <= 2)
          {
            Serial1.printf("[%02x]", switcher[index]);
            digitalWrite(switcher[index], LOW);
          }
        }
      }
      if (cmd == 0x01)
      {
        Serial1.printf("ON:");
        //  要去了 '#'
        for (size_t i = HEADER_LEN - 1; i < len - 1; i++)
        {
          int index = (int)serialBuffer[i];
          if (index <= 2)
          {
            Serial1.printf("[%02x]", switcher[index]);
            digitalWrite(switcher[index], HIGH);
          }
        }
      }
      Serial1.flush();
      syncState();
      flushBuffer();
    }
  }
}