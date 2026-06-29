# Mô Tả Thuật Toán 6 Ghost & 10 Cấp Độ Khó Campaign

Tài liệu này mô tả chi tiết nguyên lý hoạt động của **6 thuật toán di chuyển Ghost** và **10 cấp độ khó** trong chế độ Campaign của trò chơi Pacman trên kit STM32F429, khớp với triển khai trong `pacman_lib/bot.c` và `pacman_lib/pacman.c`.

---

## 1. Thuật Toán Di Chuyển Của 6 Loại Ghost

Trong trò chơi Pacman, mỗi Ghost được gán một **chiến thuật** (Personality) riêng, độc lập với màu sắc/nhân vật. Hàm `bot_calc_move_by_strategy()` ánh xạ chiến thuật sang hàm di chuyển tương ứng: **Chase**, **Ambush**, **Tricky**, **Shy**, **Drunk**, **Lazy**.

### Cơ chế tìm đường chung (`bot_calc_move`)

Tất cả chiến thuật có mục tiêu cố định đều dùng chung hàm `bot_calc_move()`:

- Xét 4 hướng hợp lệ (Up, Left, Down, Right) từ ô hiện tại — cửa mở và ô đích có thể đi được.
- **Không quay ngược** hướng đang đi (tránh giật lùi).
- Tính khoảng cách Euclid bình phương tới mục tiêu:
  $$d^2 = (x_{ghost} - T_x)^2 + (y_{ghost} - T_y)^2$$
- Chọn hướng có $d^2$ nhỏ nhất; nếu hòa, ưu tiên theo thứ tự: **Up → Left → Down → Right**.
- Nếu không tìm được hướng (ngõ cụt), cho phép quay đầu làm phương án dự phòng.

### Mục tiêu người chơi trong Co-op

Các chiến thuật Chase, Ambush, Tricky, Shy và Lazy dùng `bot_get_nearest_player()` / `bot_get_nearest_player_ptr()`:

- **Solo:** mục tiêu là Pacman (Player 1).
- **Pacman Co-op:** mục tiêu là người chơi **gần nhất** (so sánh khoảng cách tới Player 1 và Player 2).

### 1.1. Chase (Blinky) — Đuổi theo trực tiếp

- **Hành vi:** Đuổi theo người chơi gần nhất một cách quyết liệt.
- **Thuật toán:**
  - Lấy toạ độ mục tiêu trực tiếp: $T_x = P_x,\; T_y = P_y$ (vị trí người chơi gần nhất).
  - Gọi `bot_calc_move()` để chọn hướng có khoảng cách ngắn nhất.

### 1.2. Ambush (Pinky) — Phục kích đón đầu

- **Hành vi:** Cố gắng đi đón đầu trước mặt hướng di chuyển của người chơi gần nhất.
- **Thuật toán:**
  - Xác định hướng di chuyển của mục tiêu (`target->move`).
  - Nếu người chơi đang dừng (`MOVE_STOP`), suy ra hướng từ skin hiện tại (Up/Right/Down/Left).
  - Lấy toạ độ mục tiêu là ô thứ 4 phía trước theo hướng đó:
    - Đi lên: $T_x = P_x,\; T_y = P_y - 4$
    - Đi xuống: $T_x = P_x,\; T_y = P_y + 4$
    - Đi phải: $T_x = P_x + 4,\; T_y = P_y$
    - Đi trái: $T_x = P_x - 4,\; T_y = P_y$
  - Kẹp toạ độ trong biên bản đồ (`0 … ROOM_CNT_X-1`, `0 … ROOM_CNT_Y-1`).
  - Gọi `bot_calc_move()` tới điểm phục kích.

### 1.3. Tricky (Inky) — Vây ráp phối hợp

- **Hành vi:** Tạo thế gọng kìm phối hợp với Blinky quanh người chơi gần nhất.
- **Thuật toán:**
  - Lấy điểm tựa $P_{temp}$ = ô thứ 2 phía trước người chơi (cùng quy tắc hướng như Pinky, offset 2 thay vì 4).
  - Lấy vị trí Blinky ($B$). Nếu Blinky không active hoặc đã chết, dùng vị trí hiện tại của Inky làm điểm tựa.
  - Tính vector $V = P_{temp} - B$, nhân đôi và cộng vào $B$:
    $$I_{target} = B + 2 \times (P_{temp} - B)$$
  - Kẹp $I_{target}$ trong biên bản đồ.
  - Gọi `bot_calc_move()` tới $I_{target}$.

### 1.4. Shy (Clyde) — Nhút nhát

- **Hành vi:** Đuổi theo khi ở xa; hoảng sợ chạy về góc scatter khi đến quá gần.
- **Thuật toán:**
  - Tính $d^2$ giữa Clyde và người chơi gần nhất.
  - Nếu $d^2 > 64$ (xa hơn 8 ô): đuổi trực tiếp giống Chase.
  - Nếu $d^2 \le 64$: chạy về **scatter point** của Clyde — góc dưới bên trái bản đồ $(0,\; ROOM\_CNT\_Y - 1)$ qua `bot_calc_move_scatter()`.

### 1.5. Ghost Drunk — Đi ngẫu nhiên / Say rượu

- **Hành vi:** Lang thang vô định, không chủ đích đuổi người chơi.
- **Thuật toán (`bot_calc_move_random`):**
  - Tại mỗi ngã rẽ, lặp chọn hướng ngẫu nhiên `rand() % 4`, bỏ qua hướng ngược lại.
  - Chỉ chấp nhận hướng có cửa mở và ô đích đi được.
  - Tối đa 100 lần thử; sau đó chọn bất kỳ hướng hợp lệ nào.

### 1.6. Ghost Lazy — Lười biếng / Rình rập

- **Hành vi:** Đi ngẫu nhiên khi ở xa; bám đuổi khi người chơi vào vùng rình rập.
- **Thuật toán (`bot_calc_move_lazy`):**
  - Tính $d^2$ tới người chơi gần nhất.
  - Nếu $d^2 \le 36$ (trong bán kính 6 ô): chạy chiến thuật **Chase**.
  - Nếu $d^2 > 36$: chạy chiến thuật **Drunk** (đi ngẫu nhiên).

---

## 2. Chế Độ Campaign

### 2.1. Thiết lập trước khi chơi

Menu **CAMPAIGN SETUP** cho phép cấu hình:

| Tuỳ chọn | Mô tả |
|---|---|
| **Map** | Chọn bản đồ (một trong các map có sẵn). |
| **Difficulty** | Cấp độ khó 1–10. |
| **Mode** | **Solo** (1 người) hoặc **Pacman Co-op** (2 người cùng ăn dot). |

Cấu hình được áp dụng qua `pacman_apply_campaign_difficulty()` từ mảng `CampaignDifficultyScenario[10]`.

### 2.2. Cơ chế trong lúc chơi

- Chỉ các Ghost có bit tương ứng trong `ghost_active_mask` mới được vẽ và di chuyển; Ghost không active vẫn khởi tạo trong nhà ma nhưng bị ẩn.
- **Thắng một màn:** điểm cao nhất được lưu theo từng bản đồ (`campaign_high_scores[map_id]`); nếu chưa đạt cấp 10 thì `campaign_difficulty` tự tăng thêm 1 cho màn tiếp theo.
- **Thua:** quay về menu; điểm cao vẫn được cập nhật nếu vượt kỷ lục cũ.

### 2.3. Tốc độ Pacman theo cấp độ

Ngoài tốc độ Ghost (delay riêng từng cấp), Pacman trong Campaign được tăng tốc dần qua mảng `CampaignPlayerSpeedMs[10]` (chậm hơn bảng `Level[]` của chế độ chuẩn):

| Cấp độ | Tốc độ Pacman (Delay) |
|:---:|:---:|
| 1 | 50 ms |
| 2 | 46 ms |
| 3 | 42 ms |
| 4 | 40 ms |
| 5 | 38 ms |
| 6 | 36 ms |
| 7 | 34 ms |
| 8 | 32 ms |
| 9 | 30 ms |
| 10 | 28 ms |

Delay càng **nhỏ** thì nhân vật di chuyển càng **nhanh**.

### 2.4. Kịch bản chi tiết 10 cấp độ khó

| Cấp độ | Số Ghost active | Chiến thuật (G1 Blinky / G2 Pinky / G3 Inky / G4 Clyde) | Tốc độ Ghost (Delay) | Tốc độ Pacman | Ý đồ thiết kế |
|:---:|:---:|---|:---:|:---:|---|
| **1** | 4 | **Lazy** / **Lazy** / **Lazy** / **Lazy** | 80 ms | 50 ms | **Siêu dễ:** Đủ 4 Ghost nhưng tất cả rình rập (chỉ đuổi trong bán kính 6 ô), rất chậm. Làm quen bản đồ và đám đông Ghost. |
| **2** | 4 | **Drunk** / **Drunk** / **Drunk** / **Drunk** | 75 ms | 46 ms | **Dễ:** 4 Ghost lang thang ngẫu nhiên — khó bị vây nhưng vẫn gây áp lực tâm lý. |
| **3** | 3 (Blinky, Pinky, Clyde) | **Lazy** / **Drunk** / — / **Drunk** | 70 ms | 42 ms | **Dễ vừa:** Giảm còn 3 Ghost; 1 rình rập + 2 gây nhiễu ngẫu nhiên. |
| **4** | 4 | **Lazy** / **Drunk** / **Lazy** / **Drunk** | 65 ms | 40 ms | **Trung bình −:** Trở lại đủ 4 Ghost với mix Lazy/Drunk xen kẽ. |
| **5** | 3 (Blinky, Pinky, Clyde) | **Chase** / **Drunk** / — / **Lazy** | 55 ms | 38 ms | **Trung bình:** Blinky đuổi trực tiếp, kết hợp Lazy rình rập và Drunk gây nhiễu. |
| **6** | 3 (Blinky, Inky, Clyde) | **Chase** / — / **Tricky** / **Lazy** | 50 ms | 36 ms | **Trung bình +:** Inky bắt đầu vây ráp phối hợp với Blinky. |
| **7** | 4 | **Lazy** / **Lazy** / **Tricky** / **Shy** | 45 ms | 34 ms | **Khó −:** Đủ 4 Ghost; áp lực cao nhưng 2 Lazy giảm độ hung hãn. |
| **8** | 4 | **Chase** / **Ambush** / **Lazy** / **Shy** | 40 ms | 32 ms | **Khó:** Pinky chặn đầu + Blinky bám đuôi tạo gọng kìm. |
| **9** | 4 | **Chase** / **Ambush** / **Tricky** / **Shy** | 32 ms | 30 ms | **Cực khó:** Cấu hình AI chuẩn Pac-Man gốc — cả 4 chiến thuật tối ưu. |
| **10** | 4 | **Chase** / **Ambush** / **Tricky** / **Shy** | 24 ms | 28 ms | **Ác mộng:** Cùng AI cấp 9 nhưng Ghost nhanh nhất (24 ms). Thử thách nằm ở phối hợp 4 Ghost và tốc độ Ghost tối đa. |