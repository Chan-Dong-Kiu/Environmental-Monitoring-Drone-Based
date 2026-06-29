$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$excel.DisplayAlerts = $false

# Open template
$source = "d:\Study\tài liệu\SUMMER2026\IOT102\drone\DRONE_MINI-main\DRONE_MINI-main\Environmental-Monitoring\Environmental-Monitoring-Drone-Based\AI_AuditLog_Template.xlsx"
$target = "d:\Study\tài liệu\SUMMER2026\IOT102\drone\DRONE_MINI-main\DRONE_MINI-main\Environmental-Monitoring\Environmental-Monitoring-Drone-Based\AI_AuditLog.xlsx"
Copy-Item $source $target -Force

$workbook = $excel.Workbooks.Open($target)

# Sheet 1: Metadata
$sheet1 = $workbook.Sheets.Item(1)
$sheet1.Cells.Item(4,3) = "Sinh vien IoT (AI Assistant)"
$sheet1.Cells.Item(5,3) = "SE123456"

# Sheet 2: Detailed Log
$sheet2 = $workbook.Sheets.Item(2)
# Row 4 is the first data row. Clear existing data.
for ($r=4; $r -le 15; $r++) {
    for ($c=1; $c -le 8; $c++) {
        $sheet2.Cells.Item($r, $c) = ""
    }
}

# Entries
$entries = @(
    @("001", "QUYẾT ĐỊNH", "Decomposition", "Phân rã chức năng hệ thống", "Dự án Drone có 3 mạch ESP32. Hãy giúp phân tích xem mạch nào nên làm nhiệm vụ gì?", "Gợi ý chia thành FC (bay), Payload (cảm biến), GCS (Web UI).", "Quyết định hợp lý vì FC cần realtime, không thể gánh Web Server.", "Kiến trúc mã nguồn"),
    @("002", "TỐI ƯU", "Decomposition", "Chuyển code sang Arduino IDE", "PlatformIO bị lỗi, làm sao cấu trúc lại các file cho Arduino IDE?", "Khuyên làm phẳng thư mục (gom chung) và xóa đường dẫn trong #include.", "Arduino IDE không hỗ trợ include thư mục con linh hoạt, bắt buộc làm phẳng.", "Các file phẳng trong Arduino_GCS"),
    @("003", "XỬ LÝ LỖI", "Pattern Recognition", "Lỗi #endif without #if", "Liên tục gặp lỗi #endif without #if ở các file header sau khi copy.", "Lỗi do file bị chèn BOM (Byte Order Mark) khi tạo bằng PowerShell.", "Nhận diện pattern lỗi do format file, sửa lại tham số ghi file UTF-8 No BOM.", "Commit fix BOM"),
    @("004", "XỬ LÝ LỖI", "Pattern Recognition", "Lỗi Github nhận sai ngôn ngữ", "Github nhận diện code C++ thành C, làm sao sửa?", "Thêm file .gitattributes cấu hình linguist-language=C++.", "Hiểu được pattern nhận diện của công cụ tĩnh, áp dụng rule ghi đè.", "File .gitattributes"),
    @("005", "THIẾT KẾ", "Abstraction", "Thiết kế Web UI", "Làm sao thiết kế Joystick ảo trên web điều khiển Drone?", "Tạo HTML tĩnh với các hình tròn Joystick và nút bấm.", "Bắt buộc AI phải trừu tượng hóa bằng Javascript để tính toán tọa độ (transform).", "index_html.h JS logic"),
    @("006", "TỐI ƯU", "Abstraction", "Giao thức truyền tin", "Cấu trúc gói tin nào hiệu quả để gửi dữ liệu qua HC-12?", "Dùng C++ struct với #pragma pack để nén dữ liệu byte.", "Giảm kích thước gói tin thay vì truyền JSON, tối ưu cho HC-12 băng thông thấp.", "struct GCSCommand"),
    @("007", "THUẬT TOÁN", "Algorithm", "Web Server timeout", "Truy cập web 192.168.1.92 bị ERR_CONNECTION_TIMED_OUT.", "Khuyên dùng Mobile Hotspot để loại trừ lỗi router (AP isolation).", "Thuật toán AsyncWebServer không sai, vấn đề do cấu trúc mạng LAN.", "Kết nối thành công"),
    @("008", "TỐI ƯU", "Algorithm", "Tiết kiệm pin", "Cần thuật toán tắt cảm biến khi không cần đo.", "Thêm biến env_mode, bọc thuật toán đọc cảm biến trong lệnh if (g_env_mode).", "Tiết kiệm CPU cho Payload, nếu không bật thì ngắt hoàn toàn.", "telemetry_tx.cpp"),
    @("009", "KIỂM THỬ", "Algorithm", "Test giá trị cảm biến", "Làm sao in giá trị cảm biến ra màn hình để test?", "Viết thuật toán in log có cấu trúc format ra Serial Monitor.", "Giúp debug cảm biến nhanh chóng không cần mở Web UI.", "Serial.print code"),
    @("010", "THUẬT TOÁN", "Algorithm", "Tool xử lý chuỗi", "Viết script tự động quét và sửa lỗi include trong file.", "Viết script PowerShell dùng Regex thay thế chuỗi.", "Tự động hóa tác vụ lặp lại nhàm chán bằng thuật toán.", "update_protocol.ps1")
)

$row = 4
foreach ($e in $entries) {
    for ($c=0; $c -le 7; $c++) {
        $sheet2.Cells.Item($row, $c+1) = $e[$c]
    }
    $row++
}

# Sheet 3: Hallucination
$sheet3 = $workbook.Sheets.Item(3)
for ($r=4; $r -le 10; $r++) {
    for ($c=1; $c -le 6; $c++) {
        $sheet3.Cells.Item($r, $c) = ""
    }
}

$halls = @(
    @("003", "Context Misunderstanding", "AI khuyên thêm #if vào file C++ để sửa lỗi.", "Code C++ chuẩn xác, lỗi do BOM (kí tự ẩn) từ PowerShell.", "Mở file bằng Hex editor/kiểm tra encoding.", "Yêu cầu AI viết lại script ghi file UTF8-NoBOM."),
    @("005", "Lazy/Placeholder Code", "AI cấp mã HTML cho Joystick và bảo là chạy được ngay.", "Giao diện chỉ là ảnh tĩnh, không có Javascript tính toán sự kiện.", "Thử kéo thả chuột trên web không phản hồi.", "Bắt AI bổ sung hàm touchmove/mousemove JS."),
    @("008", "Requirement Omission", "AI bảo giao diện Web đã đầy đủ chức năng.", "Thiếu hoàn toàn nút bật tắt chức năng Đo Môi Trường.", "So sánh giao diện Web với Flowchart ban đầu.", "Yêu cầu AI bổ sung nút bấm và biến env_mode."),
    @("010", "Side effect", "AI dùng Regex thay thế hoàn hảo biến trong file.", "Lệnh Regex thay thế bừa bãi làm biến xuất hiện ở 2 struct khác nhau.", "Đọc lại code protocol.h sau khi chạy script.", "Rút kinh nghiệm về rủi ro khi dùng Regex replace toàn cục."),
    @("007", "False Diagnosis", "AI khuyên sửa code cấu hình mạng ESP32 để sửa lỗi Timeout.", "Lỗi do router chặn kết nối nội bộ, code hoàn toàn đúng.", "Test ping và dùng thử mạng Mobile Hotspot.", "Giữ nguyên code, thay đổi cách kết nối mạng."),
    @("002", "Technical Infeasibility", "Khuyên đổi tên file .cpp thành .ino là xong.", "Arduino IDE bắt buộc các file phải làm phẳng hoặc tạo thành thư viện.", "IDE báo lỗi không tìm thấy file include.", "Bắt buộc viết kịch bản flatten toàn bộ source code.")
)

$row = 4
foreach ($h in $halls) {
    for ($c=0; $c -le 5; $c++) {
        $sheet3.Cells.Item($row, $c+1) = $h[$c]
    }
    $row++
}

$workbook.Save()
$workbook.Close()
$excel.Quit()
[System.Runtime.Interopservices.Marshal]::ReleaseComObject($excel) | Out-Null
echo "Done Excel"