# STM32F429 Pacman Game

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![STM32F429](https://img.shields.io/badge/STM32F429-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![ARM Cortex-M4](https://img.shields.io/badge/Cortex--M4-0091BD?style=for-the-badge&logo=arm&logoColor=white)
![STM32CubeIDE 1.19.0](https://img.shields.io/badge/STM32CubeIDE_1.19.0-03234B?style=for-the-badge&logo=eclipseide&logoColor=white)
![GNU Arm Embedded](https://img.shields.io/badge/GNU_Arm_Embedded-A81D33?style=for-the-badge&logo=gnu&logoColor=white)
![SPL](https://img.shields.io/badge/SPL-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![LTDC](https://img.shields.io/badge/LTDC-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![DMA2D](https://img.shields.io/badge/DMA2D-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![ILI9341](https://img.shields.io/badge/ILI9341-1C1C1C?style=for-the-badge)
![SDRAM](https://img.shields.io/badge/SDRAM-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)

**Đồ án hệ thống nhúng** tái hiện trò chơi arcade **Pac-Man** trên vi điều khiển **STM32F429ZIT6** (ARM Cortex-M4 @ 180 MHz), chạy bare-metal trên kit **STM32F429I-Discovery** — không dùng hệ điều hành, toàn bộ logic game, đồ họa và âm thanh được điều khiển trực tiếp từ firmware. Dự án được phát triển và biên dịch bằng **STM32CubeIDE 1.19.0**.

Pipeline đồ họa dùng **LTDC** với framebuffer nằm trong **SDRAM** ngoài, tăng tốc vẽ sprite qua **DMA2D** trên panel **ILI9341** 240×320. Hệ thống AI gồm **6 chiến thuật Ghost** (Chase, Ambush, Tricky, Shy, Drunk, Lazy), chế độ **Campaign** 10 cấp độ leo thang độ khó, và hỗ trợ **solo / Co-op 2P / Vs Ghost** qua joystick analog KY-023 cùng các nút bấm GPIO ngoài.

---

## Tính năng nổi bật

- **Hiệu năng đồ họa cao**: Sử dụng bộ điều khiển LTDC kết hợp DMA2D để render bản đồ và nhân vật mượt mà với độ phân giải 240x320.
- **Nhiều chế độ chơi hấp dẫn**:
  - **Campaign (Chiến dịch)**: Chinh phục 10 cấp độ thử thách với tốc độ và độ thông minh của ghost tăng dần.
  - **Custom (Tự chọn)**: Tự do thiết lập bản đồ, tốc độ ghost/Pacman, số lượng ghost và chiến thuật di chuyển cho từng ghost.
- **Hỗ trợ 2 người chơi độc lập**:
  - **Co-op Mode**: Hai Pacman (Pacman vàng và Pacman xanh lam) cùng tham gia ăn chấm và hỗ trợ nhau.
  - **Vs Ghost Mode**: Một người chơi điều khiển Pacman, người chơi thứ hai điều khiển Ghost Blinky màu đỏ để săn Pacman.
- **Điều khiển linh hoạt**: Hỗ trợ 2 Joystick Analog (KY-023) để điều khiển nhân vật mượt mà và các phím bấm vật lý để điều hướng menu.
- **Âm thanh sinh động**: Tích hợp còi chíp thụ động (Passive Buzzer) phát âm thanh click menu, tiếng waka ăn chấm thường/to, còi báo mất mạng và các giai điệu vui tươi/trầm buồn khi thắng hoặc thua game.

---



## 🔌 Yêu cầu phần cứng & Đấu dây

Dự án yêu cầu kit **STM32F429I-Discovery** cùng các ngoại vi kết nối qua hàng rào GPIO:
- 2 Module Joystick Analog (KY-023) cho người chơi.
- 5 nút nhấn vật lý momentary (NO) để điều hướng menu và dừng game.
- 1 Còi chíp thụ động (Passive Buzzer).

👉 Chi tiết sơ đồ kết nối và bảng chân GPIO xem tại: **[GUIDE.md](GUIDE.md)**.

---

## 💻 Hướng dẫn biên dịch & Cài đặt

1. Import thư mục dự án vào **STM32CubeIDE 1.19.0** (khuyến nghị dùng đúng phiên bản để tránh lệch cấu hình toolchain).
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
