# Drone Environmental Monitoring System

Dự án này là hệ thống giám sát môi trường và điều khiển phun sương (atomizer) được tích hợp trên nền tảng Drone, sử dụng kiến trúc phân tán với 3 vi điều khiển ESP32.

## 🏗 Kiến trúc Hệ thống
Hệ thống được chia làm 3 mạch ESP32 độc lập để đảm bảo hiệu suất và tránh tình trạng nghẽn luồng xử lý:

1. **Flight Controller (ESP32 FC)**: Tập trung hoàn toàn tài nguyên để đọc cảm biến IMU (MPU6050), tính toán PID và điều khiển 4 ESC động cơ Quadcopter.
2. **Payload & Environment (ESP32 Payload)**: Chạy hệ điều hành FreeRTOS để đo khoảng cách (HY-SRF05 qua ngắt cứng), nhiệt độ, độ ẩm, áp suất (DHT11, BMP280) và điều khiển MOSFET đóng/ngắt máy phun sương. Giao tiếp hai chiều với trạm mặt đất qua sóng radio HC-12.
3. **Ground Control Station (ESP32 GCS)**: Trạm mặt đất, kết nối với mạng WiFi của bạn để đẩy dữ liệu lên **ThingSpeak**, đồng thời phát một Web Server để hiển thị Giao diện Điều khiển (Gamepad) trên laptop.

---

## 🛠 Hướng dẫn Cài đặt & Nạp Code (Flashing)

### Bước 1: Yêu cầu phần mềm
- Cài đặt **Visual Studio Code (VSCode)**.
- Cài đặt extension **PlatformIO IDE** trong VSCode.

### Bước 2: Cấu hình Mạng cho Trạm Mặt Đất (GCS)
Trước khi nạp code cho mạch GCS, bạn **bắt buộc** phải cấu hình WiFi để ESP32 có thể kết nối Internet và gửi dữ liệu lên ThingSpeak:
1. Mở file `firmware_esp32_gcs/src/config/gcs_config.h`
2. Thay đổi 2 dòng sau bằng thông tin WiFi (hoặc điểm phát sóng 4G) của bạn:
   ```cpp
   #define WIFI_SSID "TÊN_WIFI_CỦA_BẠN"
   #define WIFI_PASS "MẬT_KHẨU_WIFI"
   ```

### Bước 3: Hướng dẫn Nạp Code chi tiết (VSCode + PlatformIO)

Vì dự án được chia thành 3 thư mục PlatformIO riêng biệt, bạn cần thực hiện thao tác nạp code cho **từng mạch ESP32** theo các bước cực kỳ cụ thể sau:

1. **Cài đặt Driver USB (Nếu cần)**: Đảm bảo máy tính đã cài đặt Driver CH340 hoặc CP2102 (tuỳ loại mạch ESP32) để máy tính nhận diện được cổng COM khi cắm cáp.
2. **Mở thư mục độc lập**: 
   - Trong VSCode, chọn `File` -> `Open Folder...`. 
   - Mở **đúng** thư mục của mạch bạn muốn nạp (Ví dụ: Mở thư mục `firmware_esp32_fc`, **KHÔNG** mở toàn bộ thư mục tổng chứa cả 3 dự án). Điều này giúp PlatformIO nhận diện chính xác file cấu hình `platformio.ini`.
3. **Chờ PlatformIO khởi tạo**: Lần đầu tiên mở thư mục, hãy đợi khoảng 1-2 phút để PlatformIO tự động tải về các thư viện cần thiết (như MPU6050, DHT, ThingSpeak,...) đã được tôi khai báo sẵn.
4. **Kết nối phần cứng**: Cắm cáp USB (đảm bảo là cáp truyền dữ liệu, không phải chỉ cáp sạc) nối mạch ESP32 tương ứng vào máy tính.
5. **Biên dịch và Nạp (Build & Upload)**:
   - Nhìn xuống **Thanh trạng thái (Status Bar)** màu xanh dương ở tận cùng bên dưới của VSCode.
   - Nhấn vào biểu tượng **dấu check (✓)** để **Biên dịch (Build)** thử xem có lỗi cú pháp hay thiếu thư viện nào không.
   - Nếu Build báo thành công (SUCCESS), hãy nhấn vào biểu tượng **mũi tên hướng sang phải (→)** để **Nạp code (Upload)** vào mạch ESP32.
6. **Kiểm tra hoạt động (Serial Monitor)**:
   - Nhấn vào biểu tượng **phích cắm (🔌)** ở thanh trạng thái dưới cùng để mở màn hình Serial Monitor. 
   - Tại đây sẽ hiển thị các thông tin nhật ký hoạt động (Baudrate `115200`), giúp bạn biết ESP32 đã kết nối WiFi hay đang đọc cảm biến thành công chưa.
7. **Lặp lại**: Rút mạch vừa nạp ra, cắm mạch tiếp theo vào và lặp lại từ Bước 2 cho 2 mạch còn lại.

---

## 🎮 Hướng dẫn Sử dụng (Giao diện Web Gamepad)

Khi cả 3 mạch đã được cấp nguồn và hoạt động:
1. **Khởi động GCS**: Cấp nguồn cho mạch ESP32 GCS. Mở Serial Monitor (Baudrate `115200`), bạn sẽ thấy thông báo ESP32 đang kết nối WiFi.
2. **Lấy địa chỉ IP**: Khi kết nối WiFi thành công, Serial Monitor sẽ in ra một địa chỉ IP (ví dụ: `192.168.1.15`).
3. **Mở Giao diện Điều khiển**: 
   - Đảm bảo Laptop của bạn đang kết nối **cùng một mạng WiFi** với mạch ESP32 GCS.
   - Mở trình duyệt Web (Chrome, Edge, Safari...) trên laptop, nhập địa chỉ IP ở trên vào thanh địa chỉ.
   - Giao diện ảo (Gamepad) sẽ hiện ra. Bạn có thể sử dụng các thanh trượt Joystick để điều khiển Roll/Pitch/Yaw/Throttle và nút nhấn để Bật/Tắt đầu phun sương.

---

## 📊 Xem Dữ liệu trên ThingSpeak
Cứ mỗi 15 giây, dữ liệu cảm biến đo được từ Drone (Nhiệt độ, Độ ẩm, Áp suất, Khoảng cách siêu âm) sẽ tự động được gửi lên Channel ThingSpeak của bạn (Channel ID: `3416416`). Bạn có thể đăng nhập vào ThingSpeak để xem các biểu đồ phân tích thời gian thực.
