# BÁO CÁO ĐỒ ÁN MÔN HỌC: THIẾT KẾ HỆ THỐNG NHÚNG
## ĐỀ TÀI: GAME PACMAN TRÊN KIT STM32F429 DISCOVERY

---

# Giới thiệu

## Tổng quan trò chơi

Dự án này thực hiện thiết kế và lập trình trò chơi cổ điển **Pacman** trên kit phát triển nhúng **STM32F429 Discovery**. Trò chơi không chỉ tái hiện đầy đủ các yếu tố cơ bản của tựa game Pacman truyền thống (di chuyển trong mê cung, ăn chấm điểm, tránh né quái vật) mà còn tích hợp các thuật toán trí tuệ nhân tạo (AI Pathfinding) phức tạp cho các Ghost (bóng ma) và thiết lập chế độ chơi đa dạng để tăng tính hấp dẫn.

### Mô tả kịch bản game và các chức năng chính:

1. **Luật chơi cốt lõi (Core Gameplay):**
   - Người chơi điều khiển nhân vật **Pacman** di chuyển qua các lối đi trong mê cung để ăn toàn bộ các chấm điểm nhỏ (Dots).
   - Trên bản đồ có các chấm sức mạnh lớn (Energizers). Khi Pacman ăn được chấm to này, các Ghost sẽ bị chuyển sang trạng thái **Hoảng sợ (Frightened)** trong một khoảng thời gian ngắn (đổi sang màu xanh lam và di chuyển chậm lại). Lúc này Pacman có thể quay lại đuổi bắt và ăn các Ghost để nhận lượng điểm thưởng lớn.
   - Ghost bị ăn sẽ bị tiêu diệt và chỉ còn lại đôi mắt di chuyển nhanh quay trở về chuồng Ghost (Ghost House) để hồi sinh (Dead State).
   - Nếu Pacman va chạm với Ghost ở trạng thái bình thường, Pacman sẽ bị mất 1 mạng (Lives) và trò chơi hồi sinh lại vị trí xuất phát. Người chơi có tổng cộng 3 mạng. Trò chơi kết thúc khi người chơi ăn hết tất cả các chấm điểm (Chiến thắng - Victory) hoặc bị hết mạng (Thua cuộc - Game Over).

2. **Chế độ chơi (Game Modes):**
   - **Chế độ Chiến dịch (Campaign Mode):** Người chơi sẽ vượt qua lần lượt **10 cấp độ khó tăng tiến**. Hỗ trợ chơi đơn hoặc **Co-op 2 người** (2 Pacman). Mỗi cấp độ khó sẽ nâng cấp cả tốc độ di chuyển của Pacman/Ghost lẫn số lượng và chiến thuật AI của các Ghost (từ mức rất dễ - 1 Ghost di chuyển ngẫu nhiên, cho đến mức ác mộng - 4 Ghost AI di chuyển cực nhanh với tốc độ 24ms, đòi hỏi người chơi phải phản xạ và cua góc thật chuẩn).
   - **Chế độ Tùy chỉnh (Custom Mode):** Cho phép người chơi tự thiết lập các thông số trước khi chơi bao gồm: lựa chọn Bản đồ (Map), Số lượng người chơi (1 người chơi hoặc 2 người chơi Co-op phối hợp / 2P điều khiển Ghost Blinky), Số lượng Ghost (1 đến 4), Tốc độ của Ghost (Chậm/Bình thường/Nhanh) và lựa chọn cụ thể chiến thuật AI cho từng Ghost.

3. **Tính năng bổ sung:**
   - **High Score:** Hệ thống ghi nhớ điểm cao nhất (High Score) theo từng bản đồ ở chế độ Campaign. Bảng điểm cao có thể xem trực tiếp từ Menu chính.
   - **Điều khiển 2 Joystick:** Player 1 (Pacman vàng) dùng Joystick 1, Player 2 (Pacman xanh hoặc Ghost) dùng Joystick 2.
   - **Âm thanh (Buzzer):** Tích hợp còi thụ động (Passive Buzzer) phát tiếng "waka", hiệu ứng ăn điểm, mất mạng, nhạc thắng/thua và click menu.

4. **Giao diện Menu Điều khiển:**
   - Hệ thống cung cấp một Menu điều khiển trực quan trước khi vào game để người chơi thiết lập thông số (như mức độ khó và chế độ chơi Campaign/Custom). Bảng High Score và lựa chọn 2P cũng được tích hợp sẵn trong menu.

5. **Kịch bản di chuyển của 6 loại Ghost (AI Pathfinding):**
   Mỗi Ghost trong game sở hữu một tính cách (Personality) và chiến thuật di chuyển riêng biệt để vây bắt người chơi, bao gồm:
   - **Blinky — Chiến thuật "Chase" (Đuổi theo trực tiếp):**
     * *Hành vi:* Đuổi theo Pacman một cách liên tục và quyết liệt nhất.
     * *Cách xác định mục tiêu:* Lấy tọa độ mục tiêu trực tiếp là vị trí hiện tại của Pacman: $T_{x} = P_{xp}, T_{y} = P_{yp}$.
   - **Pinky — Chiến thuật "Ambush" (Phục kích đón đầu):**
     * *Hành vi:* Di chuyển đón đầu trước mặt hướng đi của Pacman để chặn các lối ngách.
     * *Cách xác định mục tiêu:* Lấy tọa độ mục tiêu là ô thứ 4 phía trước mặt Pacman theo hướng di chuyển hiện tại:
       - Nếu Pacman đi lên: $T_{x} = P_{xp}, T_{y} = P_{yp} - 4$.
       - Nếu Pacman đi xuống: $T_{x} = P_{xp}, T_{y} = P_{yp} + 4$.
       - Nếu Pacman đi sang phải: $T_{x} = P_{xp} + 4, T_{y} = P_{yp}$.
       - Nếu Pacman đi sang trái: $T_{x} = P_{xp} - 4, T_{y} = P_{yp}$.
   - **Inky — Chiến thuật "Tricky" (Vây ráp phối hợp):**
     * *Hành vi:* Phối hợp cùng Blinky để tạo thế bao vây gọng kìm ép Pacman vào giữa.
     * *Cách xác định mục tiêu:* Lấy điểm tựa $P_{temp}$ cách trước mặt Pacman 2 ô. Vẽ một vector từ vị trí hiện tại của Blinky ($B$) tới điểm tựa $P_{temp}$, sau đó nhân đôi vector này để xác định vị trí mục tiêu cuối cùng của Inky:
       $$T_{target} = B + 2 \times (P_{temp} - B)$$
   - **Clyde — Chiến thuật "Shy" (Nhút nhát):**
     * *Hành vi:* Đuổi theo khi ở xa Pacman, nhưng sẽ hoảng sợ bỏ chạy về góc trú ẩn riêng khi đến quá gần Pacman.
     * *Cách xác định mục tiêu:* Tính toán khoảng cách Euclid bình phương giữa Clyde và Pacman ($d_{clyde}^2$).
       - Nếu khoảng cách lớn hơn 8 ô ($d_{clyde}^2 > 64$): Đuổi trực tiếp giống Blinky (mục tiêu là vị trí Pacman).
       - Nếu khoảng cách nhỏ hơn hoặc bằng 8 ô ($d_{clyde}^2 \le 64$): Đổi mục tiêu về góc trú ẩn dưới bên trái bản đồ (Scatter Point) để lánh nạn.
   - **Ghost Drunk — Chiến thuật "Drunk" (Say rượu / Đi ngẫu nhiên):**
     * *Hành vi:* Lang thang vô định trên bản đồ, hoàn toàn vô hại và không bám đuổi người chơi.
     * *Cách xác định mục tiêu:* Tại mỗi ngã rẽ, Ghost sử dụng hàm tạo số ngẫu nhiên `rand() % 4` để chọn ngẫu nhiên một trong các hướng đi hợp lệ (hướng đi thông thoáng và không quay đầu giật lùi).
   - **Ghost Lazy — Chiến thuật "Lazy" (Lười biếng / Rình rập):**
     * *Hành vi:* Đi ngẫu nhiên khi người chơi ở xa và chỉ bắt đầu bám đuổi quyết liệt khi người chơi đi vào phạm vi rình rập của nó.
     * *Cách xác định mục tiêu:* Tính khoảng cách Euclid bình phương giữa Ghost và Pacman ($d^2$).
       - Nếu Pacman ở ngoài phạm vi bán kính 6 ô ($d^2 > 36$): Chạy chiến thuật **Drunk** (đi ngẫu nhiên hoàn toàn).
       - Nếu Pacman đi vào trong phạm vi bán kính 6 ô ($d^2 \le 36$): Chuyển sang chiến thuật **Blinky** (đuổi theo trực tiếp Pacman).

---

## Phân công công việc

Dưới đây là bảng gợi ý phân công công việc dành cho nhóm thực hiện dự án (bạn có thể tự thay đổi tên thành viên phù hợp):

| STT | Thành viên | Nhiệm vụ chi tiết | Trạng thái |
|:---:|---|---|:---:|
| 1 | **Thành viên A** (Trưởng nhóm) | - Thiết kế kiến trúc tổng thể phần mềm.<br>- Lập trình cấu hình phần cứng (Clock, GPIO, LCD LTDC, SDRAM).<br>- Triển khai thuật toán AI di chuyển cho các Ghost (Blinky, Pinky, Inky, Clyde). | Hoàn thành |
| 2 | **Thành viên B** | - Xây dựng và quản lý cấu trúc bản đồ mê cung (`maze.c`, `maze_generate.c`).<br>- Thiết kế, chuyển đổi và tích hợp tài nguyên đồ họa (Sprites, Bitmaps của nhân vật, tường, chấm điểm).<br>- Lập trình logic điều khiển Pacman và kiểm tra va chạm (`player.c`). | Hoàn thành |
| 3 | **Thành viên C** | - Thiết kế và lập trình giao diện Menu điều khiển, quản lý trạng thái chuyển tiếp game (`menu.c`).<br>- Tích hợp High Score view và hỗ trợ 2P mode.<br>- Đấu nối + driver Joystick analog (ADC), Passive Buzzer (TIM PWM), nút bấm vật lý.<br>- Soạn thảo báo cáo và làm slide thuyết trình. | Hoàn thành |

---

# Hệ thống phần cứng

## Giới thiệu các phần cứng được sử dụng

Dự án tận dụng các tài nguyên phần cứng mạnh mẽ có sẵn trên kit phát triển kết hợp với các ngoại vi ngoài:

1. **Kit STM32F429 Discovery (MCU STM32F429ZIT6):**
   - *Tác dụng:* Là bộ xử lý trung tâm (CPU) điều khiển toàn bộ hệ thống nhúng. Chip sở hữu lõi ARM Cortex-M4 hoạt động ở tần số lên đến 180 MHz, tích hợp bộ tăng tốc đồ họa LTDC và FMC điều khiển SDRAM giúp xử lý thuật toán AI và render đồ họa thời gian thực cực kỳ mượt mà.
2. **Màn hình TFT LCD QVGA (ILI9341 - 2.4 inch, độ phân giải 240x320 pixels):**
   - *Tác dụng:* Thiết bị hiển thị giao diện đồ họa chính của trò chơi (Mê cung, các nhân vật, Menu cấu hình, Bảng điểm số Score, số mạng Lives và các thông báo Win/Lose).
3. **Màn hình cảm ứng (Touchscreen Controller STMPE811) - Không sử dụng:**
   - *Tác dụng:* Trong phiên bản hiện tại, để tối ưu hóa hiệu năng và giải phóng tài nguyên hệ thống, màn hình cảm ứng không còn được sử dụng trong dự án này. Mọi thao tác tương tác menu và điều khiển nhân vật đều được thực hiện thông qua nút nhấn vật lý (menu) hoặc 2 Joystick (gameplay).
4. **Bộ nhớ ngoài SDRAM (64 Mbits):**
   - *Tác dụng:* Được sử dụng làm bộ đệm khung hình hiển thị (Frame Buffer) cho bộ điều khiển đồ họa LTDC. Do RAM nội bộ của chip không đủ chứa các lớp hình ảnh hiển thị độ phân giải QVGA 16-bit màu, SDRAM bên ngoài đóng vai trò quyết định giúp đồ họa hiển thị không bị giật hay xé hình.
5. **Nút bấm vật lý ngoài (6 nút: 4 hướng + BACK + User):**
   - *Tác dụng:* Chủ yếu dùng để điều hướng Menu (Lên/Xuống/Trái/Phải), nút BACK (PC1) để quay lại hoặc Pause, nút User (PA0) làm nút xác nhận Start/Select. Nút bấm GPIO không dùng để điều khiển nhân vật trong trận (đã chuyển sang Joystick).
6. **Joystick Analog KY-023 (2 module):**
   - *Tác dụng:* 
     - Joystick 1 (PA5/VRx, PA7/VRy - ADC1): Điều khiển Player 1 (Pacman vàng).
     - Joystick 2 (PA1/VRx, PA2/VRy - ADC1): Điều khiển Player 2 (Pacman xanh Co-op hoặc Ghost Blinky trong Vs Ghost).
   - Đọc qua ADC1, có hiệu chuẩn center tự động khi khởi động.
7. **Còi chíp thụ động Passive Buzzer (PC9 - TIM3_CH4):**
   - *Tác dụng:* Phát âm thanh tương tác: click menu, tiếng waka khi ăn chấm, hiệu ứng khi ăn energizer, tiếng chết, giai điệu thắng (Win) và thua (Lost). Sử dụng PWM + non-blocking timer.

---

## Mô tả sơ đồ mạch hệ thống

Dưới đây là sơ đồ kết nối Joystick, Buzzer, nút bấm vật lý và cấu trúc giao tiếp giữa vi điều khiển STM32F429 với màn hình TFT LCD:

```mermaid
graph TD
    subgraph "Điều khiển người chơi (Gameplay)"
        JOY1["Joystick 1 (P1 - Vàng)<br>VRx=PA5, VRy=PA7"] -->|ADC1| STM32
        JOY2["Joystick 2 (P2 - Xanh / Ghost)<br>VRx=PA1, VRy=PA2"] -->|ADC1| STM32
    end

    subgraph "Nút bấm & Âm thanh"
        BTN_UP["BTN_UP (PC2)"] --> STM32
        BTN_RIGHT["BTN_RIGHT (PC3)"] --> STM32
        BTN_DOWN["BTN_DOWN (PC11)"] --> STM32
        BTN_LEFT["BTN_LEFT (PC5)"] --> STM32
        BTN_BACK["BTN_BACK (PC1)"] --> STM32
        BTN_USER["User Button (PA0)"] --> STM32
        BUZZ["Passive Buzzer<br>PC9 (TIM3_CH4)"] --> STM32
    end

    subgraph "Kit STM32F429 Discovery"
        STM32["Vi điều khiển STM32F429ZIT6"]
        
        STM32 <-->|Giao tiếp FMC - 16bit Data| SDRAM["SDRAM ngoài (Frame Buffer)"]
        STM32 --->|Bộ điều khiển LTDC| LCD["Màn hình TFT LCD ILI9341"]
    end
    
    style STM32 fill:#1a5f7a,stroke:#333,stroke-width:2px,color:#fff
    style SDRAM fill:#57c5b6,stroke:#333,color:#000
    style LCD fill:#159895,stroke:#333,color:#fff
    style JOY1 fill:#a8e6cf,stroke:#333
    style JOY2 fill:#a8e6cf,stroke:#333
    style BUZZ fill:#ffaaa5,stroke:#333
```

### Bảng đấu nối Joystick Analog (KY-023)

| Joystick | Trục | Chân GPIO | ADC | Chức năng |
|----------|------|-----------|-----|-----------|
| **Joystick 1** | VRx | PA5 | ADC1_IN5 | Trái/Phải (Player 1) |
| **Joystick 1** | VRy | PA7 | ADC1_IN7 | Lên/Xuống (Player 1) |
| **Joystick 2** | VRx | PA1 | ADC1_IN1 | Trái/Phải (Player 2) |
| **Joystick 2** | VRy | PA2 | ADC1_IN2 | Lên/Xuống (Player 2) |
| - | VCC | 3.3V | - | Nguồn 3.3V |
| - | GND | GND | - | Mass chung |

### Chi tiết bảng đấu nối nút bấm và Buzzer:

| Thành phần | Chân GPIO | Mức logic | Mô tả |
| :--- | :--- | :--- | :--- |
| **`BTN_UP`** | `PC2` | Active LOW + Pull-up | Menu lên |
| **`BTN_RIGHT`** | `PC3` | Active LOW + Pull-up | Menu phải |
| **`BTN_DOWN`** | `PC11` | Active LOW + Pull-up | Menu xuống |
| **`BTN_LEFT`** | `PC5` | Active LOW + Pull-up | Menu trái |
| **`BTN_BACK`** | `PC1` | Active LOW + Pull-up | Quay lại / Pause |
| **`BTN_CENTER`** (User) | `PA0` | Active HIGH | Xác nhận / Start |
| **Passive Buzzer** | `PC9` (TIM3_CH4) | PWM | Âm thanh hiệu ứng |

---

# Hệ thống phần mềm

## Kiến trúc tổng thể phần mềm

Hệ thống phần mềm được xây dựng theo kiến trúc phân tầng để tách biệt rõ ràng giữa quản lý phần cứng, giao diện hiển thị và logic xử lý trò chơi:

```mermaid
graph TD
    subgraph "TẦNG ỨNG DỤNG (Game Logic Engine)"
        PacmanCore["pacman.c / .h <br> (Vòng lặp chính, Đa nhiệm Systick, Quản lý Game)"]
        PlayerControl["player.c / .h <br> (Điều khiển Pacman, Ăn điểm, Va chạm)"]
        GhostAI["bot.c, blinky.c, pinky.c, inky.c, clyde.c <br> (Thuật toán tìm đường AI của các Ghost)"]
        GameMenu["menu.c / .h <br> (Quản lý Menu, Chọn Level, Cấu hình Custom)"]
    end

    subgraph "TẦNG HỖ TRỢ GAME & HIỂN THỊ (Assets & Rendering)"
        MazeManager["maze.c, maze_generate.c <br> (Mảng Mê cung, Sinh bản đồ)"]
        GUIRender["gui.c / .h <br> (Hàm render LCD, Vẽ giao diện, Điểm số, Mạng)"]
        SkinSprites["skin.c / .h <br> (Dữ liệu Sprite Bitmap nhân vật)"]
        RandomGen["random.c / .h <br> (Tạo số ngẫu nhiên cho Ghost AI)"]
    end

    subgraph "TẦNG THƯ VIỆN BẢNG MẠCH (Utility Board Library - ub_lib)"
        UB_LCD["stm32_ub_lcd_ili9341.c / .h"]
        UB_Button["stm32_ub_button.c / .h"]
        UB_Joystick["stm32_ub_joystick.c / .h<br>(ADC1 - 2 joystick)"]
        UB_Buzzer["stm32_ub_buzzer.c / .h<br>(TIM3 PWM + non-blocking)"]
        UB_Systick["stm32_ub_systick.c / .h"]
    end

    subgraph "TẦNG PHẦN CỨNG & DRIVER (SPL / Hardware)"
        SPL["STM32F4 Standard Peripheral Library (GPIO, LTDC, SDRAM, TIM)"]
        Hardware["STM32F429ZIT6 MCU (LCD, Buttons, SDRAM)"]
    end

    %% Mối liên kết giữa các tầng
    PacmanCore --> PlayerControl & GhostAI & GameMenu
    PacmanCore --> MazeManager & GUIRender
    PlayerControl & GhostAI --> MazeManager
    GUIRender --> SkinSprites & UB_LCD
    GameMenu --> UB_Button
    PlayerControl --> UB_Joystick
    PacmanCore --> UB_Buzzer
    
    UB_LCD & UB_Button & UB_Joystick & UB_Buzzer & UB_Systick --> SPL
    SPL --> Hardware
    
    style PacmanCore fill:#f9f,stroke:#333,stroke-width:2px,color:#000
    style GhostAI fill:#ff9,stroke:#333,stroke-width:2px,color:#000
    style UB_LCD fill:#dfd,stroke:#333,color:#000
    style Hardware fill:#ddf,stroke:#333,color:#000
```

### Giải thích vai trò của các Module chính:

1. **Module Điều phối Trò chơi (`main.c` & `pacman.c`/`.h`):**
   - `main.c` là điểm khởi đầu, thực hiện cấu hình thạch anh hệ thống (`SystemInit()`) và khởi động game thông qua hàm `pacman_start()`.
   - `pacman.c` quản lý toàn bộ luồng đời của game. Nó chứa hàm vòng lặp game chính `pacman_play()`. Module này chịu trách nhiệm quản lý điểm số (`Score`), số mạng (`Lives`), mức độ khó hiện tại (`Level`), trạng thái hoảng sợ của quái vật (`Frightened`), chuyển đổi Scatter/Chase, **lưu High Score** khi thắng màn Campaign, khởi tạo Buzzer + Joystick, và xử lý logic 2 Player (player2_active, coop vs ghost).

2. **Module Điều khiển Người chơi (`player.c`/`.h`):**
   - Hỗ trợ **2 Player**: `player.c` quản lý cả `Player` (P1) và `Player2` (P2). Mỗi Player có vị trí bắt đầu, mạng, và logic di chuyển độc lập.
   - Nhận tín hiệu từ **Joystick analog** (`gui_check_joystick1()`, `gui_check_joystick2()`) để thay đổi hướng đi.
   - Kiểm tra va chạm với tường mê cung (ngăn cản Pacman đi xuyên tường) và xử lý sự kiện ăn các chấm điểm nhỏ (cộng điểm), ăn chấm to (kích hoạt chế độ Frightened của các Ghost).
   - Xử lý hoạt ảnh xoay miệng của Pacman theo hướng di chuyển. Trong chế độ 2P Co-op, cả hai Pacman đều có thể ăn điểm và bị Ghost bắt.
   - Trong chế độ Vs Ghost, Joystick 2 điều khiển trực tiếp Blinky (Ghost đỏ).

3. **Module Quản lý Mê cung (`maze.c`/`.h` & `maze_generate.c`/`.h`):**
   - Định nghĩa mảng hai chiều biểu diễn lưới bản đồ mê cung (định nghĩa ô nào là tường, ô nào là đường đi, ô nào chứa chấm thức ăn, ô nào là chuồng Ghost).
   - `maze_generate.c` cung cấp các thuật toán sinh/kiểm tra mê cung tự động để đảm bảo bản đồ hợp lệ (không bị kẹt, các đường đi thông suốt).

4. **Module Giao diện Đồ họa (`gui.c`/`.h` & `skin.c`/`.h`):**
   - `skin.c` lưu trữ các sprite đồ họa dưới dạng mảng byte bitmap (được chuyển đổi sẵn từ hình ảnh Pacman và Ghost gốc).
   - `gui.c` chịu trách nhiệm render hình ảnh lên màn hình LCD TFT qua bộ đệm khung (Frame Buffer) SDRAM. Module này vẽ bản đồ mê cung, điểm số, mạng chơi, màn hình chào mừng, menu cấu hình, màn hình thắng (Victory Screen) hoặc thua (Game Over Screen) một cách trực quan, sinh động.

5. **Module Quản lý Ghost và AI Tìm đường (`bot.c`, `blinky.c`, `pinky.c`, `inky.c`, `clyde.c`):**
   - `bot.c` quản lý cấu trúc dữ liệu chung của cả 4 Ghost (vị trí, trạng thái Alive/Frightened/Dead, tốc độ).
   - Từng Ghost sở hữu một thuật toán AI tìm đường riêng biệt được triển khai trong các file tương ứng, kế thừa cơ chế tìm đường bằng toán học.

### Cơ chế hoạt động và Logic phần mềm của Game:

1. **Đa nhiệm Độc lập nhờ Timer ngắt (Cooperative Multitasking):**
   - Game không sử dụng hệ điều hành nhúng (RTOS) để tiết kiệm tài nguyên. Thay vào đó, tốc độ di chuyển độc lập giữa Pacman và các Ghost được điều khiển thông qua các biến đếm Systick (ví dụ: `Player_Systick_Timer_ms`, `Blinky_Systic_Timer_ms`, v.v.). Các biến này được giảm dần trong hàm ngắt Systick của hệ thống (mỗi 1ms).
   - Trong vòng lặp chính `pacman_play()`, phần mềm liên tục kiểm tra nếu bộ đếm của nhân vật nào về 0 thì mới cập nhật di chuyển cho nhân vật đó và nạp lại giá trị tốc độ tương ứng (ví dụ: Pacman di chuyển mỗi 30ms, Blinky di chuyển mỗi 40ms ở Level 1). Điều này giúp các nhân vật di chuyển mượt mà với các tốc độ độc lập khác nhau.

2. **Thuật toán tìm đường AI của các Ghost:**
   - Tại mỗi ngã rẽ trong mê cung, Ghost sẽ tính toán hướng đi tiếp theo. Thuật toán hoạt động như sau:
     - Xác định tọa độ mục tiêu $T_{target}(x, y)$ dựa trên thuật toán tính cách riêng của Ghost (như đã mô tả ở phần Giới thiệu).
     - Duyệt qua tối đa 4 hướng đi có thể rẽ (Lên, Xuống, Trái, Phải). Hướng đối lập với hướng đang đi sẽ bị loại bỏ để tránh Ghost quay đầu đột ngột (trừ khi chuyển sang chế độ Frightened).
     - Với mỗi hướng hợp lệ, tính toán khoảng cách Euclid bình phương từ ô tiếp theo đó tới tọa độ mục tiêu:
       $$d^2 = (x_{next} - x_{target})^2 + (y_{next} - y_{target})^2$$
     - Ghost sẽ lựa chọn hướng đi có khoảng cách $d^2$ ngắn nhất để di chuyển tiếp.

3. **Cơ chế phát hiện va chạm (Collision Detection):**
   - Sau mỗi bước di chuyển, hàm `player_check_collisions()` được gọi để kiểm tra xem tọa độ của Pacman có trùng khớp với bất kỳ Ghost nào đang hoạt động hay không.
   - Nếu xảy ra va chạm:
     - Nếu Ghost đang ở trạng thái **Alive** (bình thường): Pacman bị tiêu diệt, trừ mạng chơi, phát hoạt ảnh chết và reset lại màn chơi. Trong 2P Co-op, cả P1 và P2 đều có mạng riêng.
     - Nếu Ghost đang ở trạng thái **Frightened** (hoảng sợ do Pacman ăn chấm to): Ghost bị Pacman ăn, đổi trạng thái sang **Dead** (chỉ còn đôi mắt chạy về chuồng hồi sinh), đồng thời cộng điểm thưởng lớn cho Pacman.

4. **High Score & Âm thanh:**
   - Sau khi thắng một màn Campaign, điểm số được so sánh và cập nhật vào `Game.campaign_high_scores[map_id]`.
   - `menu.c` cung cấp màn hình riêng "HIGH SCORES" để xem điểm cao nhất của tất cả các bản đồ.
   - Buzzer được gọi ở nhiều nơi: `UB_Buzzer_Play_MenuClick()`, `Play_EatDot()`, `Play_EatEnergizer()`, `Play_Die()`, `Play_Win()`, `Play_Lost()`. Âm thanh non-blocking được quản lý qua Systick.

---

# Kết quả đạt được

*(Phần này bạn hãy chụp ảnh thực tế màn hình Kit chạy game Pacman và chèn vào báo cáo theo hướng dẫn dưới đây)*

Dự án đã triển khai thành công trò chơi Pacman hoàn chỉnh trên kit STM32F429 Discovery với đầy đủ các tính năng đề ra, bao gồm **High Score**, **chế độ 2 người chơi dùng 2 Joystick**, và **hệ thống âm thanh Buzzer**. Dưới đây là mô tả luồng vận hành của game kèm theo các hình ảnh kết quả thực tế:

### 1. Giao diện Menu chính khi khởi động hệ thống
- **Hình ảnh minh họa:** *(Bạn chụp ảnh màn hình Menu lựa chọn Chế độ chơi và độ khó + nút High Score)*
- **Giải thích:** Khi khởi động hệ thống, màn hình TFT LCD hiển thị Menu chính cho phép lựa chọn chế độ chơi (Campaign / Custom) và cài đặt mức độ khó xuất phát (Start Level từ 1 đến 10). Người dùng sử dụng **nút bấm vật lý** (PC2/PC3/PC5/PC11 + PC1 BACK) để điều hướng và thay đổi giá trị. Nút **High Score** cho phép xem bảng điểm cao nhất của từng bản đồ Campaign. Nhấn nút xanh User Button (PA0) để xác nhận. Trong trận, điều khiển dùng 2 Joystick.

### 2. Màn hình chuẩn bị bắt đầu màn chơi (Get Ready)
- **Hình ảnh minh họa:** *(Bạn chụp ảnh màn hình lúc đếm ngược 1, 2, 3, GO)*
- **Giải thích:** Sau khi nhấn Start, bản đồ mê cung được vẽ ra rõ nét, các nhân vật (Pacman và các Ghost) được xếp vào các vị trí xuất phát mặc định. Hệ thống thực hiện đếm ngược từ 3 về 1 và hiện chữ "GO" kèm theo độ trễ hệ thống để người chơi chuẩn bị trước khi các nhân vật bắt đầu di chuyển.

### 3. Quá trình vận hành game thực tế (Gameplay) - 1P & 2P
- **Hình ảnh minh họa:** *(Bạn chụp ảnh Pacman đang di chuyển ăn chấm điểm (1P) và ảnh 2 Pacman Co-op hoặc Pacman + Ghost Blinky Vs Ghost)*
- **Giải thích:** 
  - **1 Player:** Player 1 điều khiển Pacman vàng bằng **Joystick 1** (PA5/PA7). Các Ghost bám đuổi theo đúng AI.
  - **2 Player Co-op:** Cả hai Pacman (vàng + xanh lam) cùng ăn điểm, chia sẻ nhiệm vụ. Mỗi người có mạng riêng (hiển thị "P1: x P2: y"). Sử dụng cả **Joystick 1** và **Joystick 2**.
  - **2 Player Vs Ghost:** Player 2 điều khiển trực tiếp Blinky (Ghost đỏ) săn Pacman qua Joystick 2.
  - Màn hình cập nhật tọa độ liên tục không giật lag nhờ SDRAM framebuffer + cơ chế partial clear. Điểm số (Score) liên tục tăng. Âm thanh "waka" phát khi ăn chấm (buzzer). Nhấn BACK (PC1) để pause.

### 4. Trạng thái các Ghost hoảng sợ khi Pacman ăn chấm to (Frightened Mode)
- **Hình ảnh minh họa:** *(Bạn chụp ảnh khi ăn chấm to, các Ghost đổi sang màu xanh lam)*
- **Giải thích:** Ngay khi Pacman ăn chấm to (Energizer), các Ghost lập tức đổi màu sang xanh lam, di chuyển chậm lại và di chuyển hỗn loạn trốn chạy. Nếu va chạm, Ghost bị ăn (đôi mắt), người chơi nhận điểm lớn. Buzzer phát âm thanh đặc trưng energizer.

### 5. High Score Screen
- **Hình ảnh minh họa:** *(Bạn chụp ảnh bảng HIGH SCORES hiển thị điểm của các map)*
- **Giải thích:** Từ Menu chính chọn High Score sẽ hiển thị điểm cao nhất đã đạt được ở từng bản đồ (Map) của Campaign. Điểm được cập nhật tự động khi người chơi hoàn thành màn với điểm số cao hơn kỷ lục cũ.

### 6. Kết quả thắng/thua (Victory & Game Over)
- **Hình ảnh minh họa:** *(Bạn chụp ảnh màn hình Victory Screen hoặc Game Over Screen)*
- **Giải thích:** 
  - Nếu Pacman (hoặc cả 2 Pacman ở Co-op) ăn hết toàn bộ chấm thức ăn, màn hình hiển thị **VICTORY** kèm điểm đạt được. Nếu ở Campaign sẽ tự động cập nhật High Score và chuyển level tiếp theo.
  - Nếu hết mạng (cả 2 người ở Co-op), màn hình **GAME OVER** + điểm số + nhấn nút để về Menu.
  - Có nhạc thắng (thăng) và nhạc thua (giáng) phát qua Buzzer.

### 7. Âm thanh Buzzer
- **Mô tả:** 
  - Menu: click ngắn khi di chuyển lựa chọn.
  - Ăn chấm nhỏ: "waka" ngắn 800Hz.
  - Ăn chấm to: beep cao hơn.
  - Chết: chuỗi tone giảm dần.
  - Win: giai điệu C-D-E-G-C (thăng).
  - Lose: giai điệu buồn (A-Ab-G-F).
- **Hình ảnh minh họa:** *(Không chụp được âm thanh, có thể quay video demo ngắn hoặc mô tả trong slide)*

### 8. Kết luận
Dự án đã hoàn thành đầy đủ yêu cầu: đồ họa mượt, AI Ghost đa dạng, Campaign 10 level, Custom linh hoạt, **hỗ trợ 2 người dùng 2 joystick**, **High Score**, và **hệ thống âm thanh** phong phú trên nền tảng nhúng hạn chế tài nguyên.
