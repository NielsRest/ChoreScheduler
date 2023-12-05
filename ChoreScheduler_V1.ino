/*
  ChoreScheduler - Arduino Program

  This Arduino program, ChoreScheduler, is designed to assist in managing daily and scheduled chores
  related to gardening, pet care, and other recurring tasks. The program utilizes an Arduino, a real-time clock (RTC),
  a 4x20 LCD display, and two buttons for user interaction.

  Key Features:
  - Displays chore messages on the LCD screen at 5 AM each day.
  - Supports different chore types: watering plants, feeding chickens, cleaning chickens, and cutting trees.
  - Messages are displayed chronologically and can be cleared with a button press.
  - Buzzer activates when unread messages are older than a day.
  - Backlog of messages persists, allowing review of older tasks.

  User Interaction:
  - Pressing buttons during chore message display clears the message or the entire backlog.
  - LCD backlight is turned on only when messages are pending, conserving power.
  - Buzzer alerts when unread messages age beyond a day.

  Note: Adjust the RTC date if needed and customize chore messages based on individual preferences.

  Author: Niels van der Rest
  Date: 19/11/2023
  Version: 3
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);

const int ConfirmPin = 2;
const int ResetPin = 3;
const int buzzerPin = 10;

struct ChoreMessage {
  DateTime timestamp;
  String message;
  bool isRead;
};

const int MAX_MESSAGES = 200;
ChoreMessage backlog[MAX_MESSAGES];
int backlogSize = 0;

bool hasDisplayedToday = false;
bool buzzerActivated = false;
bool lcdBacklightOn = false;  // Track LCD backlight state
unsigned long lastBuzzerActivationTime = 0;

void setup() {
  lcd.begin(20, 4);
  rtc.begin();

  // Check if the RTC date is before a specific date
  DateTime rtcDate = rtc.now();
  if (rtcDate.year() < 2023) {
    // Set the RTC to your desired default date
    rtc.adjust(DateTime(2023, 11, 19, 19, 42, 0));
  }

  pinMode(ConfirmPin, INPUT_PULLUP);
  pinMode(ResetPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  DateTime now = rtc.now();

  if (now.hour() == 5 && now.minute() == 0 && !hasDisplayedToday) {
    checkAndDisplayMessage(now);
    hasDisplayedToday = true;
  }

  if (digitalRead(ConfirmPin) == LOW || digitalRead(ResetPin) == LOW) {
    processButtonPress();
    delay(1000);
    lcd.clear();
    hasDisplayedToday = false;
    buzzerActivated = false;
  }

  if (!hasDisplayedToday && !lcdBacklightOn) {
    lcd.backlight();
    lcdBacklightOn = true;
    buzzerActivated = false;
  }

  if (!hasDisplayedToday) {
    checkUnreadMessages(now);
  }

  if (!hasDisplayedToday && !lcdBacklightOn) {
    lcd.noBacklight();
    lcdBacklightOn = false;
  }
}

void checkAndDisplayMessage(DateTime now) {
  int messageCode = 0; // 0: No message, 1: Water plants, 2: Feed chickens, 3: Clean chickens, 4: Cut the tree
  
  if (now.dayOfTheWeek() == 1) {
    messageCode = 2; // Feed chickens every day
  }
  if (now.dayOfTheWeek() == 7) {
    messageCode = 3; // Clean chickens every week
  }
  if (now.day() == 13 && now.month() == 9) {
    messageCode = 4; // Cut the tree on 13th September
  }
  if (now.day() % 3 == 0) {
    messageCode = 1; // Water plants every 3 days
  }

  if (messageCode != 0) {
    lcdBacklightOn = true; // Turn on the backlight
    lcd.backlight();
    displayMessage(now, messageCode);
    delay(500); // Delay to ensure the message is visible
  }
}

void displayMessage(DateTime now, int messageCode) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(now.toString("YYYY-MM-DD HH:mm")); // Display the timestamp

  lcd.setCursor(0, 1);
  switch (messageCode) {
    case 1:
      lcd.print("Water plants - Every 3 days");
      break;
    case 2:
      lcd.print("Feed chickens - Every day");
      break;
    case 3:
      lcd.print("Clean chickens - Every week");
      break;
    case 4:
      lcd.print("Cut the tree - Only on 13th September");
      break;
    default:
      lcd.print("No message");
      break;
  }
}

void addToBacklog(DateTime timestamp, String message) {
  if (backlogSize < MAX_MESSAGES) {
    backlog[backlogSize].timestamp = timestamp;
    backlog[backlogSize].message = message;
    backlog[backlogSize].isRead = false;
    backlogSize++;
  }
}

void checkUnreadMessages(DateTime now) {
  for (int i = 0; i < backlogSize; ++i) {
    if (!backlog[i].isRead && now.unixtime() - backlog[i].timestamp.unixtime() > 86400) {
      activateBuzzer();
      buzzerActivated = true;
      return;
    }
  }
}

void activateBuzzer() {
  unsigned long currentTime = millis();
  if (currentTime - lastBuzzerActivationTime >= 600000) {  // 600,000 milliseconds = 10 minutes
    tone(buzzerPin, 1000);
    delay(500);  // Buzzer on for 0.5 seconds
    noTone(buzzerPin);
    lastBuzzerActivationTime = currentTime;  // Update the last activation time
  }
}

void processButtonPress() {
  if (digitalRead(ConfirmPin) == LOW && digitalRead(ResetPin) == LOW) {
    deleteEntireBacklog();
  }
}

void deleteEntireBacklog() {
  lcd.clear();
  lcd.print("Deleting backlog...");
  delay(1000);
  backlogSize = 0;
  lcd.clear();
  lcd.print("Backlog deleted!");
  delay(1000);
  lcd.clear();
}
