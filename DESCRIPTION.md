# Mô Tả Thuật Toán 6 Ghost & 10 Cấp Độ Khó Campaign

Tài liệu này mô tả chi tiết nguyên lý hoạt động của **6 thuật toán di chuyển của các Ghost** và **10 cấp độ khó** trong chế độ Campaign của trò chơi Pacman trên kit STM32F429.

---

## 1. Thuật Toán Di Chuyển Của 6 Loại Ghost

Trong trò chơi Pacman, mỗi Ghost sở hữu một tính cách (Personality) và thuật toán tìm đường (Pathfinding) riêng biệt. Điều này tạo nên sự phối hợp chiến thuật đa dạng nhằm bao vây và dồn ép người chơi.

### 1.1. Blinky — Chiến thuật "Chase" (Đuổi theo trực tiếp)
- **Hành vi:** Đuổi theo Pacman một cách quyết liệt và liên tục.
- **Thuật toán:**
  - Lấy toạ độ mục tiêu trực tiếp là vị trí hiện tại của Pacman: $T_{xp} = P_{xp}, T_{yp} = P_{yp}$.
  - Tính toán khoảng cách từ 4 ô xung quanh vị trí của Blinky (Up, Left, Down, Right - loại trừ ô đối diện hướng đang đi để tránh quay đầu đột ngột) tới mục tiêu bằng công thức khoảng cách Euclid bình phương: 
    $$d^2 = (x_{ghost} - T_{xp})^2 + (y_{ghost} - T_{yp})^2$$
  - Chọn hướng đi có khoảng cách ngắn nhất để di chuyển tiếp.

### 1.2. Pinky — Chiến thuật "Ambush" (Phục kích đón đầu)
- **Hành vi:** Cố gắng đi đón đầu trước mặt hướng di chuyển của Pacman để chặn ngách.
- **Thuật toán:**
  - Xác định hướng di chuyển hiện tại của Pacman.
  - Lấy toạ độ mục tiêu là ô thứ 4 phía trước mặt Pacman theo hướng di chuyển đó:
    - Nếu Pacman đi lên: $T_{xp} = P_{xp}, T_{yp} = P_{yp} - 4$.
    - Nếu Pacman đi xuống: $T_{xp} = P_{xp}, T_{yp} = P_{yp} + 4$.
    - Nếu Pacman đi sang phải: $T_{xp} = P_{xp} + 4, T_{yp} = P_{yp}$.
    - Nếu Pacman đi sang trái: $T_{xp} = P_{xp} - 4, T_{yp} = P_{yp}$.
  - Đảm bảo toạ độ mục tiêu nằm trong biên giới hạn bản đồ.
  - Di chuyển đến ô có khoảng cách ngắn nhất tới điểm phục kích này.

### 1.3. Inky — Chiến thuật "Tricky" (Vây ráp phối hợp)
- **Hành vi:** Di chuyển lắt léo và tạo các thế gọng kìm phức tạp khi phối hợp cùng Blinky.
- **Thuật toán:**
  - Lấy điểm tựa là ô thứ 2 phía trước mặt Pacman: $P_{temp}$ (ví dụ nếu Pacman đi sang phải thì $P_{temp} = [P_{xp} + 2, P_{yp}]$).
  - Vẽ một vector từ vị trí hiện tại của Blinky ($B$) tới điểm tựa $P_{temp}$ này.
  - Nhân đôi vector này để xác định vị trí mục tiêu cuối cùng của Inky ($I_{target}$):
    $$I_{target} = B + 2 \times (P_{temp} - B)$$
  - Điều này có nghĩa Inky sẽ cố gắng đứng đối xứng với Blinky qua điểm tựa trước mặt Pacman, tạo ra chiến thuật bao vây ép Pacman vào giữa hai Ghost.
  - Di chuyển đến ô có khoảng cách ngắn nhất tới điểm mục tiêu này.

### 1.4. Clyde — Chiến thuật "Shy" (Nhút nhát)
- **Hành vi:** Đuổi theo Pacman khi ở xa, nhưng sẽ hoảng sợ và bỏ chạy về góc riêng khi đến quá gần Pacman.
- **Thuật toán:**
  - Tính toán khoảng cách Euclid bình phương giữa Clyde và Pacman: $d_{clyde}^2$.
  - Nếu khoảng cách lớn hơn 8 ô ($d_{clyde}^2 > 64$): Clyde chuyển sang hành vi đuổi trực tiếp giống Blinky (mục tiêu là vị trí Pacman).
  - Nếu khoảng cách nhỏ hơn hoặc bằng 8 ô ($d_{clyde}^2 \le 64$): Clyde hoảng sợ, đổi mục tiêu về góc trú ẩn của mình (Góc dưới bên trái bản đồ - Scatter Point).

### 1.5. Ghost Drunk — Chiến thuật "Drunk" (Đi ngẫu nhiên / Say rượu)
- **Hành vi:** Đi lang thang vô định trên bản đồ, hoàn toàn vô hại và không có chủ đích đuổi theo người chơi.
- **Thuật toán:**
  - Tại mỗi ngã rẽ, Ghost sẽ chọn ngẫu nhiên một trong các hướng đi hợp lệ (hướng có cửa mở và không phải hướng ngược lại để tránh đi giật lùi liên tục).
  - Sử dụng hàm tạo số ngẫu nhiên `rand() % 4` để chọn hướng đi.

### 1.6. Ghost Lazy — Chiến thuật "Lazy" (Lười biếng / Rình rập)
- **Hành vi:** Đi lang thang nhàn nhã khi ở xa và chỉ trở nên hung hãn bám đuổi khi người chơi đi vào vùng ranh giới rình rập của nó.
- **Thuật toán:**
  - Tính toán khoảng cách giữa Ghost và Pacman.
  - Nếu Pacman ở ngoài phạm vi bán kính 6 ô ($d^2 > 36$): Ghost chạy chiến thuật **Drunk** (đi ngẫu nhiên hoàn toàn).
  - Nếu Pacman đi vào trong phạm vi bán kính 6 ô ($d^2 \le 36$): Ghost lập tức thức tỉnh và chạy chiến thuật **Blinky** (đuổi theo trực tiếp Pacman).

---

## 2. Kịch Bản Chi Tiết 10 Cấp Độ Khó Chế Độ Campaign

Chế độ Campaign cho phép người chơi chọn bản đồ tùy ý và nâng dần thử thách qua 10 cấp độ khó thiết kế tăng tiến mượt mà:

| Cấp độ khó | Số lượng Ghost | Chiến thuật cấu hình cho từng Ghost | Tốc độ Ghost (Delay) | Chi tiết thử thách và ý đồ thiết kế |
|:---:|:---:|---|:---:|---|
| **1** | 1 | 1 Ghost Clyde chạy chiến thuật **Drunk** | **80 ms** | **Siêu dễ:** Bản đồ chỉ có duy nhất 1 Ghost đi lang thang ngẫu nhiên và tốc độ rất chậm. Phù hợp để người chơi làm quen sơ đồ map và cách di chuyển. |
| **2** | 1 | 1 Ghost Blinky chạy chiến thuật **Lazy** | **70 ms** | **Dễ:** 1 Ghost di chuyển ngẫu nhiên khi ở xa, bắt đầu đuổi theo khi bạn đến gần trong phạm vi 6 ô. Tốc độ Ghost vẫn rất chậm. |
| **3** | 2 | G1 (Blinky): **Lazy**<br>G4 (Clyde): **Drunk** | **65 ms** | **Dễ vừa:** 2 Ghost xuất hiện đồng thời. Một kẻ đi lang thang tự do gây nhiễu, kẻ còn lại rình rập bảo vệ các khu vực đi qua. |
| **4** | 2 | G1 (Blinky): **Lazy**<br>G4 (Clyde): **Shy (Clyde)** | **60 ms** | **Trung bình -:** Xuất hiện Clyde với hành vi chuẩn (đuổi xa, chạy gần) phối hợp cùng Lazy. Tốc độ di chuyển của Ghost nhanh hơn. |
| **5** | 3 | G1 (Blinky): **Chase (Blinky)**<br>G2 (Pinky): **Drunk**<br>G4 (Clyde): **Lazy** | **55 ms** | **Trung bình:** 3 Ghost. Blinky đuổi trực tiếp dai dẳng bắt đầu ép người chơi di chuyển liên tục, kết hợp với Lazy rình rập và Drunk đi ngẫu nhiên. |
| **6** | 3 | G1 (Blinky): **Chase (Blinky)**<br>G3 (Inky): **Tricky (Inky)**<br>G4 (Clyde): **Lazy** | **50 ms** | **Trung bình +:** 3 Ghost. Inky xuất hiện và bắt đầu thực hiện các pha vây ráp phối hợp cùng Blinky. Lazy bảo vệ các chấm ngách. |
| **7** | 4 | G1 (Blinky): **Lazy**<br>G2 (Pinky): **Lazy**<br>G3 (Inky): **Tricky (Inky)**<br>G4 (Clyde): **Shy (Clyde)** | **45 ms** | **Khó -:** Đầy đủ cả 4 Ghost xuất hiện trên bản đồ. Áp lực tăng lên đáng kể khi số lượng tối đa nhưng tốc độ và hành vi vẫn ở mức chịu đựng được nhờ 2 Lazy. |
| **8** | 4 | G1 (Blinky): **Chase (Blinky)**<br>G2 (Pinky): **Ambush (Pinky)**<br>G3 (Inky): **Lazy**<br>G4 (Clyde): **Shy (Clyde)** | **40 ms** | **Khó:** Xuất hiện Pinky (chặn đầu) phối hợp cùng Blinky (bám đuôi) tạo ra các tình huống gọng kìm nguy hiểm. Tốc độ Ghost đạt mức nhanh. |
| **9** | 4 | G1 (Blinky): **Chase (Blinky)**<br>G2 (Pinky): **Ambush (Pinky)**<br>G3 (Inky): **Tricky (Inky)**<br>G4 (Clyde): **Shy (Clyde)** | **32 ms** | **Cực khó:** Đây là cấu hình chuẩn gốc của tựa game Pacman. Cả 4 Ghost chạy chiến thuật tối ưu, phối hợp hoàn hảo với tốc độ di chuyển nhanh. |
| **10** | 4 | G1 (Blinky): **Chase (Blinky)**<br>G2 (Pinky): **Ambush (Pinky)**<br>G3 (Inky): **Tricky (Inky)**<br>G4 (Clyde): **Shy (Clyde)** | **24 ms** | **Ác mộng:** Cả 4 Ghost chạy chiến thuật tối ưu với tốc độ siêu nhanh (24ms), nhanh hơn tốc độ di chuyển cơ bản của Pacman (30ms). Đòi hỏi khả năng xử lý góc cua hoàn hảo. |
