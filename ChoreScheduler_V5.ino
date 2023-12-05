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
  Version: 4
*/

// Libraries (necessary!)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

// Function declarations
void setup();                                            // Setup function to initialize hardware and variables
void loop();                                             // Loop function for the main program logic
void checkAndDisplayMessage(DateTime now);               // Check and display chore messages based on the current time
void displayMessage(DateTime now, const char* message);  // Display a chore message on the LCD
void checkUnreadMessages(DateTime now);                  // Check for unread messages and activate the buzzer if needed
void activateBuzzer();                                   // Activate the buzzer (sound alert)
void processButtonPress();                               // Process button presses
void deleteEntireBacklog();                              // Delete the entire backlog of messages
void addToBacklog(DateTime now, int messageCode);        // Adds incomming messages to a backlog
void displayNextMessage();                               // Go to next message

// CONSTANTS_IN_CAPITALS
const int CONFIRM_PIN = 2;                     // Pin for confirmation button
const int RESET_PIN = 3;                       // Pin for reset button
const int BUZZER_PIN = 10;                     // Pin for the buzzer
const unsigned long BUZZER_INTERVAL = 600000;  // 10 minutes in milliseconds
const unsigned long ONE_DAY_SECONDS = 86400;   // 24 hours in seconds
const int MAX_MESSAGES = 200;                  // Hour at which messages are displayed
const int MORNING_HOUR = 5;                    // Maximum number of messages
const int MORNING_MINUTE = 0;                  // Minute at which messages are displayed

// VariablesNotInCapitals
bool hasDisplayedToday = false;              // Flag to track whether messages have been displayed today
bool lcdBacklightOn = false;                 // Track LCD backlight state
bool buzzerActivated = false;                // Flag to track whether the buzzer has been activated
int backlogSize = 0;                         // Number of messages in the backlog
int currentMessageIndex = 0;                 // The current message in the backlog array
char timestampStr[20];                       // String to store formatted timestamps: Assuming "YYYY-MM-DD HH:mm" format needs 19 characters + null terminator
unsigned long lastBuzzerActivationTime = 0;  // Last time the buzzer was activated

RTC_DS3231 rtc;                      // RTC (Real-Time Clock) object
LiquidCrystal_I2C lcd(0x27, 20, 4);  // LCD object

// Enumeration to represent chore message types
enum MessageType {
  NO_MESSAGE = 0,
  WATER_PLANTS = 1,
  FEED_CHICKENS = 2,
  CLEAN_CHICKENS = 3,
  CUT_THE_TREE = 4
};


obj = str('meessagegegege',  int jaar(2025)

/// XAns idee

String msg[0][1] = {
  { 'Geef planten water' },  //0
  { 'voer de kippen' },      //1
  {''},                      //2
  {''}                       //3
};


/*test msgData   
year    0 = elk jaar, anders jaartal
maand   0 = iedere maand, anders maandgetal
week    0=iedere week,  2=iedere 2 weken
dag
starttijd 2245
eindtijd
x minuten 
prioriteit 0=laag, 10 = hoog
is read 0/1



*/

uint8 8_t msgData1[4] =  { 0, 8, 1, 1 };


int msgData2[4][4] = {
  { 0, 8, 1, 1 },  //0   
  { 0, 0, 1, 1 },  //1
  { 0, 0, 1, 1 },  //1
  { 0, 0, 1, 1 }   //2
};
}


Serial.println("Msg array");
for (int y = 0; y < 10; y++) {
  for (int x = 0; x < 4; x++) {
    msgData[y][x])=10;
  }
  Serial.println(" ");
}

//tel van 1 tot 10
  for (int x = 10; x > 0; x+=1) {
  

  }


//int voorbeeld = msgData[3][3];  //=8
///


// Structure to represent a chore message, combining timestamp, message content, and read status
struct ChoreMessage {
  DateTime timestamp;  // Timestamp indicating when the message was generated
  String message;      // Content of the chore message
  bool isRead;         // Flag to track whether the message has been read
};

// Array to store chore message backlog
ChoreMessage backlog[MAX_MESSAGES];

void setup() {
  // Initialize LCD and RTC (Liquid Crystal Display & Real Time Clock)
  lcd.begin(20, 4);  // Initialize the LCD with 20 columns and 4 rows
  rtc.begin();       // Initialize the RTC
  delay(500);

  // Check if the RTC date is before a specific date
  DateTime rtcDate = rtc.now();
  if (rtcDate.year() < 2023) {
    // Set the RTC to your desired default date
    rtc.adjust(DateTime(2023, 11, 19, 19, 42, 0));

    // Activates the pins of the Arduino
    pinMode(CONFIRM_PIN, INPUT_PULLUP);  // Set confirmation button pin mode to INPUT_PULLUP
    pinMode(RESET_PIN, INPUT_PULLUP);    // Set reset button pin mode to INPUT_PULLUP
    pinMode(BUZZER_PIN, OUTPUT);         // Set buzzer pin mode to OUTPUT

    // Initial delays for stabilization & hearing if the setup worked
    delay(500);
    tone(BUZZER_PIN, 1000, 250);
    delay(250);
    tone(BUZZER_PIN, 1000, 250);
    delay(250);
    tone(BUZZER_PIN, 1000, 250);
    delay(250);
    tone(BUZZER_PIN, 1000, 250);
    delay(250);
  }
}
//This is the main body of the program. It points to different functions and then goes back to the loop.
void loop() {
  DateTime now = rtc.now();  // Get the current time from the RTC
  delay(250);                // Delay for stability and to prevent unnecessary processing

  // Display messages at 5 AM
  if (now.hour() == MORNING_HOUR && now.minute() == MORNING_MINUTE && !hasDisplayedToday) {
    checkAndDisplayMessage(now);  // Call a function to check and display chore messages
    hasDisplayedToday = true;     // Set the flag to indicate that messages have been displayed today
  }

  // Handle button presses
  if (digitalRead(CONFIRM_PIN) == LOW || digitalRead(RESET_PIN) == LOW) {
    processButtonPress();       // Call a function to process button presses
    lcd.clear();                // Clear the LCD display
    hasDisplayedToday = false;  // Reset the flag for today's message display
  }

  // Turn on LCD backlight if there are pending messages
  if (!hasDisplayedToday && !lcdBacklightOn) {
    lcd.backlight();        // Turn on the LCD backlight
    lcdBacklightOn = true;  // Set the flag to indicate that the backlight is on
  }

  // Check for unread messages and activate buzzer if needed
  if (!hasDisplayedToday) {
    checkUnreadMessages(now);  // Call a function to check for unread messages and activate the buzzer
  }

  // Turn off LCD backlight if there are no pending messages
  if (!hasDisplayedToday && !lcdBacklightOn) {
    lcd.noBacklight();       // Turn off the LCD backlight
    lcdBacklightOn = false;  // Set the flag to indicate that the backlight is off
  }

  // Display messages one by one
  if (backlogSize > 0 && digitalRead(CONFIRM_PIN) == LOW) {
    displayNextMessage();
  } else if (backlogSize == 0 && lcdBacklightOn) {
    // Turn off LCD backlight when there are no more messages
    lcd.noBacklight();
    lcdBacklightOn = false;
  }
}

// Check and display chore messages based on the current time
void checkAndDisplayMessage(DateTime now) {
  MessageType message = NO_MESSAGE;

  if (now.dayOfTheWeek() == 1) {
    message = FEED_CHICKENS;  // Feed chickens every day
  } else if (now.dayOfTheWeek() == 7) {
    message = CLEAN_CHICKENS;  // Clean chickens every week
  } else if (now.day() == 13 && now.month() == 9) {
    message = CUT_THE_TREE;  // Cut the tree on 13th September
  } else if (now.day() % 3 == 0) {
    message = WATER_PLANTS;  // Water plants every 3 days
  } else if (message != NO_MESSAGE) {
    addToBacklog(now, message);  // Add the new message to the backlog
  } else {
    //niets was vantoepassing
  }
}

void addToBacklog(DateTime now, MessageType message) {
  // Check if there is space in the backlog
  if (backlogSize < MAX_MESSAGES) {
    backlog[backlogSize].timestamp = now;
    backlog[backlogSize].message = message;
    backlog[backlogSize].isRead = false;
    backlogSize++;
  } else {
    // Handle backlog overflow if needed
  }
}

void displayNextMessage() {
  lcdBacklightOn = true;
  lcd.backlight();

  // Convert String to const char*
  const char* message = backlog[currentMessageIndex].message.c_str();

  // Display the current message
  displayMessage(backlog[currentMessageIndex].timestamp, message);
  backlog[currentMessageIndex].isRead = true;

  // Wait for button press to acknowledge the message
  while (digitalRead(CONFIRM_PIN) == HIGH) {
    // You can add other tasks here if needed
  }

  // Move to the next message or reset if all messages are displayed
  currentMessageIndex++;
  if (currentMessageIndex >= backlogSize) {
    currentMessageIndex = 0;
    backlogSize = 0;  // Reset the backlog after displaying all messages
  }
  lcd.clear();
}


// Display a chore message on the LCD
void displayMessage(DateTime now, const char* message) {
  snprintf(timestampStr, sizeof(timestampStr), "%04d-%02d-%02d %02d:%02d",
           now.year(), now.month(), now.day(), now.hour(), now.minute());
  lcd.setCursor(0, 0);
  lcd.print(timestampStr);

  lcd.setCursor(0, 1);

  if (strcmp(message, "Water plants") == 0) {
    lcd.print("Water plants - Every 3 days");
  } else if (strcmp(message, "Feed chickens") == 0) {
    lcd.print("Feed chickens - Every day");
  } else if (strcmp(message, "Clean chickens") == 0) {
    lcd.print("Clean chickens - Every week");
  } else if (strcmp(message, "Cut the tree") == 0) {
    lcd.print("Cut the tree - Only on 13th September");
  } else {
    lcd.print("No message");
  }
}


// Check for unread messages and activate the buzzer if needed
void checkUnreadMessages(DateTime now) {
  for (int i = 0; i < backlogSize; ++i) {
    if (!backlog[i].isRead && now.unixtime() - backlog[i].timestamp.unixtime() > ONE_DAY_SECONDS) {
      activateBuzzer();
      return;
    }
  }
}

// Activate the buzzer (sound alert)
void activateBuzzer() {
  unsigned long currentTime = millis();
  if (currentTime - lastBuzzerActivationTime >= BUZZER_INTERVAL) {
    tone(BUZZER_PIN, 1000, 500);
    noTone(BUZZER_PIN);
    lastBuzzerActivationTime = currentTime;  // Update the last activation time
  }
}

// Process button presses
void processButtonPress() {
  if (digitalRead(CONFIRM_PIN) == LOW && digitalRead(RESET_PIN) == LOW) {
    deleteEntireBacklog();
  }
}

// Delete the entire backlog of messages
void deleteEntireBacklog() {
  lcd.print("Deleting backlog...");
  backlogSize = 0;
  lcd.clear();
  lcd.print("Backlog deleted!");
  delay(1000);
  lcd.clear();
}
