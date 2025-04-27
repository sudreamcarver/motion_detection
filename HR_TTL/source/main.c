#include "OLED.h"
#include "max30102.h"
#include "serial.h"
#include <Key.h>
#include <delay.h>
#include <stdint.h>

#define MAX_BRIGHTNESS 255
#define INTERRUPT_REG 0x00

uint32_t aun_ir_buffer[500]; // IR LED 红外光数据，用于计算血氧
int32_t n_ir_buffer_length;  // 数据长度

uint32_t aun_red_buffer[500]; // RED 红光数据，用于计算心率
int32_t n_heart_rate;         // 心率值
int8_t ch_heartRate_valid;    // 用于显示心率计算是否有效的指示符

int32_t n_spo2;       // SPO2值
int8_t ch_spo2_valid; // 用于显示SPO2是否有效的指示符

uint32_t un_min, un_max, un_prev_data; // restore min and max signal
int i;                                 // index
int32_t n_brightness;
float f_temp;

uint8_t temp[6];
uint8_t str[100];
uint8_t dis_hr = 0, dis_spo2 = 0;

int main(void) {
  OLED_Init();
  serial_init();
  MAX30102_Init();

  un_min = 0x3FFFF;
  un_max = 0;

  OLED_ShowString(1, 1, "HeartRate:");
  OLED_ShowString(2, 1, "SPO2:");

  n_ir_buffer_length = 500; // 缓冲区长度为100,可存储以100sps运行的5秒样本
                            // 读取500个样本，并确定信号范围

  for (i = 0; i < n_ir_buffer_length; i++) {
    while (MAX30102_INT == 1)
      ;

    max30102_FIFO_ReadBytes(REG_FIFO_DATA, temp);
    aun_red_buffer[i] = (long)((long)((long)temp[0] & 0x03) << 16) |
                        (long)temp[1] << 8 | (long)temp[2];
    aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03) << 16) |
                       (long)temp[4] << 8 | (long)temp[5];

    // 替换最值
    if (un_min > aun_red_buffer[i])
      un_min = aun_red_buffer[i];
    if (un_max < aun_red_buffer[i])
      un_max = aun_red_buffer[i];
  }

  un_prev_data = aun_red_buffer[i];

  maxim_heart_rate_and_oxygen_saturation(
      aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2,
      &ch_spo2_valid, &n_heart_rate, &ch_heartRate_valid);

  while (1) {
    for (i = 100; i < 500; i++) {
      // 将数据前推100位
      aun_red_buffer[i - 100] = aun_red_buffer[i];
      aun_ir_buffer[i - 100] = aun_ir_buffer[i];

      // update the signal main and max
      if (un_min > aun_red_buffer[i])
        un_min = aun_red_buffer[i];
      if (un_max < aun_red_buffer[i])
        un_max = aun_red_buffer[i];
    }

    for (i = 400; i < 500; i++) {
      un_prev_data = aun_red_buffer[i - 1];
      while (MAX30102_INT == 1)
        ;
      max30102_FIFO_ReadBytes(REG_FIFO_DATA, temp);
      aun_red_buffer[i] = (long)((long)((long)temp[0] & 0x03) << 16) |
                          (long)temp[1] << 8 | (long)temp[2];
      aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03) << 16) |
                         (long)temp[4] << 8 | (long)temp[5];
      if (aun_red_buffer[i] > un_prev_data) {
        f_temp = aun_red_buffer[i] - un_prev_data;
        f_temp /= (un_max - un_min);
        f_temp *= MAX_BRIGHTNESS;
        n_brightness -= (int)f_temp;
        if (n_brightness < 0)
          n_brightness = 0;
      } else {
        f_temp = un_prev_data - aun_red_buffer[i];
        f_temp /= (un_max - un_min);
        f_temp *= MAX_BRIGHTNESS;
        n_brightness += (int)f_temp;
        if (n_brightness > MAX_BRIGHTNESS)
          n_brightness = MAX_BRIGHTNESS;
      }

      if (ch_heartRate_valid == 1 && n_heart_rate < 120) {
        dis_hr = n_heart_rate;
        dis_spo2 = n_spo2;
      } else {
        dis_hr = 0;
        dis_spo2 = 0;
      }
    }
    maxim_heart_rate_and_oxygen_saturation(
        aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2,
        &ch_spo2_valid, &n_heart_rate, &ch_heartRate_valid);

    if (dis_hr == 0) {
      OLED_ShowNum(1, 11, 000, 3);
    } else {
      OLED_ShowNum(1, 11, dis_hr - 20, 3);
    }

    OLED_ShowNum(2, 6, dis_spo2, 3);

    if (dis_hr == 0) {
      serial_sendstring("HeartRate:");
      serial_sendnumber(000, 3);
      serial_sendstring("\n\r");

    } else {
      serial_sendstring("HeartRate:");
      serial_sendnumber(dis_hr - 20, 3);
      serial_sendstring("\n\r");
    }

    serial_sendstring("SPO2:");
    serial_sendnumber(dis_spo2, 3);
    serial_sendstring("\n\r");
  }
}
