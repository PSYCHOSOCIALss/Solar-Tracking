#define BLYNK_TEMPLATE_ID "TMPL6KjLcSt8Y" // ก าหนด Template ID ของโปรเจกต์ใน Blynk 
#define BLYNK_TEMPLATE_NAME "Quickstart Template" // ชื ่อ Template ที ่ใช้ใน Blynk 
#define BLYNK_AUTH_TOKEN "NURKUDJeF6yjLYpW1qQAAYoPqM1CVzlr" // รหัสส าหรับเชื ่อมต่อโปรเจกต์
ใน Blynk 
#include <ESP8266WiFi.h> // ไลบรารีส าหรับเชื ่อมต่อ Wi-Fi บนบอร์ด ESP8266 
#include <BlynkSimpleEsp8266.h> // ไลบรารีส าหรับเชื ่อมต่อ Blynk ผ่าน Wi-Fi 
char auth[] = BLYNK_AUTH_TOKEN; // เก็บรหัส Auth Token ส าหรับเชื ่อมต่อกับ Blynk 
char ssid[] = "KawNetwork"; // ชื ่อเครือข่าย Wi-Fi ที ่ต้องการเชื ่อมต่อ 
char pass[] = "Password"; // รหัสผ่านของเครือข่าย Wi-Fi 
BlynkTimer timer; // สร้างตัวจับเวลา Blynk เพื ่อใช้เรียกฟังก์ชันซ ้า ๆ 
bool manualControl = false; // ตัวแปรบอกสถานะโหมดควบคุม (Manual หรือ Auto) 
#include <Wire.h> // ไลบรารีส าหรับการสื ่อสารแบบ I2C 
#include <BH1750.h> // ไลบรารีส าหรับเซ็นเซอร์แสง BH1750 
BH1750 lightMeter; // สร้างออบเจกต์ส าหรับเซ็นเซอร์ BH1750 
float lux[4]; // อาร์เรย์เก็บค่าความเข้มแสงจากเซ็นเซอร์แต่ละตัว 
#include <Servo.h> // ไลบรารีส าหรับควบคุมเซอร์โวมอเตอร์ 
Servo servo1; // สร้างเซอร์โวมอเตอร์ตัวที ่ 1 
Servo servo2; // สร้างเซอร์โวมอเตอร์ตัวที ่ 2 
int differenceX1; // ความต่างของแสงในแกน X (เซ็นเซอร์ 1 และ 2) 
int differenceX2; // ความต่างของแสงในแกน X (เซ็นเซอร์ 3 และ 4) 
int differenceY1; // ความต่างของแสงในแกน Y (เซ็นเซอร์ 1 และ 3) 
int differenceY2; // ความต่างของแสงในแกน Y (เซ็นเซอร์ 2 และ 4) 
int angle1 = 90; // มุมเริ ่มต้นของเซอร์โวแกน X 
int angle2 = 90; // มุมเริ ่มต้นของเซอร์โวแกน Y 
int margin = 20; // ค่าความแตกต่างขั ้นต ่าที ่ใช้ในการปรับมุม 
#include <LiquidCrystal_I2C.h> // ไลบรารีส าหรับจอ LCD แบบ I2C 
LiquidCrystal_I2C lcd(0x27,16,2); // ก าหนดที ่อยู ่และขนาดของจอ LCD 
/ ฟังก์ชันเลือกช่องส าหรับ TCA9548A I2C Multiplexer 
void tcaSelect(uint8_t channel) { 
if (channel > 7) return; // ตรวจสอบว่าเลือกช่องถูกต้อง (0-7 เท่านั ้น) 
Wire.beginTransmission(0x70); // เริ ่มการส่งข้อมูลไปยัง TCA9548A 
Wire.write(1 << channel); // เลือกช่องที ่ต้องการ 
Wire.endTransmission(); // สิ ้นสุดการส่งข้อมูล 
} 
/ ฟังก์ชันตั ้งค่าจอ LCD 
void lcdsetup(){ 
tcaSelect(4); // เลือกช่องที ่เชื ่อมต่อกับจอ LCD 
lcd.init(); // เริ ่มต้นการท างานของจอ LCD 
lcd.backlight(); // เปิดไฟหลังจอ LCD 
lcd.setCursor(0, 0); // ตั ้งเคอร์เซอร์ที ่ต าแหน่งเริ ่มต้น 
lcd.print("Initializing..."); // แสดงข้อความเริ ่มต้นบนจอ LCD 
} 
/ ฟังก์ชันแสดงค่าความเข้มแสงบนจอ LCD 
void lcdout(){ 
lcd.setCursor(0, 0); // ตั ้งเคอร์เซอร์แถวที ่ 1 
lcd.print("1:");  
lcd.print(lux[0]); // แสดงค่าความเข้มแสงจากเซ็นเซอร์ที ่ 1 
lcd.print(" 2:");  
lcd.println(lux[1]); // แสดงค่าความเข้มแสงจากเซ็นเซอร์ที ่ 2 
lcd.setCursor(0, 1); // ตั ้งเคอร์เซอร์แถวที ่ 2 
lcd.print("3:");  
lcd.print(lux[2]); // แสดงค่าความเข้มแสงจากเซ็นเซอร์ที ่ 3 
lcd.print(" 4:");  
lcd.println(lux[3]); // แสดงค่าความเข้มแสงจากเซ็นเซอร์ที ่ 4 
} 
/ ฟังก์ชันตั ้งค่าเซอร์โวมอเตอร์ 
void servosetup(){ 
servo1.attach(D3); // ก าหนดขาสัญญาณของเซอร์โว 1 
servo2.attach(D4); // ก าหนดขาสัญญาณของเซอร์โว 2 
servo1.write(angle1); // ตั ้งมุมเริ ่มต้นของเซอร์โว 1 
servo2.write(angle2); // ตั ้งมุมเริ ่มต้นของเซอร์โว 2 
} 
/ ฟังก์ชันปรับมุมเซอร์โวแกน X 
void X(){ 
differenceX1 = lux[0] - lux[1]; // ค านวณความต่างแสงในแกน X (เซ็นเซอร์ 1 และ 2) 
differenceX2 = lux[2] - lux[3]; // ค านวณความต่างแสงในแกน X (เซ็นเซอร์ 3 และ 4) 
if ((differenceX1 > margin)||(differenceX2 > margin)){ // หากความต่างมากกว่าค่าที ่ก าหนด 
if(angle1 < 180){ // ตรวจสอบว่ามุมไม่เกินขอบเขต 
angle1 = angle1 + 1; // เพิ ่มมุม 
} 
} else if((differenceX1 < (-1*margin))||(differenceX2 < (-1*margin))){ // หากความต่างน้อยกว่าค่าที ่ก าหนด 
if(angle1 > 0){ // ตรวจสอบว่ามุมไม่ต ่ากว่า 0 
angle1 = angle1 - 1; // ลดมุม 
} 
} 
servo1.write(angle1);  // ค าสั ่งนี ้จะหมุนเซอร์โวที ่ 1 ไปยังมุมที ่ก าหนดในตัวแปร angle1 
Serial.print("difference X 1 : ");  // พิมพ์ข้อความ "difference X 1 : " บน Serial Monitor 
Serial.println(differenceX1);  // พิมพ์ค่าของตัวแปร differenceX1 ลงใน Serial Monitor ตามด้วยการขึ ้นบรรทัด
ใหม่ 
Serial.print("difference X 2 : ");  // พิมพ์ข้อความ "difference X 2 : " บน Serial Monitor 
Serial.println(differenceX2);  // พิมพ์ค่าของตัวแปร differenceX2 ลงใน Serial Monitor ตามด้วยการขึ ้นบรรทัด
ใหม่ 
Serial.print("angle 1 : ");  // พิมพ์ข้อความ "angle 1 : " บน Serial Monitor 
Serial.println(angle1);  // พิมพ์ค่าของตัวแปร angle1 ลงใน Serial Monitor ตามด้วยการขึ ้นบรรทัดใหม่ 
} 
/ ฟังก์ชันปรับมุมเซอร์โวแกน Y 
void Y(){ 
differenceY1 = lux[0] - lux[2]; // ค านวณความต่างแสงในแกน Y (เซ็นเซอร์ 1 และ 3) 
differenceY2 = lux[1] - lux[3]; // ค านวณความต่างแสงในแกน Y (เซ็นเซอร์ 2 และ 4) 
if ((differenceY1 > margin)||(differenceY2 > margin)){ // หากความต่างมากกว่าค่าที ่ก าหนด 
if(angle2 < 180){ // ตรวจสอบว่ามุมไม่เกินขอบเขต 
angle2 = angle2 + 1; // เพิ ่มมุม 
} 
} else if((differenceY1 < (-1*margin))||(differenceY2 < (-1*margin))){ // หากความต่างน้อยกว่าค่าที ่ก าหนด 
if(angle2 > 0){ // ตรวจสอบว่ามุมไม่ต ่ากว่า 0 
angle2 = angle2 - 1; // ลดมุม 
} 
} 
servo2.write(angle2);  // ค าสั ่งนี ้จะหมุนเซอร์โวที ่ 2 ไปยังมุมที ่ก าหนดในตัวแปร angle2 
Serial.print("difference Y 1 : ");  // พิมพ์ข้อความ "difference Y 1 : " บน Serial Monitor 
Serial.println(differenceY1);  // พิมพ์ค่าของตัวแปร differenceY1 ลงใน Serial Monitor ตามด้วยการขึ ้นบรรทัด
ใหม่ 
Serial.print("difference Y 2 : ");  // พิมพ์ข้อความ "difference Y 2 : " บน Serial Monitor 
Serial.println(differenceY2);  // พิมพ์ค่าของตัวแปร differenceY2 ลงใน Serial Monitor ตามด้วยการขึ ้นบรรทัด
ใหม่ 
Serial.print("angle 2 : ");  // พิมพ์ข้อความ "angle 2 : " บน Serial Monitor 
Serial.println(angle2);  // พิมพ์ค่าของตัวแปร angle2 ลงใน Serial Monitor ตามด้วยการขึ ้นบรรทัดใหม่ 
} 
/ ฟังก์ชันตั ้งค่า Wi-Fi และเชื ่อมต่อ Blynk 
void WiFisetup(){ 
WiFi.begin(ssid, pass); // เริ ่มเชื ่อมต่อ Wi-Fi 
Serial.print("Connecting to Wi-Fi"); // แสดงข้อความสถานะการเชื ่อมต่อ 
while (WiFi.status() != WL_CONNECTED) {  // รอจนกว่า Wi-Fi จะเชื ่อมต่อส าเร็จ 
Serial.print(".");  // แสดงจุดเพื ่อบ่งบอกว่าโค้ดก าลังรอการเชื ่อมต่อ Wi-Fi 
delay(300);  // รอ 300 มิลลิวินาที 
} 
Serial.println(); //ฟังก์ชันนี ้ใช้ในการพิมพ์ข้อความหรือข้อมูลลงใน Serial Monitor และขึ ้นบรรทัดใหม่ (ท าให้
ผลลัพธ์ที ่แสดงใน Serial Monitor ดูเป็นระเบียบมากขึ ้น) 
Serial.print("Connected with IP: "); //ฟังก์ชันนี ้จะพิมพ์ข้อความ "Connected with IP: " ไปที ่ Serial Monitor เพื ่อ
บอกว่า Wi-Fi เชื ่อมต่อส าเร็จแล้ว และจะตามด้วยที ่อยู ่ IP ที ่ได้จากการเชื ่อมต่อ Wi-Fi 
Serial.println(WiFi.localIP()); // แสดง IP Address ที ่ได้รับ 
Blynk.begin(auth, ssid, pass); // เริ ่มเชื ่อมต่อกับ Blynk 
timer.setInterval(10000L, sendlux); // ตั ้งค่าให้ส่งข้อมูลไปยัง Blynk ทุก 10 วินาที 
} 
/ ฟังก์ชันส่งข้อมูลค่าความเข้มแสงและมุมเซอร์โวไปยัง Blynk 
void sendlux(){ 
Blynk.virtualWrite(0, lux[0]); // ส่งค่าความเข้มแสงของเซ็นเซอร์ 1 
Blynk.virtualWrite(1, lux[1]); // ส่งค่าความเข้มแสงของเซ็นเซอร์ 2 
Blynk.virtualWrite(2, lux[2]); // ส่งค่าความเข้มแสงของเซ็นเซอร์ 3 
Blynk.virtualWrite(3, lux[3]); // ส่งค่าความเข้มแสงของเซ็นเซอร์ 4 
Blynk.virtualWrite(4, angle1); // ส่งค่ามุมเซอร์โว 1 
Blynk.virtualWrite(5, angle2); // ส่งค่ามุมเซอร์โว 2 
if (!manualControl) { // หากโหมดเป็นอัตโนมัติ 
X(); // ปรับมุมเซอร์โวแกน X 
Y(); // ปรับมุมเซอร์โวแกน Y 
} 
} 
BLYNK_WRITE(V4) { // ฟังก์ชันส าหรับรับค่าจากปุ ่มหรือจอยสติ ๊กใน Virtual Pin 4 
int x = param.asInt(); // รับค่าจาก Blynk ว่าเป็นค่า Integer 
angle1 = map(x, -100, 100, 0, 180); // แปลงค่าจาก -100 ถึง 100 เป็นมุมระหว่าง 0 ถึง 180 องศา 
servo1.write(angle1); // ส่งมุมไปยังเซอร์โว 1 
manualControl = true; // เปลี ่ยนสถานะเป็นโหมดควบคุมด้วยมือ 
Serial.print("Joystick X: "); // แสดงค่าจากจอยสติ ๊กในแกน X 
Serial.println(x); // แสดงค่าจอยสติ ๊กที ่ได้รับ 
} 
BLYNK_WRITE(V5) { // ฟังก์ชันส าหรับรับค่าจากปุ ่มหรือจอยสติ ๊กใน Virtual Pin 5 
int y = param.asInt(); // รับค่าจาก Blynk ว่าเป็นค่า Integer 
angle2 = map(y, -100, 100, 0, 180); // แปลงค่าจาก -100 ถึง 100 เป็นมุมระหว่าง 0 ถึง 180 องศา 
servo2.write(angle2); // ส่งมุมไปยังเซอร์โว 2 
manualControl = true; // เปลี ่ยนสถานะเป็นโหมดควบคุมด้วยมือ 
Serial.print("Joystick Y: "); // แสดงค่าจากจอยสติ ๊กในแกน Y 
Serial.println(y); // แสดงค่าจอยสติ ๊กที ่ได้รับ 
} 
void setup() { 
Serial.begin(115200); // เริ ่มต้นการเชื ่อมต่อ Serial Monitor ด้วยบอดเรต 115200 
WiFisetup(); // ตั ้งค่าการเชื ่อมต่อ Wi-Fi และ Blynk 
Wire.begin(D2, D1); // เริ ่มต้นการเชื ่อมต่อ I2C ด้วยพิน D2 และ D1 
for (uint8_t i = 0; i < 4; i++) { // ท าการวนรอบ 4 ครั ้งส าหรับการเลือกเซ็นเซอร์แสง 
tcaSelect(i); // เลือกช่องที ่ i บน TCA9548A 
lightMeter.begin(); // เริ ่มต้นเซ็นเซอร์แสง BH1750 
} 
lcdsetup(); // ตั ้งค่าจอ LCD 
servosetup(); // ตั ้งค่าเซอร์โวมอเตอร์ 
} 
void loop() { 
for (uint8_t i = 0; i <= 5; i++) { // วนรอบทั ้งหมด 6 ช่อง 
if (i <= 3) // ตรวจสอบว่า i น้อยกว่าหรือเท่ากับ 3 (มีแค่ 4 เซ็นเซอร์แสง) 
{ 
tcaSelect(i); // เลือกช่องที ่ i บน TCA9548A 
lux[i] = lightMeter.readLightLevel(); // อ่านค่าความเข้มแสงจากเซ็นเซอร์ 
Serial.print("Sensor ");  
Serial.print(i+1); // แสดงหมายเลขของเซ็นเซอร์ 
Serial.print(": "); //ฟังก์ชันนี ้จะพิมพ์เครื ่องหมาย : ไปที ่ Serial Monitor โดยไม่ขึ ้นบรรทัดใหม่ เพื ่อแสดง
ข้อมูลความเข้มแสงที ่อ่านจากเซ็นเซอร์หลังจากข้อความ "Sensor" 
Serial.print(lux[i]); // แสดงค่าความเข้มแสงที ่อ่านได้ 
Serial.println(" lx"); //ฟังก์ชันนี ้พิมพ์ " lx" ซึ ่งหมายถึงหน่วยของค่าความเข้มแสง (lux) และขึ ้นบรรทัดใหม่
หลังจากแสดงผล ค่านี ้จะแสดงใน Serial Monitor เพื ่อให้รู ้ว่าเป็นค่าแสงที ่วัดได้จากเซ็นเซอร์ 
} 
} 
Serial.println(); // เว้นบรรทัดในการแสดงข้อมูลบน Serial Monitor 
lcdout(); // แสดงข้อมูลค่าความเข้มแสงบนจอ LCD 
Blynk.run(); // รันฟังก์ชัน Blynk เพื ่อเชื ่อมต่อกับ Blynk 
timer.run(); // รันตัวจับเวลาของ Blynk 
}
