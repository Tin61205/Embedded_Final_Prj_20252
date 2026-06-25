# STM32F429 Discovery Pacman Project Configuration Guide

Tài liệu này hướng dẫn cách kết nối phần cứng (sơ đồ nối dây), cấu hình chân GPIO trong mã nguồn, cơ chế xử lý nút bấm và hệ thống điều chỉnh độ khó trong dự án game Pacman trên kit **STM32F429 Discovery**.

---

## 1. Sơ đồ kết nối phần cứng (Wiring Diagram)

Do thiết kế phần cứng của kit **STM32F429 Discovery** mặc định sử dụng chân **PC14** và **PC15** làm bộ dao động thạch anh 32.768 kHz (LSE) và chân **PC12** cho hệ thống LCD/SDRAM, các nút bấm điều khiển hướng đã được chuyển sang các chân GPIO trống hoàn toàn.

Hãy kết nối các nút bấm của bạn với các chân trên hàng rào Pin Header theo sơ đồ dưới đây:

```text
               +---------------------------------------+
               |        STM32F429 Discovery            |
               +---------------------------------------+
                   |       |       |       |       |
                  PC2     PC3     PC4     PC5     PA0
                   |       |       |       |       |
                 [Nút]   [Nút]   [Nút]   [Nút]   [Nút]
                 Lên     Phải    Xuống   Trái    Center/Start
                   |       |       |       |       |
               +---------------------------------------+
               |                  GND                  | 
               +---------------------------------------+
```

### Chi tiết chân kết nối:
| Tên nút bấm | Chân GPIO | Cách đấu dây vật lý | Mức logic tích cực |
| :--- | :--- | :--- | :--- |
| **`BTN_UP` (Lên)** | **`PC2`** | Một đầu nối `PC2`, đầu kia nối **`GND`** | Active LOW (Mặc định HIGH, ấn nút = `0`) |
| **`BTN_RIGHT` (Phải)** | **`PC3`** | Một đầu nối `PC3`, đầu kia nối **`GND`** | Active LOW (Mặc định HIGH, ấn nút = `0`) |
| **`BTN_DOWN` (Xuống)** | **`PC4`** | Một đầu nối `PC4`, đầu kia nối **`GND`** | Active LOW (Mặc định HIGH, ấn nút = `0`) |
| **`BTN_LEFT` (Trái)** | **`PC5`** | Một đầu nối `PC5`, đầu kia nối **`GND`** | Active LOW (Mặc định HIGH, ấn nút = `0`) |
| **`BTN_CENTER` (Chọn/Start)** | **`PA0`** | Nút nhấn màu xanh mặc định trên kit | Active HIGH (Mặc định LOW, ấn nút = `1`) |

---




## 2. Hướng dẫn điều khiển menu

Hệ thống menu được điều khiển hoàn toàn bằng **nút bấm vật lý** hoặc **Joystick**:

### A. Menu chính (Main Menu)
* **Lựa chọn**: Di chuyển vệt sáng giữa **Campaign** (Chế độ chiến dịch) và **Custom** (Chế độ tùy chỉnh) bằng nút Lên (`PC2`) / Xuống (`PC11`) hoặc gạt Joystick.
* **Xác nhận**: Nhấn nút **Center (PA0)** để vào giao diện cấu hình tiếp theo.

### B. Chế độ Campaign (Chiến dịch)
* **Di chuyển**: Sử dụng nút Lên (`PC2`) / Xuống (`PC11`) hoặc Joystick để chuyển đổi giữa hai dòng cấu hình: **Map** (Bản đồ) và **Difficulty** (Độ khó).
* **Thay đổi giá trị**: Nhấn nút Phải (`PC3`) / Trái (`PC5`) hoặc gạt Joystick sang Phải/Trái để thay đổi bản đồ hoặc tăng/giảm độ khó (từ 1 đến 10).
* **Bắt đầu chơi**: Nhấn nút **Center (PA0)** để bắt đầu. Để quay lại menu chính, bấm nút **BACK (PC1)**.

### C. Chế độ Custom (Tự chọn)
* **Cấu hình**: Cho phép thiết lập số người chơi (1P/2P), chế độ chơi 2P (Co-op/VS), bản đồ, tốc độ ghost, số lượng ghost và chiến thuật cho từng ghost.
* **Điều khiển**: Sử dụng các nút Lên/Xuống/Trái/Phải hoặc Joystick để di chuyển vệt sáng và thay đổi giá trị cấu hình tương ứng.
* **Bắt đầu chơi**: Nhấn nút **Center (PA0)** để bắt đầu chơi. Bấm nút **BACK (PC1)** để quay lại.

---

## 3. Cấu hình độ khó (Speed level)

Hệ thống độ khó được lập trình bằng cách giảm thời gian trễ dịch chuyển (tính bằng mili-giây - ms) của Pacman và các Ghost. Càng lên level cao, các nhân vật di chuyển càng nhanh:

| Level | Pacman Speed (ms) | Blinky Speed (ms) | Pinky Speed (ms) | Inky Speed (ms) | Clyde Speed (ms) |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **1** | 30 | 40 | 50 | 60 | 70 |
| **2** | 26 | 35 | 44 | 52 | 60 |
| **3** | 22 | 30 | 38 | 44 | 50 |
| **4** | 18 | 25 | 32 | 36 | 40 |
| **5** | 14 | 20 | 26 | 28 | 30 |
| **6** | 10 | 18 | 24 | 26 | 28 |
| **7** | 10 | 16 | 22 | 24 | 26 |
| **8** | 10 | 14 | 20 | 22 | 24 |
| **9** | 10 | 12 | 18 | 20 | 22 |
| **10**| 10 | 10 | 16 | 18 | 20 |

> [!TIP]
> Để điều chỉnh tốc độ hoặc thêm các mốc độ khó mới, bạn hãy mở file **`pacman_lib/pacman.c`** và sửa các giá trị số trong mảng cấu trúc `Level_t Level[]`.

---

## 4. Cấu hình Project và Biên dịch (STM32CubeIDE Settings)

Để biên dịch thành công dự án sử dụng Standard Peripheral Library (SPL) này trên STM32CubeIDE, cần cấu hình các thông số sau trong phần **Properties** của Project:

### A. Định nghĩa ký hiệu tiền xử lý (Preprocessor Symbols)
Vào **Properties** -> **C/C++ Build** -> **Settings** -> **Tool Settings** -> **MCU GCC Compiler** -> **Preprocessor**:
Thêm các symbols sau vào mục **Defined symbols (-D)**:
* `DEBUG`
* `USE_STDPERIPH_DRIVER`
* `STM32F429_439xx`
* `STM32F4XX`
* `STM32`
* `STM32F429ZITx`
* `STM32F4`

### B. Cấu hình đường dẫn thư mục Include (Include Paths)
Vào **Properties** -> **C/C++ Build** -> **Settings** -> **Tool Settings** -> **MCU GCC Compiler** -> **Include paths**:
Thêm các đường dẫn tương đối (hoặc tuyệt đối) sau vào mục **Include paths (-I)**:
* `"${workspace_loc:/${ProjName}/Inc}"`
* `"${workspace_loc:/${ProjName}/cmsis}"`
* `"${workspace_loc:/${ProjName}/cmsis_boot}"`
* `"${workspace_loc:/${ProjName}/cmsis_lib/include}"`
* `"${workspace_loc:/${ProjName}/pacman_lib}"`
* `"${workspace_loc:/${ProjName}/ub_lib}"`
* `"${workspace_loc:/${ProjName}/ub_lib/bilder}"`
* `"${workspace_loc:/${ProjName}/ub_lib/font}"`
* `"${workspace_loc:/${ProjName}/ub_lib/usb_hid_host_lolevel}"`

### C. Cấu hình phần cứng biên dịch (MCU Settings)
Vào **Properties** -> **C/C++ Build** -> **Settings** -> **Tool Settings** -> **MCU Settings**:
* **Floating point unit**: `FPv4-SP-D16`
* **Floating point ABI**: `Hard input (float-abi=hard)`
* **Instruction set**: `Thumb (-mthumb)`

### D. Loại trừ file khỏi Build (Exclude from Build)
Do STM32CubeIDE tự động tạo file startup assembly tương thích GCC (`Startup/startup_stm32f429zitx.s`), thư mục startup cũ của CoIDE (`cmsis_boot/startup/`) đã được **loại bỏ khỏi quá trình build (Exclude from build)** nhằm tránh lỗi định nghĩa trùng lặp hàm khởi động (`Duplicate Symbol`).

