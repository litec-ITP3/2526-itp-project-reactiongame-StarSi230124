#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
// ---------- DISPLAY ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
// ---------- BUTTONS (angepasst auf D16 / D18) ----------
#define BUTTON_A 16   // D16 = GP16
#define BUTTON_B 18   // D18 = GP18
 
// ---------- GAME CONFIG ----------
#define MAX_LEVELS 5
unsigned long reactionA[MAX_LEVELS];
unsigned long reactionB[MAX_LEVELS];
int gameMode = 1; // 1 = Einzelspieler, 2 = Zwei-Spieler
 
void setup() {
 
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
 
  // ---------- I2C0 (richtig für dein Grove Shield) ----------
  Wire.setSDA(4);   // SDA = GP4
  Wire.setSCL(5);   // SCL = GP5
  Wire.begin();
 
  delay(100);
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
 
  randomSeed(micros());
}
 
void loop() {
  showWelcome();
  waitForStart();
  selectMode();
 
  for (int level = 1; level <= MAX_LEVELS; level++) {
    playLevel(level);
  }
 
  showResultsTable();
  delay(10000);
}
 
// ---------------- WILLKOMMEN ----------------
void showWelcome() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 15);
  display.println("Reaction");
  display.setCursor(35, 35);
  display.println("Game");
  display.display();
}
 
void waitForStart() {
  display.setTextSize(1);
  display.setCursor(20, 55);
  display.println("Taste druecken");
  display.display();
 
  while (digitalRead(BUTTON_A) == LOW || digitalRead(BUTTON_B) == LOW);
  while (digitalRead(BUTTON_A) == HIGH && digitalRead(BUTTON_B) == HIGH);
 
  delay(200);
}
 
// ---------------- MODUSAUSWAHL ----------------
void selectMode() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Modus waehlen:");
  display.setCursor(0, 20);
  display.println("Button A: 1 Spieler");
  display.println("Button B: 2 Spieler");
  display.display();
 
  while (true) {
    if (digitalRead(BUTTON_A) == LOW) {
      gameMode = 1;
      break;
    }
    if (digitalRead(BUTTON_B) == LOW) {
      gameMode = 2;
      break;
    }
  }
  delay(300);
}
 
// ---------------- SPIEL-LOGIK ----------------
void playLevel(int level) {
 
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(30, 20);
  display.print("Level ");
  display.print(level);
  display.display();
  delay(1500);
 
  display.clearDisplay();
  display.display();
 
  int maxWait = 5000 - (level * 400);
  delay(random(1500, maxWait));
 
  display.fillCircle(64, 32, 12, WHITE);
  display.display();
 
  unsigned long startTime = millis();
  bool p1Done = false;
  bool p2Done = (gameMode == 1);
 
  while (!p1Done || !p2Done) {
    if (!p1Done && digitalRead(BUTTON_A) == LOW) {
      reactionA[level - 1] = millis() - startTime;
      p1Done = true;
    }
    if (!p2Done && digitalRead(BUTTON_B) == LOW) {
      reactionB[level - 1] = millis() - startTime;
      p2Done = true;
    }
  }
 
  delay(800);
}
 
// ---------------- ERGEBNIS ----------------
void showResultsTable() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("LVL | P1 ms | P2 ms");
  display.println("--------------------");
 
  unsigned long totalA = 0;
  unsigned long totalB = 0;
 
  for (int i = 0; i < MAX_LEVELS; i++) {
    display.print(i + 1);
    display.print("   | ");
    display.print(reactionA[i]);
    display.print(" | ");
 
    totalA += reactionA[i];
 
    if (gameMode == 2) {
      display.println(reactionB[i]);
      totalB += reactionB[i];
    } else {
      display.println("---");
    }
  }
 
  display.display();
  delay(5000);
 
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.println("Winner:");
 
  display.setCursor(10, 35);
 
  if (gameMode == 1) {
    display.println("Spieler 1");
  } else {
    if (totalA < totalB) {
      display.println("Spieler 1");
    } else if (totalB < totalA) {
      display.println("Spieler 2");
    } else {
      display.println("Remis");
    }
  }
 
  display.display();
}