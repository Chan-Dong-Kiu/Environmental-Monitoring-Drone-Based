$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$excel.DisplayAlerts = $false

# Open template
$source = "d:\Study\tài liệu\SUMMER2026\IOT102\drone\DRONE_MINI-main\DRONE_MINI-main\Environmental-Monitoring\Environmental-Monitoring-Drone-Based\AI_AuditLog_Template.xlsx"
$target = "d:\Study\tài liệu\SUMMER2026\IOT102\drone\DRONE_MINI-main\DRONE_MINI-main\Environmental-Monitoring\Environmental-Monitoring-Drone-Based\AI_AuditLog_English.xlsx"
Copy-Item $source $target -Force

$workbook = $excel.Workbooks.Open($target)

# Sheet 1: Metadata
$sheet1 = $workbook.Sheets.Item(1)
$sheet1.Cells.Item(4,3) = "IoT Student (AI Assistant)"
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
    @("001", "DECISION", "Decomposition", "System Architecture Decomposition", "The Drone project uses 3 ESP32 boards. Please help analyze which board should handle which task.", "Suggested dividing into FC (flight), Payload (sensors), GCS (Web UI).", "Logical decision because FC requires realtime processing and cannot bear the Web Server load.", "Source code architecture"),
    @("002", "OPTIMIZATION", "Decomposition", "Migrating code to Arduino IDE", "PlatformIO is failing. How to restructure the files for Arduino IDE?", "Advised to flatten the directory (put everything together) and remove relative paths in #include.", "Arduino IDE doesn't flexibly support including subdirectories, making flattening mandatory.", "Flattened files in Arduino_GCS"),
    @("003", "ERROR HANDLING", "Pattern Recognition", "Error #endif without #if", "Constantly encountering '#endif without #if' error in header files after copying.", "Error was caused by BOM (Byte Order Mark) inserted during file creation by PowerShell.", "Recognized the error pattern due to file format. Fixed by setting the encoding to UTF-8 No BOM.", "Commit fix BOM"),
    @("004", "ERROR HANDLING", "Pattern Recognition", "GitHub language misidentification", "GitHub identifies the C++ code as C, how to fix this?", "Add a .gitattributes file to configure linguist-language=C++.", "Understood the static tool's identification pattern and applied an override rule.", ".gitattributes file"),
    @("005", "DESIGN", "Abstraction", "Web UI Design", "How to design a virtual Joystick on the web to control the Drone?", "Created static HTML with Joystick circles and a button.", "Forced the AI to use Abstraction by adding Javascript to calculate coordinates (transform).", "index_html.h JS logic"),
    @("006", "OPTIMIZATION", "Abstraction", "Communication Protocol", "Which packet structure is most efficient for sending data via HC-12?", "Use C++ struct with #pragma pack to compress byte data.", "Reduced packet size instead of transmitting JSON, optimizing for the low-bandwidth HC-12.", "struct GCSCommand"),
    @("007", "ALGORITHM", "Algorithm", "Web Server timeout", "Accessing the web at 192.168.1.92 throws ERR_CONNECTION_TIMED_OUT.", "Advised using a Mobile Hotspot to rule out router (AP isolation) issues.", "The AsyncWebServer algorithm wasn't flawed; the issue stemmed from the LAN structure.", "Successful connection"),
    @("008", "OPTIMIZATION", "Algorithm", "Power Saving", "Need an algorithm to turn off sensors when not measuring.", "Added env_mode variable and wrapped the sensor reading algorithm inside an if (g_env_mode) statement.", "Saves CPU cycles for the Payload. Completely suspends reading if not enabled.", "telemetry_tx.cpp"),
    @("009", "TESTING", "Algorithm", "Testing sensor values", "How to print sensor values to the screen for testing?", "Wrote a structured logging algorithm to format output to the Serial Monitor.", "Helps debug sensors quickly without needing to open the Web UI.", "Serial.print code"),
    @("010", "ALGORITHM", "Algorithm", "String processing tool", "Write an automated script to scan and fix include errors in files.", "Wrote a PowerShell script using Regex for string replacement.", "Automated repetitive and tedious tasks using an algorithm.", "update_protocol.ps1")
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
    @("003", "Context Misunderstanding", "AI advised adding #if into the C++ file to fix the error.", "The C++ code was correct; the error was due to the hidden BOM character from PowerShell.", "Opened the file with a Hex editor/checked encoding.", "Requested the AI to rewrite the script to save files as UTF8-NoBOM."),
    @("005", "Lazy/Placeholder Code", "AI provided HTML code for the Joystick and claimed it would work immediately.", "The interface was just static images without the Javascript to calculate events.", "Tried dragging the mouse on the web with no response.", "Forced the AI to add the JS touchmove/mousemove functions."),
    @("008", "Requirement Omission", "AI claimed the Web interface was fully functional.", "It completely lacked the Environment Measurement toggle button.", "Compared the actual Web interface with the original Flowchart.", "Requested the AI to add the button and the env_mode variable."),
    @("010", "Side effect", "AI perfectly replaced the variable in the file using Regex.", "The indiscriminate Regex replacement caused the variable to appear in two different structs.", "Reviewed the protocol.h code after running the script.", "Learned the risks of using global Regex replace."),
    @("007", "False Diagnosis", "AI advised modifying the ESP32 network configuration code to fix the Timeout error.", "The error was caused by the router blocking internal connections; the code was perfectly correct.", "Tested ping and tried using a Mobile Hotspot network.", "Kept the code unchanged and modified the network connection method."),
    @("002", "Technical Infeasibility", "Advised that simply changing the .cpp file extension to .ino would solve the issue.", "Arduino IDE requires files to be flattened or structured as standard libraries.", "IDE threw 'file not found' errors for includes.", "Forced to write a script to flatten the entire source code.")
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
echo "Done Excel English Translation"
