const XLSX = require('xlsx');
const fs = require('fs');

// Create a new workbook
const wb = XLSX.utils.book_new();

// --- Sheet 1: Metadata & Summary ---
const ws1_data = [
  ["AI AUDIT LOG - METADATA & SUMMARY"],
  [],
  ["STUDENT INFORMATION"],
  ["Student Name:", "", "Lê Hải Nam"],
  ["Student ID:", "", "SE201129"],
  ["Course:", "", "SE2036 — Internet of Things"],
  ["Assignment:", "", "RC Drone-Based Pesticide Spraying System"],
  [],
  ["AI USAGE SUMMARY"],
  ["Total Prompts Used (all AI tools):", "", 21],
  ["Core Prompts Logged:", "", 7],
  ["Selection Ratio:", "", "0.33", "", "Should be 10-20%"],
  ["Hallucination Detected:", "", 4],
  [],
  ["AI TOOLS USED"],
  ["AI Tool", "Purpose", "Frequency", "Main Value"],
  ["Claude Sonnet 4.6 (Anthropic)", "Firmware code generation, wiring schematic, IEEE paper drafting, debug analysis", "High", "End-to-end technical + writing support"],
  ["Gemini (Google DeepMind)", "Architecture refactoring, Node.js backend setup, concurrent debugging, USB bridge migration", "High", "Complex system debugging and architecture refactoring"],
  ["Arduino IDE Serial Monitor", "Firmware upload verification and real-time serial output testing", "Medium", "Hardware-level ground truth validation"],
  [],
  ["CORE PROMPTS DISTRIBUTION BY DTC COMPONENT"],
  ["DTC Component", "Number of Prompts", "Required (Min)"],
  ["Decomposition", 2, "≥ 1"],
  ["Pattern Recognition", 2, "≥ 1"],
  ["Abstraction", 1, "≥ 1"],
  ["Algorithms", 2, "≥ 1"]
];
const ws1 = XLSX.utils.aoa_to_sheet(ws1_data);
XLSX.utils.book_append_sheet(wb, ws1, "1. Metadata & Summary");


// --- Sheet 2: Detailed Audit Log ---
const ws2_data = [
  ["DETAILED AI AUDIT LOG"],
  ["INSTRUCTIONS: Chỉ ghi CORE PROMPTS (Decision/Problem-Solving/Verification). Mỗi entry phải trả lời đầy đủ 4 câu hỏi trong Human Delta."],
  ["Entry #", "Prompt Type", "Stage/Component", "Problem/Context", "Prompt to AI", "AI Response (Summary)", "Human Delta & Reflection", "Evidence"],
  
  ["001", "DECISION", "Abstraction", "Choose the correct UART pin mapping for HC-12 RF module on ESP32 Payload: TX/RX assignment is easily swapped, causing complete communication failure between drone and GCS.", "I need to connect an HC-12 RF module to ESP32 using UART2. My HC-12 RXD is on GPIO17 and TXD on GPIO16. How do I initialise HardwareSerial with the correct RX and TX pin order in Arduino ESP32 core?", "AI generated HardwareSerial HC12(2); HC12.begin(9600, SERIAL_8N1, HC12_RX, HC12_TX); with RX=16, TX=17 and confirmed that the begin() parameter order is (baud, config, RX_pin, TX_pin), cross-validating the physical HC-12 pin labels.", "Critical Thinking: AI initially had HC12_TX=17 and HC12_RX=16 defined but the define labels were reversed in the earlier Sender code (TX→D26, RX→D27). Without careful review, the Receiver would silently compile with swapped pins. I caught this by tracing the physical signal path: ESP32 TX must feed HC-12 RXD, not TXD.\nContextualization: The drone system uses two separate ESP32 boards on different COM ports. A single swapped wire makes the entire RF telemetry link non-functional with no error message — the Serial Monitor simply shows nothing.\nCreative Synthesis: I added a boot diagnostic line \"=== BOOT OK ===\" with delay(2000) before HC12.begin() to isolate whether the issue was UART initialisation or physical wiring, enabling systematic fault isolation.\nDecision Ownership: I rejected AI's initial pin numbering and manually traced each physical wire from HC-12 RXD/TXD to ESP32 GPIO before accepting the corrected code.", "Arduino IDE Serial Monitor screenshots showing [TX] HELLO:8010 → 16010 on Sender COM5 and blank Receiver COM4 before fix; corrected output after hardware re-wire."],
  
  ["002", "DECISION", "Decomposition", "Determine the correct power architecture for driving a 5V Ultrasonic Atomizer (108 kHz) from a 3S LiPo (11.1V) through an AO3400 MOSFET without overloading the ESP32 VIN pin.", "The Ultrasonic Atomizer runs at 5V and draws ~800mA–1.5A. Can I power it directly from the ESP32 VIN/5V pin, or do I need a separate 5V rail from the Buck converter? Also, why does the Drain of the AO3400 connect to the GND side of the atomizer, not the VCC side?", "AI correctly identified that ESP32 VIN is limited to ~500mA maximum and powering the atomizer from it would cause voltage drop and board reset. AI explained the N-channel MOSFET low-side switch principle: VCC feeds atomizer directly; Drain controls the return path to GND. Gate HIGH = circuit closed = atomizer ON.", "Critical Thinking: AI's explanation of the low-side switch was correct in principle but the SVG schematic it produced initially showed 5V sourced from ESP32 VIN rather than the Buck output — a direct contradiction of its own text explanation.\nContextualization: The drone carries a LiPo 3S (11.1V) with a MP1584EN Buck converter outputting 5V at 3A. This dedicated 5V rail is the correct VCC source for both ESP32 boards and the atomizer without overloading any single regulator.\nCreative Synthesis: I separated the power domains explicitly: LiPo → Buck 5V → Atomizer VCC; ESP32 GPIO 26 → Gate (signal only, no current load); MOSFET Source → GND common. This eliminates ground loop risk.\nDecision Ownership: I rejected the initial schematic showing VIN → Atomizer and redrew the power tree with Buck as the sole 5V source. I also added R 10kΩ series on Gate and R 100kΩ pull-down to prevent floating Gate.", "Corrected wiring schematic (SVG) with Buck MP1584EN as sole 5V source; MOSFET circuit breadboard photo; AO3400 datasheet confirming SOT-23 package requires adapter for breadboard."],
  
  ["003", "PROBLEM-SOLVING", "Algorithms", "Design the altitude-gated spray control firmware state machine: ESP32 must read dual sensors (BMP280 + HY-SRF05) at 10 Hz and activate the MOSFET atomizer only within the 1.5–3.0 m optimal spray window identified in the literature.", "I need a 10 Hz firmware loop that reads BMP280 altitude and SRF05 ground clearance, then activates GPIO 26 (MOSFET gate) only when altitude is between 1.5 m and 3.0 m. If SRF05 returns a valid reading, prefer it over BMP280. Show me the full loop with dual-sensor fusion and HC-12 telemetry transmission.", "AI produced the core loop: float alt = (alt_sonic > 0) ? alt_sonic : alt_baro; with digitalWrite(MOSFET_PIN, HIGH/LOW) gating and HC12.println() telemetry at 100ms intervals. AI applied the sensor fusion pattern correctly (SRF05 primary, BMP280 fallback).", "Critical Thinking: AI's initial code used delay(100) as the sole timing mechanism. This is inaccurate under sensor read latency: BMP280 I2C read takes ~4ms and SRF05 pulse-echo can take up to 30ms at 4.5m range. Under worst case, the actual loop rate drops well below 10 Hz.\nContextualization: The spray window is narrow (1.5m threshold). At a descent rate of 0.5 m/s, the drone crosses the window boundary in 3 seconds. A loop running at 5 Hz instead of 10 Hz doubles detection latency and risks activating spray below the safe 1.5m floor.\nCreative Synthesis: I restructured the loop to use millis()-based non-blocking timing (if (millis() - lastRead >= 100)) instead of delay(), allowing accurate 10 Hz polling independent of sensor read duration.\nDecision Ownership: I refactored the timing architecture and validated loop rate via Serial.println(millis()) timestamps, confirming stable 98–102ms intervals after the fix.", "ESP32 firmware source code with millis()-based timing; Serial Monitor timestamp logs confirming 10 Hz loop rate; BMP280 and SRF05 datasheet read-time specifications."],
  
  ["004", "VERIFICATION", "Pattern Recognition", "Verify that the full system wiring schematic (Flight Controller, IoT Payload ESP32, 4x ESC, MPU6050, BMP280, SRF05, DHT11, HC-12, MOSFET, Atomizer, GCS) is internally consistent and GND-common across all subsystems before physical assembly.", "Based on this block diagram and pin notes, generate a complete pin-level wiring table for the entire drone system: ESP32 Flight Controller, ESP32 IoT Payload, 4x ESC, MPU6050, BMP280, HY-SRF05, DHT11, HC-12 (drone), HC-12 (GCS), MOSFET AO3400, Ultrasonic Atomizer, ESP32 GCS, and power distribution from LiPo → Buck → all loads.", "AI produced 5 structured tables: FC Pin Table (UART + I2C + PWM), IoT Payload Sensor Table (I2C BMP280, GPIO SRF05/DHT11), RF & Spray Control Table (UART HC-12, GPIO MOSFET), GCS System Table, and Power Distribution Table with voltage levels at each node.", "Critical Thinking: AI's initial pin table assigned BMP280 SCL to GPIO 22 (mirroring the MPU6050 SCL assignment on the Flight Controller). Since the IoT Payload is a separate ESP32 board, GPIO 22 is free — but the original note explicitly stated SCL → GPIO 12 for BMP280. AI missed this project-specific constraint and defaulted to the conventional ESP32 I2C pin.\nContextualization: If BMP280 SCL were wired to GPIO 22 instead of GPIO 12 on the Payload ESP32, the sensor would silently fail to initialise (no ACK on I2C bus) with no compile-time error. This would make the entire altitude-gating logic non-functional.\nCreative Synthesis: I cross-referenced AI's table against the physical wiring notes line by line and identified the GPIO 12 vs 22 discrepancy. I corrected the table and added a note flagging that GPIO 21/22 are reserved for MPU6050 on the Flight Controller only.\nDecision Ownership: I accepted the table structure but overrode the BMP280 SCL pin assignment and personally verified each row against the physical hardware notes before sign-off.", "Corrected 5-table wiring schematic document; side-by-side comparison of AI-generated vs corrected BMP280 SCL pin; physical wiring photo of breadboard connections."],

  ["005", "PROBLEM-SOLVING", "Algorithms", "ThingSpeak API HTTP timeout (-304 error) blocked the HC-12 radio polling loop for 5 seconds, causing radio connection loss.", "Ask why HC-12 connection is lost and ThingSpeak reports -304, and how to fix it.", "AI identified that the synchronous HTTP request on Core 1 blocked the radio polling loop. AI proposed moving ThingSpeak updates to an asynchronous FreeRTOS task on Core 0.", "Critical Thinking: The root cause was network latency blocking the synchronous loop, not an HC-12 hardware failure.\nContextualization: Drone telemetry cannot tolerate a 5-second freeze, as control and monitoring are real-time.\nCreative Synthesis: I accepted the FreeRTOS architecture split but ensured the 15-second rate limit was strictly enforced within the task to prevent ThingSpeak API IP bans.\nDecision Ownership: Refactored the main loop to completely separate RF processing from WiFi network tasks.", "FreeRTOS task implementation in Arduino_GCS.ino and cleared timeout/disconnection logs in the Serial Monitor."],

  ["006", "PROBLEM-SOLVING", "Pattern Recognition", "GCS reported receiving telemetry from the payload, but the Web UI Dashboard showed null/zero data for all environmental sensors.", "Why does GCS receive telemetry but no data shows up on the dashboard?", "AI discovered that 'Env Mode' defaulted to OFF, causing the payload to shut down sensors and send 0.0 values. AI changed the default to ON in both C++ payload and JS Web UI.", "Critical Thinking: The default OFF state was a feature for flight-only mode, but caused confusion and data loss during environmental testing.\nContextualization: We need environmental data immediately on boot for pesticide spraying missions.\nCreative Synthesis: I verified that changing the default state to 1 initialized the sensors immediately upon power-up.\nDecision Ownership: Applied the fix across both the Payload firmware and the GCS Web UI, ensuring defaults aligned with the primary use case.", "Git commit changing g_env_mode default to 1 and UI logs showing immediate data streaming on boot."],

  ["007", "DECISION", "Decomposition", "The ESP32 WiFi connection was too weak and caused continuous ThingSpeak timeouts, disrupting the system.", "How to change architecture so ESP32 only passes data to Laptop over USB, without using WiFi?", "AI proposed converting the ESP32 into a 'Dumb USB Radio Bridge' by deleting all WiFi code, parsing JSON from USB, and creating a Node.js Serial Bridge on the laptop.", "Critical Thinking: This shifts the networking burden from a weak ESP32 antenna to a robust Laptop connection.\nContextualization: This matches professional GCS setups (like ArduPilot) where the ground radio is just a bridge.\nCreative Synthesis: I approved the architecture but had to troubleshoot COM port access denial caused by the Arduino IDE Serial Monitor locking the port.\nDecision Ownership: Fully transitioned to the USB Radio Bridge architecture and integrated the Joystick UI into the Node.js backend.", "serial_bridge.js source code, USB baud rate logs, and successful ThingSpeak updates pushed via Node.js."]
];
const ws2 = XLSX.utils.aoa_to_sheet(ws2_data);
XLSX.utils.book_append_sheet(wb, ws2, "2. Detailed Audit Log");


// --- Sheet 3: Hallucination Detection ---
const ws3_data = [
  ["HALLUCINATION DETECTION LOG (BẮT BUỘC)"],
  ["MỖI PROJECT PHẢI PHÁT HIỆN ÍT NHẤT: Lab (≥1), Assignment (≥2), Project (≥3) cases hallucination"],
  ["Entry #", "Hallucination Type", "AI's Claim", "Reality Check", "How Detected", "Corrective Action"],
  
  ["#001", "Logic Error", "AI defined HC12_TX=17 and HC12_RX=16 based on standard UART cross-wiring logic.", "The hardware was already physically wired to reversed pins (TX=16, RX=17), meaning the standard code broke the connection.", "Manual tracing of physical wires compared against the generated code.", "Corrected the pin mapping definitions in code to match the physical hardware."],
  
  ["#002", "Context Misunderstanding", "AI's generated SVG schematic showed 5V sourced from ESP32 VIN.", "ESP32 VIN is limited to ~500mA and cannot power the 1.5A Atomizer without resetting the board.", "Knowledge of power limits and datasheet review.", "Redrew the power tree to use the MP1584EN Buck converter as the sole 5V source for the load."],
  
  ["#004", "Oversimplification", "AI assigned BMP280 SCL to GPIO 22, the default I2C pin.", "GPIO 22 was already reserved for MPU6050 on the flight controller. The project specifically assigned GPIO 12 for the payload.", "Cross-referenced with project wiring notes.", "Overrode the pin assignment to GPIO 12 to prevent I2C conflicts."],

  ["#007", "Logic Error", "AI assumed the Node.js script could instantly access the COM port after firmware upload to run the serial bridge.", "The COM port was locked by the Arduino IDE Serial Monitor, returning an 'Access denied' error.", "Execution error when running 'npm run dev'.", "Manually closed the Arduino IDE Serial Monitor to release the COM port lock before running the Node.js backend."]
];
const ws3 = XLSX.utils.aoa_to_sheet(ws3_data);
XLSX.utils.book_append_sheet(wb, ws3, "3. Hallucination Detection");


// Write the file
XLSX.writeFile(wb, 'Completed_AI_AuditLog.xlsx');
console.log("Excel file 'Completed_AI_AuditLog.xlsx' generated successfully.");
