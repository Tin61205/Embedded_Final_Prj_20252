#ifndef __RANDOM_H
#define __RANDOM_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include <stdbool.h>

/**
  * @brief  Khởi tạo peripheral RNG (bật clock, enable RNG)
  * @param  None
  * @retval None
  */
void random_init(void);

/**
  * @brief  Lấy một số ngẫu nhiên 32-bit (hardware RNG)
  * @param  None
  * @retval uint32_t: Số ngẫu nhiên 32-bit
  */
uint32_t get_random_number32(void);

/**
  * @brief  Lấy 1 bit ngẫu nhiên (true/false)
  * @param  None
  * @retval bool: true hoặc false, tùy bit ngẫu nhiên
  */
bool get_random_bit(void);

/**
  * @brief  Lấy số ngẫu nhiên trong khoảng [min, max]
  * @param  min: giá trị nhỏ nhất
  * @param  max: giá trị lớn nhất
  * @retval uint32_t: Số ngẫu nhiên trong khoảng [min, max]
  */
uint32_t get_randrange(uint32_t min, uint32_t max);

#ifdef __cplusplus
}
#endif

#endif /* __RANDOM_H */
