# STM32F429 Pacman Game

Dự án game **Pacman cổ điển** được phát triển trên kit phát triển **STM32F429I-Discovery** (sử dụng vi điều khiển Cortex-M4 STM32F429ZIT6). Game hiển thị trực tiếp trên màn hình LCD ILI9341 tích hợp, tận dụng bộ nhớ SDRAM làm Framebuffer để có đồ họa mượt mà và không giật lag.

Dự án hỗ trợ cả chế độ chơi đơn và chơi hai người bằng cách kết nối thêm các module điều khiển ngoài.

---

## Tính năng nổi bật

- **Hiệu năng đồ họa cao**: Sử dụng bộ điều khiển LTDC kết hợp DMA2D để render bản đồ và nhân vật mượt mà với độ phân giải 240x320.
- **Nhiều chế độ chơi hấp dẫn**:
  - **Campaign (Chiến dịch)**: Chinh phục 10 cấp độ thử thách với tốc độ và độ thông minh của ghost tăng dần.
  - **Custom (Tự chọn)**: Tự do thiết lập bản đồ, tốc độ ghost, số lượng ghost và chiến thuật di chuyển cho từng ghost.
- **Hỗ trợ 2 người chơi độc lập**:
  - **Co-op Mode**: Hai Pacman (Pacman vàng và Pacman xanh lam) cùng tham gia ăn chấm và hỗ trợ nhau.
  - **Vs Ghost Mode**: Một người chơi điều khiển Pacman, người chơi thứ hai điều khiển Ghost Blinky màu đỏ để săn Pacman.
- **Điều khiển linh hoạt**: Hỗ trợ 2 Joystick Analog (KY-023) để điều khiển nhân vật mượt mà và các phím bấm vật lý để điều hướng menu.

---

## Cấu trúc thư mục dự án

- **`pacman_lib/`**: Chứa logic cốt lõi của trò chơi bao gồm quản lý người chơi (`player.c`), hành vi AI của ghost (`bot.c`), bản đồ (`maze.c`), menu cấu hình (`menu.c`) và hiển thị đồ họa game (`gui.c`).
- **`ub_lib/`**: Hệ thống driver giao tiếp phần cứng cấp thấp (LCD ILI9341, SDRAM, phím bấm vật lý, joystick analog qua ADC, UART debug).
- **`Src/` & `Inc/`**: Mã nguồn khởi chạy chính của hệ thống (`main.c`).

---

## 🔌 Yêu cầu phần cứng & Đấu dây

Dự án yêu cầu kit **STM32F429I-Discovery** cùng các ngoại vi kết nối qua hàng rào GPIO:
- 2 Module Joystick Analog (KY-023) cho người chơi.
- 5 nút nhấn vật lý momentary (NO) để điều hướng menu và dừng game.

👉 Chi tiết sơ đồ kết nối và bảng chân GPIO xem tại: **[GUIDE.md](GUIDE.md)**.

---

## 💻 Hướng dẫn biên dịch & Cài đặt

1. Import thư mục dự án vào công cụ **STM32CubeIDE**.
2. Dự án được cấu hình sẵn sử dụng **Standard Peripheral Library (SPL)** cùng các thiết lập compiler tương ứng.
3. Kết nối kit Discovery với máy tính qua cổng USB ST-Link tích hợp.
4. Nhấp chuột phải vào Project chọn **Build Project**, sau đó chọn **Run** hoặc **Debug** để nạp chương trình xuống kit.

---

## 🎮 Cách điều khiển trong game

- **Trong Menu chính & Menu thiết lập**:
  - Sử dụng các phím bấm hướng vật lý (`PC2`/`PC11`/`PC5`/`PC3`) để di chuyển vệt sáng và thay đổi giá trị.
  - Nhấn nút **CENTER (PA0)** (User Button màu xanh trên board) để xác nhận/bắt đầu.
  - Nhấn nút **BACK (PC1)** để quay lại trang trước.
- **Trong trận đấu (Gameplay)**:
  - **Player 1** (Pacman vàng) di chuyển bằng **Joystick 1** (nối chân `PA5`/`PA7`).
  - **Player 2** (Pacman xanh hoặc Ghost Blinky) di chuyển bằng **Joystick 2** (nối chân `PA1`/`PA2`).
  - Nhấn nút **BACK (PC1)** để mở menu tạm dừng (Pause Menu).
