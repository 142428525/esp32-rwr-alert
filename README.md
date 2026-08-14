# 基于ESP32+Arduino环境的RWR复刻
仓库内代码在（软件条件）Windows 7 + Arduino 1.8.x + ESP32 Arduino Core 1.0.6 + （硬件条件）普通ESP32开发板下编译烧录成功。

代码引用了FreeRTOS API、U8g2库、Ticker库。请保证这些依赖库存在。

OLED屏幕为SH1106，硬件SPI。若有不同，请自行修改U8g2初始化器。

开发板引脚参数在`consts.h`。若有不同，请自行修改。

若有其他方面的不同导致无法成功运行，还请自行调试。
