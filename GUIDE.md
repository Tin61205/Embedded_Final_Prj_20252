# Hướng dẫn lắp ngoại vi — Game Pacman STM32F429

Tài liệu này mô tả **cách kết nối phần cứng** cho dự án Pacman trên kit **STM32F429I-Discovery**. Đọc kỹ phần **xung đột chân** trước khi hàn dây.

---

## 1. Kit và MCU

| Thành phần | Thông số |
|---|---|
| Board | **STM32F429I-Discovery** |
| MCU | **STM32F429ZIT6** (Cortex-M4 + FPU) |
| Màn hình | ILI9341 2.4" RGB, **240×320** pixel |
| Cảm ứng | Không sử dụng (Đã gỡ bỏ) |
| Thư viện | STM32 **Standard Peripheral Library (SPL)** |

Các ngoại vi **gắn sẵn trên board** (không cần nối thêm): LCD, SDRAM, nút User (PA0), cổng USB OTG HS, ST-Link debug.

---

## 2. Tổng quan ngoại vi cần lắp

| Ngoại vi | Bắt buộc? | Nối thêm? | Mục đích |
|---|---|---|---|
| **4 nút hướng** (UP/RIGHT/DOWN/LEFT) | Khuyến nghị | Có — hàn lên header | Player 1 điều khiển Pacman (thay thế bằng joystick) |
| **Joystick analog** (KY-023) | Khuyến nghị | Có — hàn lên header | Player 1 điều khiển Pacman qua ADC |
| **Nút BACK (PC1)** | Khuyến nghị | Có — hàn lên header | Quay lại menu chính (Campaign / Custom setup) |
| **Nút CENTER (PA0)** | Có | Không — có sẵn trên kit | Xác nhận / Start |
| **USB Keyboard** | Tùy chọn | Có — cắm cáp USB | Player 2 (mode Custom 2 người) |
| **UART (PA9/PA10)** | Tùy chọn | Có — USB-TTL | Debug serial 115200 baud |

> **Lưu ý:** Game hỗ trợ **joystick analog** (module KY-023) hoặc **4 nút GPIO** riêng lẻ. Có thể dùng một trong hai — ưu tiên nút GPIO nếu cả hai đều được lắp.

---

## 3. Nút bấm (4 hướng + Back + Start)

### 3.1. Sơ đồ đấu dây

```text
                    STM32F429 Discovery
                    ┌─────────────────────┐
                    │                     │
         PC2 ───────┤ UP                  │
         PC3 ───────┤ RIGHT               │
         PC4 ───────┤ DOWN   ⚠ xem mục 8 │
         PC5 ───────┤ LEFT                │
         PC1 ───────┤ BACK                │
         PA0 ───────┤ CENTER (nút xanh)   │
                    │                     │
         GND ───────┤ GND chung           │
                    └─────────────────────┘

Mỗi nút hướng / Back (PC1–PC5):
    [Chân GPIO] ────[ Nút nhấn ]──── GND
```

### 3.2. Bảng chân chi tiết

| Tên trong code | Chức năng | Chân GPIO | Đấu dây | Mức khi nhấn |
|---|---|---|---|---|
| `BTN_UP` | Lên | **PC2** | 1 đầu → PC2, 1 đầu → GND | **LOW** (0) |
| `BTN_RIGHT` | Phải | **PC3** | 1 đầu → PC3, 1 đầu → GND | **LOW** (0) |
| `BTN_DOWN` | Xuống | **PC11** | 1 đầu → PC11, 1 đầu → GND | **LOW** (0) |
| `BTN_LEFT` | Trái | **PC5** | 1 đầu → PC5, 1 đầu → GND | **LOW** (0) |
| `BTN_BACK` | Quay lại menu | **PC1** | 1 đầu → PC1, 1 đầu → GND | **LOW** (0) |
| `BTN_CENTER` | Chọn / Start | **PA0** | Nút **User** màu xanh trên kit | **HIGH** (1) |

### 3.3. Loại nút nhấn khuyến nghị

- Nút nhấn momentary (nhấn giữ), kiểu **thường mở** (NO).
- Không cần điện trở kéo ngoài — firmware bật **pull-up nội** cho PC1–PC5.
- Dùng dây Dupont hoặc hàn trực tiếp lên hàng chân **CN5 / CN6** (GPIO header) của Discovery.

### 3.4. Vị trí file cấu hình

```text
ub_lib/stm32_ub_button.c   ← bảng chân GPIO thực tế
ub_lib/stm32_ub_button.h   ← tên enum BTN_UP, BTN_RIGHT, ...
```

Nếu đổi chân, sửa mảng `BUTTON[]` trong `stm32_ub_button.c`.

### 3.5. Debounce

- TIM7, chu kỳ ~**50 ms** — tránh nút bị nhảy / nhấn đúp.
- Hàm đọc trong game: `UB_Button_Read()`, `UB_Button_OnClick()`.

### 3.6. Joystick analog (module KY-023)

Dùng **một module joystick 2 trục** thay cho 4 nút hướng riêng lẻ. Firmware đọc trục X/Y qua **ADC1** và chuyển thành hướng UP/RIGHT/DOWN/LEFT.

#### 3.6.1. Sơ đồ đấu dây

```text
              Joystick KY-023                STM32F429 Discovery
              ┌─────────────┐                ┌─────────────────────┐
              │ VCC ────────┼────────────────┤ 3.3V                │
              │ GND ────────┼────────────────┤ GND                 │
              │ VRx ────────┼────────────────┤ PA1 (ADC1_IN1)      │
              │ VRy ────────┼────────────────┤ PA2 (ADC1_IN2)      │
              │ SW  ────────┼─ (tùy chọn)    │ không dùng trong FW │
              └─────────────┘                └─────────────────────┘
```

#### 3.6.2. Bảng chân

| Tín hiệu module | Chân GPIO | Mô tả |
|---|---|---|
| **VRx** (trục X) | **PA1** | ADC1 kênh 1 — Trái/Phải |
| **VRy** (trục Y) | **PA2** | ADC1 kênh 2 — Lên/Xuống |
| **VCC** | **3.3V** | Không dùng 5V |
| **GND** | **GND** | Mass chung |
| **SW** | — | Không map trong firmware (dùng **PA0** để Start) |

#### 3.6.3. Cấu hình phần mềm

```text
ub_lib/stm32_ub_joystick.c   ← khởi tạo ADC + đọc trục
ub_lib/stm32_ub_joystick.h   ← ngưỡng deadzone / threshold
pacman_lib/gui.c             ← gui_check_joystick()
```

- Bật/tắt joystick: `JOYSTICK_USE_ADC` trong `stm32_ub_joystick.h` (`1` = bật).
- Điều chỉnh độ nhạy: `JOY_ADC_DEADZONE`, `JOY_ADC_THRESHOLD`.
- Nếu hướng bị ngược (Lên/Xuống đổi chỗ), đổi dây VRx ↔ VRy hoặc chỉnh logic trong `stm32_ub_joystick.c`.

#### 3.6.4. Thứ tự ưu tiên đầu vào (khi chơi)

1. **4 nút GPIO** (PC2–PC5 hoặc PC11 cho nút DOWN)
2. **Joystick analog** (PA1/PA2)

---

## 4. Màn hình cảm ứng (Touch — STMPE811) - Không sử dụng

Để tối ưu hóa hiệu năng hệ thống, bộ thư viện điều khiển cảm ứng `stm32_ub_touch_stmpe811` và bus giao tiếp `stm32_ub_i2c3` đã được **gỡ bỏ hoàn toàn** khỏi mã nguồn của dự án. Mọi thao tác chọn lựa và điều hướng nhân vật hiện được thực hiện độc quyền qua phím vật lý hoặc Joystick.

---

## 5. LCD và SDRAM (có sẵn trên kit)

LCD **không cần lắp thêm**. Hệ thống đồ họa gồm 3 phần:

### 5.1. SPI5 — cấu hình chip ILI9341

| Tín hiệu | Chân |
|---|---|
| SCK | PF7 |
| MOSI | PF9 |
| MISO | PF8 |
| CS | **PC2** |
| D/C (WRX) | PD13 |

### 5.2. LTDC — xuất hình RGB

Điều khiển song song các chân màu R/G/B, HSYNC, VSYNC, CLK, DE (xem comment đầu file `ub_lib/stm32_ub_lcd_ili9341.c`).

### 5.3. SDRAM — framebuffer

- Bus FMC, base `0xD0000000`
- Độ phân giải hiển thị: **240×320**, RGB565

---

## 6. USB Keyboard (Player 2)

### 6.1. Cổng cắm

| Thông số | Giá trị |
|---|---|
| Cổng trên kit | **USB OTG HS** (micro-AB, cạnh board) |
| Chế độ | USB **Host** (board cấp nguồn cho thiết bị) |
| DM | PB14 |
| DP | PB15 |
| VBUS enable | **PC4** (công tắc nguồn 5 V cho thiết bị USB) |

### 6.2. Cách lắp

1. Dùng cáp **USB A → micro-AB** (hoặc hub có nguồn nếu bàn phím tiêu thụ cao).
2. Cắm **bàn phím USB** vào cổng **OTG HS** trên Discovery.
3. Khởi động game — firmware tự nhận keyboard HID.

### 6.3. Phím điều khiển (Player 2)

| Phím | Mã HID trong code | Hướng |
|---|---|---|
| Mũi tên lên | 83 | UP |
| Mũi tên xuống | 84 | DOWN |
| Mũi tên trái | 79 | LEFT |
| Mũi tên phải | 89 | RIGHT |

Định nghĩa trong `pacman_lib/gui.c` → `gui_check_keyboard()`.

### 6.4. Khi nào dùng keyboard?

Chỉ trong **Custom → Mode: 2 người**:

| Sub-mode | Player 1 | Player 2 |
|---|---|---|
| **Pacman Co-op** | Nút / Joystick | Keyboard |
| **Pacman vs Ghost** | Nút / Joystick (Pacman) | Keyboard (điều khiển Blinky) |

---

## 7. UART Debug (tùy chọn)

| Tín hiệu | Chân | Baud |
|---|---|---|
| TX | **PA9** | 115200 |
| RX | **PA10** | 115200 |

Nối module **USB-TTL** (3.3 V):

```text
PA9 (TX board) ──→ RX (USB-TTL)
PA10 (RX board) ←── TX (USB-TTL)
GND ─────────────── GND
```

Mặc định debug UART **tắt** (`USE_GUI_UART_DEBUG = 0` trong `gui.h`).

---

## 8. ⚠ Xung đột chân — đọc trước khi hàn

### 8.1. PC2 — UP vs LCD Chip Select

| Chức năng | Dùng PC2 |
|---|---|
| Nút **BTN_UP** | GPIO input |
| LCD **SPI CS** | GPIO output |

Cùng một chân. Trên Discovery, PC2 **nối cứng với LCD**. Nút UP ngoài có thể gây nhiễu lúc khởi tạo LCD. Sau khi game chạy, đọc nút UP vẫn thường hoạt động.

**Khuyến nghị:** Ưu tiên **touch D-pad** cho hướng Lên nếu nút PC2 không ổn định.

### 8.2. PC4 — DOWN vs USB VBUS

| Chức năng | Dùng PC4 |
|---|---|
| Mạch **USB Host VBUS** | GPIO output (bật/tắt 5 V cho keyboard) |

**Xung đột nghiêm trọng** khi dùng **USB keyboard**:
- Khi cắm keyboard, firmware điều khiển PC4 làm **ngõ ra** (VBUS switch).
- Do đó, nút DOWN của Player 1 đã được cấu hình lại để sử dụng chân **PC11** (thay vì PC4) nhằm tránh xung đột phần cứng.

**Khuyến nghị đấu dây:**
1. Đấu dây nút DOWN vật lý vào chân **PC11** trên kit STM32F429.
2. Hoặc sử dụng **Joystick analog** để đi xuống.

### 8.3. Chân đã bận — không dùng cho nút ngoài

| Chân | Lý do |
|---|---|
| PC12 | LCD / SDRAM |
| PC14, PC15 | Thạch anh 32.768 kHz (LSE) |
| PA1, PA2 | Joystick analog (ADC1) |
| PA8, PC9 | Chân trống (đã tắt I2C3 touch) |
| PB14, PB15 | USB OTG |
| PF7–PF9 | SPI5 LCD |

---

## 9. Thứ tự khởi tạo phần cứng

Khi bật nguồn, `main()` gọi `pacman_hw_init()` theo thứ tự:

```text
1. UB_Systick_Init()      → SysTick 1 ms
2. UB_USB_HID_HOST_Init() → USB Host OTG HS
3. UB_Uart_Init()         → USART1 (nếu dùng debug)
4. UB_Button_Init()       → PC1–PC5, PA0 + TIM7 debounce
5. UB_Joystick_Init()     → PA1/PA2 ADC1 (nếu bật)
6. UB_LCD_Init()          → SPI5 + ILI9341 + SDRAM + LTDC
7. gui_clear_screen()
```

---

## 10. Cách điều khiển trong game

### 10.1. Menu chính

| Thao tác | Cách làm |
|---|---|
| Chọn mục | Nút UP / DOWN hoặc Joystick |
| Xác nhận | Nút CENTER (PA0) |

### 10.2. Campaign / Custom setup

| Thao tác | Cách làm |
|---|---|
| Quay menu chính | Nút **BACK (PC1)** |
| Bắt đầu chơi | Nút **CENTER (PA0)** |

### 10.3. Custom wizard (chi tiết cấu hình)

| Bước | Nội dung |
|---|---|
| Mode | 1 hoặc 2 người |
| Map | Chọn map + xem preview |
| Ghost Speed | Slow / Normal / Fast |
| 2P Mode | Co-op hoặc Vs Ghost *(nếu 2 người)* |
| Ghost Count | 1–4 ghost |
| Ghost Setup | Tính cách từng ghost |

- Sử dụng phím hướng hoặc Joystick để thay đổi các giá trị lựa chọn.
- **Back / Start** sử dụng nút vật lý **BACK (PC1)** / **CENTER (PA0)**.

### 10.4. Trong ván chơi

**Player 1 (Pacman vàng):**

1. Ưu tiên **nút vật lý** PC2–PC5 (nút DOWN dùng PC11)
2. Nếu không nhấn nút → dùng **joystick analog** PA1/PA2

**Player 2 (Pacman cyan — Co-op):**

- **Keyboard** mũi tên

**Bắt đầu ván:** Nhấn **PA0** (nút xanh) sau menu — đếm 3-2-1-GO.

---

## 11. Checklist lắp ráp từng bước

### Bước 1 — Chuẩn bị

- [ ] Kit STM32F429I-Discovery
- [ ] 5 nút nhấn momentary (NO) — 4 hướng + Back *(hoặc 1 module joystick KY-023 thay 4 nút hướng)*
- [ ] (Tùy chọn) Module joystick analog KY-023 + dây nối PA1/PA2
- [ ] Dây Dupont hoặc dây hàn + header GPIO
- [ ] (Tùy chọn) Bàn phím USB + cáp OTG

### Bước 2 — Hàn điều khiển Player 1

**Cách A — 4 nút riêng:**

- [ ] Nút UP → **PC2** + GND
- [ ] Nút RIGHT → **PC3** + GND
- [ ] Nút LEFT → **PC5** + GND
- [ ] Nút DOWN → **PC11** + GND *(tránh dùng PC4 vì xung đột USB keyboard)*
- [ ] Nút BACK → **PC1** + GND

**Cách B — Joystick KY-023 (thay 4 nút hướng):**

- [ ] VRx → **PA1**, VRy → **PA2**, VCC → **3.3V**, GND → **GND**

### Bước 3 — Kiểm tra không short

- [ ] Đo continuity: khi **không nhấn**, chân GPIO không nối GND
- [ ] Khi **nhấn**, chân GPIO nối GND

### Bước 4 — Cấp nguồn và flash

- [ ] Cắm USB ST-Link (CN1) vào PC
- [ ] Build & flash project (xem `README.md` mục 4)
- [ ] Màn hình hiện menu **Campaign / Custom**

### Bước 5 — Test từng ngoại vi

| Test | Kỳ vọng |
|---|---|
| Nhấn UP/DOWN/LEFT/RIGHT hoặc gạt joystick | Pacman di chuyển đúng hướng |
| Cắm USB keyboard | Custom 2P — P2 điều khiển được |
| Nhấn PA0 | Bắt đầu ván chơi |
| Nhấn BACK (PC1) trong Campaign/Custom setup | Quay về menu chính |

---

## 12. Xử lý sự cố

| Triệu chứng | Nguyên nhân có thể | Cách xử lý |
|---|---|---|
| Nút không phản hồi | Sai chân / không nối GND | Đối chiếu `stm32_ub_button.c` |
| Joystick không phản hồi | Sai chân / cấp 5V thay vì 3.3V | Kiểm tra PA1/PA2, `stm32_ub_joystick.c` |
| Joystick lệch hướng | Trục X/Y đảo hoặc ngưỡng sai | Đổi dây VRx/VRy hoặc chỉnh `JOY_ADC_THRESHOLD` |
| DOWN không nhận / bị lỗi | Xung đột PC4 + USB | Đã chuyển nút sang PC11 trong firmware. Hãy đấu dây nút DOWN sang PC11. |
| UP giật / LCD lỗi | Xung đột PC2 + LCD CS | Đấu dây sang Joystick hoặc cấu hình lại chân nút UP |
| Keyboard không nhận | Sai cổng USB | Cắm vào **OTG HS**, không phải ST-Link |
| Keyboard nhận chậm | VBUS chưa ổn | Rút/cắm lại, đợi 2–3 giây sau boot |
| Không build được | Thiếu symbol/include | Xem `README.md` mục 4 |

---

## 13. Tài liệu liên quan

| File | Nội dung |
|---|---|
| `README.md` | Cấu hình STM32CubeIDE, bảng tốc độ level Campaign |
| `ub_lib/stm32_ub_button.c` | Chân GPIO nút bấm |
| `ub_lib/stm32_ub_joystick.c` | Joystick analog ADC |
| `ub_lib/stm32_ub_spi5.c` | Chân SPI LCD |
| `ub_lib/usb_hid_host_lolevel/usb_bsp.h` | Chân USB VBUS (PC4) |
| `pacman_lib/menu.c` | Luồng menu Custom |

---

## 14. Sơ đồ khối hệ thống

```text
┌─────────────────────────────────────────────────────────────┐
│                    STM32F429 Discovery                       │
│                                                              │
│  [4 nút GPIO]──PC2-PC5──► Player 1 (ưu tiên)                │
│  [Joystick ADC]──PA1/PA2─► Player 1 (dự phòng)              │
│  [Nút BACK PC1]──────────► Quay menu (Campaign/Custom)      │
│  [Nút User PA0]──────────► Start / Xác nhận                 │
│  [LCD ILI9341]──SPI+LTDC──► Hiển thị game                   │
│  [USB OTG HS]────────────► Keyboard → Player 2              │
│  [USART1 PA9/10]─────────► Debug (tùy chọn)                 │
└─────────────────────────────────────────────────────────────┘
```

## 15. Giao diện thông báo kết quả (You Win / You Lost)

Trò chơi đã được bổ sung thêm 2 màn hình thông báo kết quả chuyên nghiệp và sinh động khi một ván đấu kết thúc:

### 15.1. Màn hình chiến thắng (You Win)
- **Kích hoạt khi:** Người chơi ăn sạch tất cả các chấm trên bản đồ (Level complete).
- **Nội dung hiển thị:**
  - Tiêu đề **YOU WIN!** lớn (gấp 3 lần font gốc) màu xanh lá.
  - Hình vẽ Pacman vàng ăn chấm sinh động ở trung tâm màn hình.
  - Điểm số (**SCORE**) hiện tại màu trắng.
  - Cấp độ vừa hoàn thành (**LEVEL**) màu cyan.
- **Tương tác:** Chờ người chơi nhấn phím cứng **CENTER (PA0)** để tiếp tục sang level tiếp theo.

### 15.2. Màn hình thất bại (You Lost)
- **Kích hoạt khi:** Người chơi hết sạch mạng (lives == 0) dẫn đến Game Over.
- **Nội dung hiển thị:**
  - Tiêu đề **YOU LOST** lớn màu đỏ.
  - Hình vẽ Ghost Blinky màu đỏ sinh động ở trung tâm màn hình.
  - Điểm số chung cuộc (**SCORE**) đạt được màu trắng.
  - Cấp độ cao nhất đạt được (**LEVEL**) màu xám.
- **Tương tác:** Chờ người chơi nhấn phím cứng **CENTER (PA0)** để quay lại màn hình Menu chính.

---

*Tài liệu cập nhật theo nhánh `feature/custom-menu-ghosts` — menu Campaign/Custom, nút BACK ngoại vi PC1, joystick analog KY-023 (PA1/PA2), Co-op 2 Pacman, USB keyboard Player 2, màn hình Win/Lose hiển thị điểm.*