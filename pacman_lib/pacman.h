#ifndef __STM32F4_UB_PACMAN_H
#define __STM32F4_UB_PACMAN_H

#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32_ub_lcd_ili9341.h"
#include "stm32_ub_graphic2d.h"
#include "maze.h"
#include "maze_generate.h"
#include "gui.h"
#include "stm32_ub_font.h"
#include "stm32_ub_systick.h"
#include "menu.h"

//--------------------------------------------------------------
// Cấu hình TIMER và NVIC cho TIM7
// Tần số = 84 MHz / (prescaler+1) / (period+1) = 1kHz (1ms)
//--------------------------------------------------------------
#define TILED_TIMER_NAME         TIM7
#define TILED_TIMER_CLK          RCC_APB1Periph_TIM7
#define TILED_TIMER_PRESCALE     41
#define TILED_TIMER_PERIOD       1999
#define TILED_TIMER_IRQ          TIM7_IRQn
#define TILED_TIMER_IRQHANDLER   TIM7_IRQHandler

//--------------------------------------------------------------
// Thời gian chế độ game (đơn vị: 100ms)
//--------------------------------------------------------------
#define GAME_MODE_TIMER      100 /**< Chu kỳ timer cơ sở (100ms) */
#define GAME_CHASE_TIME      200 /**< Thời gian Chase (đuổi bắt): 20 giây */
#define GAME_FRIGHTENED_TIME 60  /**< Thời gian Frightened (hoảng sợ): 6 giây */
#define GAME_FRIGHTENED_BLINK 20  /**< Thời gian nhấp nháy báo hết hoảng sợ: 2 giây */

#define GAME_POINTS_NORMAL   10  /**< Điểm ăn chấm tròn thường */
#define GAME_POINTS_ENERGY   100 /**< Điểm ăn chấm sức mạnh */

#define GAME_FRIGHTENED_START_POINTS 300 /**< Điểm ăn ghost đầu tiên */
#define GAME_FRIGHTENED_STEP_POINTS  300 /**< Lượng điểm tăng thêm cho ghost tiếp theo */

//--------------------------------------------------------------
// Trạng thái game
//--------------------------------------------------------------
#define GAME_RUN         0 /**< Game đang chạy */
#define GAME_PLAYER_WIN  1 /**< Người chơi thắng (vượt màn) */
#define GAME_PLAYER_LOSE 2 /**< Người chơi thua (mất mạng) */
#define GAME_OVER        3 /**< Game over (hết mạng) */
#define GAME_EXIT        4 /**< Thoát game */

//--------------------------------------------------------------
// Bitmask điều khiển di chuyển các thực thể
//--------------------------------------------------------------
#define MOVE_NOBODY      0x00 /**< Không thực thể nào di chuyển */
#define MOVE_PLAYER      0x01 /**< Bit 0: Di chuyển Người chơi 1 */
#define MOVE_GHOST0      0x02 /**< Bit 1: AI ghost slot 0 */
#define MOVE_GHOST1      0x04 /**< Bit 2: AI ghost slot 1 */
#define MOVE_GHOST2      0x08 /**< Bit 3: AI ghost slot 2 */
#define MOVE_GHOST3      0x10 /**< Bit 4: AI ghost slot 3 */
#define MOVE_PLAYER2     0x20 /**< Bit 5: Di chuyển Người chơi 2 */
#define MOVE_HUMAN_GHOST 0x40 /**< Bit 6: Di chuyển Ghost do người điều khiển */

/* Legacy mask aliases */
#define MOVE_BLINKY MOVE_GHOST0
#define MOVE_PINKY  MOVE_GHOST1
#define MOVE_INKY   MOVE_GHOST2
#define MOVE_CLYDE  MOVE_GHOST3

#define MOVE_GHOST_ALL (MOVE_GHOST0 | MOVE_GHOST1 | MOVE_GHOST2 | MOVE_GHOST3)

//--------------------------------------------------------------
// Hằng số logic
//--------------------------------------------------------------
#define BOOL_FALSE 0
#define BOOL_TRUE  1

#define GAME_CONTROL_TOUCH   0 /**< Điều khiển bằng màn hình cảm ứng */
#define GAME_CONTROL_4BUTTON 1 /**< Điều khiển bằng 4 nút nhấn cứng */

//--------------------------------------------------------------
// Lựa chọn chế độ và cấu hình
//--------------------------------------------------------------
#define GAME_MODE_CHASE   1 /**< Chế độ đuổi bắt (ghost đuổi pacman) */

#define GAME_PLAY_CAMPAIGN 0 /**< Chế độ chơi chiến dịch */
#define GAME_PLAY_CUSTOM   1 /**< Chế độ chơi tùy chọn tự cấu hình */

#define CUSTOM_PLAYER_1 1 /**< Chơi đơn tùy chọn */
#define CUSTOM_PLAYER_2 2 /**< Chơi đôi tùy chọn */

#define CUSTOM_2P_COOP      0 /**< 2 người chơi hợp tác (co-op) */
#define CUSTOM_2P_VS_GHOST  1 /**< 2 người đối kháng (Pacman vs Ghost) */

#define CUSTOM_SPEED_SLOW   0 /**< Tốc độ tùy chọn: Chậm */
#define CUSTOM_SPEED_NORMAL 1 /**< Tốc độ tùy chọn: Bình thường */
#define CUSTOM_SPEED_FAST   2 /**< Tốc độ tùy chọn: Nhanh */

#define PLAYER_SPEED_SLOW_MS   40 /**< Khoảng thời gian bước đi tốc độ chậm (40ms) */
#define PLAYER_SPEED_NORMAL_MS 30 /**< Khoảng thời gian bước đi tốc độ thường (30ms) */
#define PLAYER_SPEED_FAST_MS   20 /**< Khoảng thời gian bước đi tốc độ nhanh (20ms) */

#define CUSTOM_MAX_GHOSTS 4 /**< Số ghost tối đa trong chế độ custom */


// Cấu hình game custom
typedef struct {
    uint32_t player_count;                     /**< Số lượng người chơi (1 hoặc 2) */
    uint32_t map_id;                           /**< ID của bản đồ được chọn */
    uint32_t player_speed_idx;                 /**< Chỉ mục tốc độ của người chơi */
    uint32_t ghost_speed_idx;                  /**< Chỉ mục tốc độ của ghost */
    uint32_t two_player_mode;                  /**< Chế độ chơi 2 người (COOP hoặc VS) */
    uint32_t ghost_count;                      /**< Số lượng ghost trong game */
    uint32_t ghost_strategies[CUSTOM_MAX_GHOSTS]; /**< Chỉ mục chiến thuật AI cho từng ghost */
}
CustomConfig_t;


// Độ khó chế độ chiến dịch
typedef struct {
    uint32_t ghost_count;   /**< Số lượng ghost hoạt động */
    uint32_t active_mask;    /**< Mask kích hoạt các ghost */
    uint32_t ghost_speed;   /**< Tốc độ di chuyển của ghost (ms/bước) */
    uint32_t strategies[4]; /**< Chỉ mục chiến thuật cho 4 ghost */
}
CampaignDifficulty_t;


// Cấu trúc trạng thái chính của game
typedef struct {
    uint32_t collision;                           /**< Trạng thái va chạm */
    uint32_t controller;                          /**< Bộ điều khiển được dùng (Cảm ứng/Nút nhấn) */
    uint32_t mode;                                /**< Chế độ hoạt động hiện tại (Chase) */
    uint32_t mode_timer;                          /**< Bộ đếm thời gian chế độ hiện tại */
    uint32_t frightened;                          /**< Trạng thái hoảng sợ của ghost */
    uint32_t frightened_timer;                    /**< Thời gian còn lại của hoảng sợ */
    uint32_t frightened_points;                   /**< Điểm cộng cho lần ăn ghost kế tiếp */
    uint32_t numberOfBots;                        /**< Số lượng ghost do AI điều khiển */
    uint32_t play_type;                           /**< Chế độ chơi (Campaign hoặc Custom) */
    uint32_t ghost_active_mask;                   /**< Mask các ghost đang hoạt động */
    uint32_t player2_joy;                         /**< Chọn joystick điều khiển cho Người chơi 2 */
    uint32_t player2_active;                      /**< Cờ kiểm tra Người chơi 2 có hoạt động hay không */
    uint32_t campaign_map_id;                     /**< ID bản đồ chiến dịch hiện tại */
    uint32_t campaign_difficulty;                 /**< Chỉ mục độ khó chiến dịch */
    uint32_t campaign_coop;                       /**< Cờ chơi co-op trong chiến dịch */
    CustomConfig_t custom;                        /**< Cấu hình chế độ chơi tùy chọn */
    uint32_t campaign_high_scores[MAZE_MAP_COUNT]; /**< Điểm số kỷ lục của các map chiến dịch */
}
Game_t;

extern Game_t Game;


void pacman_start(void);

/**
 * @brief Áp dụng cấu hình độ khó cho chế độ chiến dịch.
 */
void pacman_apply_campaign_difficulty(uint32_t mode);

/**
 * @brief Khởi tạo phần cứng (Timer, NVIC) cho game.
 * @return 0 nếu thành công.
 */
uint32_t pacman_hw_init(void);


//Khởi tạo và làm mới các biến trạng thái game.
void pacman_init(uint32_t mode);

//Áp dụng cấu hình chơi tùy chọn cho game.
void pacman_apply_custom_config(uint32_t mode);

/**
 * @brief Vòng lặp chơi game chính (thực thi mỗi chu kỳ).
 * @return Trạng thái game sau chu kỳ (GAME_RUN, GAME_PLAYER_WIN, GAME_PLAYER_LOSE...)
 */
uint32_t pacman_play(void);

/**
 * @brief Giảm các bộ đếm thời gian chế độ game (gọi định kỳ).
 */
void pacman_dec_mode_timer(void);

/**
 * @brief Callback ngắt Timer định kỳ mỗi 1ms.
 */
void UB_Tiled_1ms_ISR_CallBack(void); 

/**
 * @brief Callback cập nhật và xử lý thiết bị điều khiển.
 */
void UB_Tiled_Input_Device_CallBack(void); 

/**
 * @brief Callback xử lý quét và vẽ đồ họa LCD.
 */
void UB_Tiled_LCD_CallBack(void); 

/**
 * @brief Callback xử lý các sự kiện logic game phát sinh.
 */
void UB_Tiled_Event_CallBack(void); 

#endif // __STM32F4_UB_PACMAN_H