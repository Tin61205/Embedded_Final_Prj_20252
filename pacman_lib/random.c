#include "random.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_rcc.h"

/**
  * @brief  Khởi tạo peripheral RNG
  *         - Bật clock AHB2 cho RNG
  *         - Enable RNG
  * @param  None
  * @retval None
  */
void random_init(void)
{
    /* Bật clock cho RNG trên AHB2 */
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

    /* Kích hoạt RNG */
    RNG_Cmd(ENABLE);
}

/**
  * @brief  Kiểm tra RNG có sẵn sàng dữ liệu hay không, sau đó đọc 1 số 32-bit
  * @param  None
  * @retval uint32_t: Số ngẫu nhiên 32-bit
  */
uint32_t get_random_number32(void)
{
    /* Chờ cờ DRDY (Data Ready) = 1 */
    while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
    {
        /* Kiểm tra lỗi Clock Error hoặc Seed Error (tùy ý xử lý) */
        if (RNG_GetFlagStatus(RNG_FLAG_CECS) == SET)
        {
            // Clock error: thường do PLL48CLK chưa sẵn sàng
            // Clear cờ, hoặc reset RNG...
            RNG_ClearFlag(RNG_FLAG_CECS);
        }
        if (RNG_GetFlagStatus(RNG_FLAG_SECS) == SET)
        {
            // Seed error
            RNG_ClearFlag(RNG_FLAG_SECS);
            // Reset RNG
            RNG_Cmd(DISABLE);
            RNG_Cmd(ENABLE);
        }
    }

    /* Đọc thanh ghi DR => giá trị random 32-bit */
    return RNG_GetRandomNumber();
}

/**
  * @brief  Lấy 1 bit ngẫu nhiên (true/false)
  * @param  None
  * @retval bool: true hoặc false
  */
bool get_random_bit(void)
{
    /* Lấy 32-bit, rồi lấy LSB */
    uint32_t rand_val = get_random_number32();
    bool bit_val = (rand_val & 0x01) ? true : false;
    return bit_val;
}

/**
  * @brief  Lấy số ngẫu nhiên trong khoảng [min, max]
  * @note   Nếu min > max, hàm sẽ trả về min (tuỳ ý bạn xử lý)
  * @param  min: giá trị nhỏ nhất
  * @param  max: giá trị lớn nhất
  * @retval uint32_t: Số ngẫu nhiên nằm trong [min, max]
  */
uint32_t get_randrange(uint32_t min, uint32_t max)
{
    if (min > max)
    {
        // Xử lý trường hợp min > max: tuỳ ứng dụng
        // Ở đây ta cứ trả về min
        return min;
    }

    if (min == max)
    {
        // Nếu cùng giá trị => chỉ có 1 kết quả
        return min;
    }

    // Tính khoảng
    uint32_t range = (max - min) + 1;

    // Lấy random 32-bit
    uint32_t rnd32 = get_random_number32();

    // Lấy modulo -> [0..(range-1)]
    uint32_t val = rnd32 % range;

    // Dời lên min -> [min..max]
    return (val + min);
}
