#include <WiFi.h>
#include <ESP32Servo.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "Himesh's Wifi";
const char* password = "12345678";

// Servo pins
const int basePin = 18;
const int shoulderPin = 19;
const int elbowPin = 21;
const int wristRotatePin = 22;
const int wristUpDownPin = 23;
const int gripperPin = 25;

// Servo objects
Servo baseServo, shoulderServo, elbowServo, wristRotateServo, wristUpDownServo, gripperServo;

// Servo angle limits (adjust according to your mechanical constraints)
const int BASE_MIN = 0, BASE_MAX = 180;
const int SHOULDER_MIN = 30, SHOULDER_MAX = 150;
const int ELBOW_MIN = 20, ELBOW_MAX = 160;
const int WRIST_ROTATE_MIN = 0, WRIST_ROTATE_MAX = 180;
const int WRIST_UPDOWN_MIN = 45, WRIST_UPDOWN_MAX = 135;
const int GRIPPER_MIN = 60, GRIPPER_MAX = 120;

// Create a web server on port 80
WebServer server(80);

void handleServoControl(String servoName, Servo& servo, int minAngle, int maxAngle) {
  if (server.hasArg("angle")) {
    int angle = server.arg("angle").toInt();
    angle = constrain(angle, minAngle, maxAngle);
    
    servo.write(angle);
    server.send(200, "text/plain", servoName + " set to " + String(angle) + " degrees");
    Serial.println(servoName + " set to " + String(angle) + " degrees");
  } else {
    server.send(400, "text/plain", "Missing angle parameter");
  }
}

void handleRoot() {
  String html = "<html><head><title>Robotic Arm Control</title><style>";
  html += "body { font-family: Arial, sans-serif; margin: 20px; }";
  html += "h1 { color: #444; }";
  html += "ul { list-style-type: none; padding: 0; }";
  html += "li { margin: 8px 0; }";
  html += "a { color: #0066cc; text-decoration: none; }";
  html += "a:hover { text-decoration: underline; }";
  html += "</style></head><body>";
  html += "<h1>Robotic Arm Control</h1>";
  html += "<p>Use these endpoints:</p>";
  html += "<ul>";
  html += "<li><a href='/base?angle=90'>/base?angle=value</a> (0-180)</li>";
  html += "<li><a href='/shoulder?angle=90'>/shoulder?angle=value</a> (30-150)</li>";
  html += "<li><a href='/elbow?angle=90'>/elbow?angle=value</a> (20-160)</li>";
  html += "<li><a href='/wrist_rotate?angle=90'>/wrist_rotate?angle=value</a> (0-180)</li>";
  html += "<li><a href='/wrist_updown?angle=90'>/wrist_updown?angle=value</a> (45-135)</li>";
  html += "<li><a href='/gripper?angle=90'>/gripper?angle=value</a> (60-120)</li>";
  html += "</ul>";
  html += "<p>Current IP: " + WiFi.localIP().toString() + "</p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Attach servos to pins and initialize
  Serial.println("Attaching servos...");
  
  if (!baseServo.attach(basePin)) Serial.println("Failed to attach base servo");
  if (!shoulderServo.attach(shoulderPin)) Serial.println("Failed to attach shoulder servo");
  if (!elbowServo.attach(elbowPin)) Serial.println("Failed to attach elbow servo");
  if (!wristRotateServo.attach(wristRotatePin)) Serial.println("Failed to attach wrist rotate servo");
  if (!wristUpDownServo.attach(wristUpDownPin)) Serial.println("Failed to attach wrist up/down servo");
  if (!gripperServo.attach(gripperPin)) Serial.println("Failed to attach gripper servo");

  // Initialize servos to middle position
  baseServo.write(90);
  shoulderServo.write(90);
  elbowServo.write(90);
  wristRotateServo.write(90);
  wristUpDownServo.write(90);
  gripperServo.write(90);

  Serial.println("All servos initialized to 90 degrees.");

  // Set up HTTP endpoints
  server.on("/", handleRoot);
  server.on("/base", []() { handleServoControl("base", baseServo, BASE_MIN, BASE_MAX); });
  server.on("/shoulder", []() { handleServoControl("shoulder", shoulderServo, SHOULDER_MIN, SHOULDER_MAX); });
  server.on("/elbow", []() { handleServoControl("elbow", elbowServo, ELBOW_MIN, ELBOW_MAX); });
  server.on("/wrist_rotate", []() { handleServoControl("wrist_rotate", wristRotateServo, WRIST_ROTATE_MIN, WRIST_ROTATE_MAX); });
  server.on("/wrist_updown", []() { handleServoControl("wrist_updown", wristUpDownServo, WRIST_UPDOWN_MIN, WRIST_UPDOWN_MAX); });
  server.on("/gripper", []() { handleServoControl("gripper", gripperServo, GRIPPER_MIN, GRIPPER_MAX); });

  server.begin();
  Serial.println("HTTP server started. Use '/[servo_name]?angle=...' to control servos.");
}

void loop() {
  server.handleClient();
  delay(2); // Allow the CPU to switch to other tasks
}