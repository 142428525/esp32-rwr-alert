#include <Ticker.h>
#include <U8g2lib.h>
#include "consts.h"

enum BuzType {
  BUZ_NONE,
  BUZ_RADAR,
  BUZ_LOCK,
  BUZ_MSL
};

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI oled(U8G2_R0, 5, 33, 32);
bool blink_flag = false;
int8_t blink_cnt = 0;

uint16_t touch0_old = 0;
uint16_t touch0 = 0;
uint16_t touch1_old = 0;
uint16_t touch1 = 0;
BuzType buz_ty = BUZ_NONE;
TaskHandle_t buz_task = NULL;
Ticker buz_ticker;

void do_buz(void*);
void ledc_buz(double, uint32_t);
void stop_buz();

//void buz(int, int, int);

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println("successfully initialized the serial");
  
  pinMode(BUZ_PIN, OUTPUT);
  ledcAttachPin(BUZ_PIN, BUZ_CHAN);
  
  oled.begin();
  oled.enableUTF8Print();
  oled.setFont(u8g2_font_wqy12_t_gb2312);

  randomSeed(analogRead(RANDSEED_PIN));
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //buz(BUZ_PIN, 1136, 1000);
  //delay(1000);
  //buz(BUZ_PIN, 1012, 500);
  //buz(BUZ_PIN, 1515, 500);
  //delay(1000);

  touch0_old = touch0;
  touch0 = touchRead(TOUCH0_PIN);
  Serial.println(touch0);
  
  if (touch0_old < TOUCH_GATE && touch0 < TOUCH_GATE) { // 触发了
    blink_flag = true;
    
    touch1_old = touch1;
    touch1 = touchRead(TOUCH1_PIN);
    // GPIO 4, 13 touched = 敌导弹
    // GPIO 4 touched = 敌跟踪
    buz_ty = touch1 < TOUCH_GATE ? BUZ_MSL : BUZ_LOCK;

    if (buz_task == NULL) {
      blink_cnt = 0;
      
      xTaskCreate(do_buz, "do_buz_task", 1024, NULL, 1, &buz_task);
      Serial.print("successfully created the task for ");
      Serial.println(buz_ty);
    }
  } else if (buz_task != NULL && buz_ty != BUZ_RADAR) {
    stop_buz();
  } else if (random(100) < 1) { // 空闲时1%随机
    blink_flag = true;
    blink_cnt = 0;
    buz_ty = BUZ_RADAR;

    if (buz_task == NULL) {
      xTaskCreate(do_buz, "do_buz_task", 1024, NULL, 1, &buz_task);
      Serial.print("successfully created the task for ");
      Serial.println(buz_ty);
    }
  }
  
  oled.firstPage();
  do {
    oled.setCursor(0, 16);
    oled.print(touch0);
    
    if (blink_flag && blink_cnt >= 0) {
      oled.drawDisc(96, 31, 27);
      oled.setDrawColor(0);
      oled.drawDisc(96, 31, 19);
      oled.setDrawColor(1);
      for (uint8_t i = SLSH_POSDELI; i < SLSH_POSLEN; i++) {
        oled.drawXBM(SLSH_LX[i], SLSH_LY[i], SLSH_W, SLSH_H, LSLSH_BITS);
        oled.drawXBM(SLSH_RX[i], SLSH_RY[i], SLSH_W, SLSH_H, RSLSH_BITS);
      }
      switch (buz_ty) {
        case BUZ_LOCK:
          oled.drawButtonUTF8(12, 45, U8G2_BTN_BW2, 0, 2, 3, "敌跟踪");
          break;
          
        case BUZ_MSL:
          oled.drawButtonUTF8(12, 45, U8G2_BTN_BW2, 0, 2, 3, "敌导弹");
          break;
          
        default:
          break;
      }
    }
    
    for (uint8_t i = 0; i < SLSH_POSDELI; i++) {
      oled.drawXBM(SLSH_LX[i], SLSH_LY[i], SLSH_W, SLSH_H, LSLSH_BITS);
      oled.drawXBM(SLSH_RX[i], SLSH_RY[i], SLSH_W, SLSH_H, RSLSH_BITS);
    }
    oled.drawCircle(96, 31, 29);
    oled.drawCircle(96, 31, 4);
    oled.drawHLine(94, 31, 5);
    oled.drawVLine(96, 29, 5);
    oled.drawHLine(68, 31, 3);
    oled.drawHLine(122, 31, 3);
    oled.drawVLine(96, 3, 3);
    oled.drawVLine(96, 57, 3);
  } while (oled.nextPage());
  
  if (blink_flag && ++blink_cnt >= 4) {
    blink_cnt = -blink_cnt;
  }
  
  delay(50);
}

void do_buz(void* params) {
  for (;;) {
    switch (buz_ty) {
      case BUZ_RADAR:
        buz_ticker.once_ms(100, stop_buz);
        ledc_buz(NOTE_MIDL, 101);
        break;

      case BUZ_LOCK:
        ledc_buz(NOTE_HIGH, 215);
        ledc_buz(NOTE_LOW1, 215);
        break;

      case BUZ_MSL:
        ledc_buz(NOTE_HIGH, 83);
        ledc_buz(NOTE_LOW2, 83);
        break;

      default:
        Serial.print("buz: 0 ");
        //ledc_buz(0, 50);
        //ledcWrite(BUZ_CHAN, 0);
        break;
    }
  }
}

void ledc_buz(double freq, uint32_t delay_time) {
  ledcSetup(BUZ_CHAN, freq, 10);
  ledcWrite(BUZ_CHAN, 1);
  vTaskDelay(delay_time / portTICK_PERIOD_MS);
  ledcWrite(BUZ_CHAN, 0);
}

void stop_buz() {
  buz_ty = BUZ_NONE;
  ledcWrite(BUZ_CHAN, 0); // 必须在ledcSetup()执行后才能执行，否则会神秘地通过多任务调度队列崩系统
  vTaskDelete(buz_task);
  buz_task = NULL;
  blink_flag = false;
  blink_cnt = 0;
}

/*void buz(int pin_p, int delay_us_p, int milli_p)
{
  for (int i = 500 * milli_p / delay_us_p; i > 0; i--)
  {
    digitalWrite(pin_p, HIGH);
    delayMicroseconds(delay_us_p);
    digitalWrite(pin_p, LOW);
    delayMicroseconds(delay_us_p);
  }
}*/
