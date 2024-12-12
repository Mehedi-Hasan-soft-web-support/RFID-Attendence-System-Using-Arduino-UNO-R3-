#include <SPI.h>
#include <MFRC522.h>

// Define RFID pins
#define RST_PIN 9  // Reset pin
#define SS_PIN 10  // Slave Select pin

// Define buzzer and LED pins
#define BUZZER_PIN 8
#define LED_PIN_A0 A0    // Authorized LED Pin (LED 1)
#define LED_PIN_A1 A1    // Unauthorized LED Pin (LED 2)

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Define the target UIDs for multiple cards
const byte targetUIDs[][4] = {
  {0xC3, 0x0F, 0x17, 0x05},  // Card 1 UID (Mehedi Hasan)
  {0x90, 0x8E, 0xA5, 0x53}   // Card 2 UID (Maria Rashid)
};

// Define the corresponding names and IDs for each card
const char* names[] = {
  "Md. Mehedi Hasan",   // Name for Card 1
  "Maria Rashid"        // Name for Card 2
};

const char* ids[] = {
  "232-15-497",         // ID for Card 1
  "232-15-160"          // ID for Card 2
};

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  while (!Serial);

  // Initialize SPI bus and RFID reader
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Place your card on the reader...");

  // Initialize buzzer and LED pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN_A0, OUTPUT);  // Authorized LED Pin
  pinMode(LED_PIN_A1, OUTPUT);  // Unauthorized LED Pin
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN_A0, LOW);
  digitalWrite(LED_PIN_A1, LOW);  // Ensure Unauthorized LED is off initially
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Print the detected UID
  String detectedUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    detectedUID += String(mfrc522.uid.uidByte[i], HEX);
    if (i != mfrc522.uid.size - 1) {
      detectedUID += ":";
    }
  }
  Serial.print("Card UID: ");
  Serial.println(detectedUID);

  // Check if the detected UID matches any of the target UIDs
  int cardIndex = getCardIndex(mfrc522.uid.uidByte, mfrc522.uid.size);
  if (cardIndex != -1) {
    Serial.print("Attendance Updated\nName: ");
    Serial.println(names[cardIndex]);
    Serial.print("ID: ");
    Serial.println(ids[cardIndex]);
    activateAuthorizedLED();  // Turn on Authorized LED (A0)
  } else {
    Serial.println("Unauthorized card.");
    activateUnauthorizedLED();  // Turn on Unauthorized LED (A1)
  }

  // Halt the card
  mfrc522.PICC_HaltA();
}

// Function to compare the detected UID with the target UIDs
int getCardIndex(byte *uid, byte uidSize) {
  for (int i = 0; i < sizeof(targetUIDs) / sizeof(targetUIDs[0]); i++) {
    if (uidSize == sizeof(targetUIDs[0])) {
      bool match = true;
      for (byte j = 0; j < uidSize; j++) {
        if (uid[j] != targetUIDs[i][j]) {
          match = false;
          break;
        }
      }
      if (match) {
        return i;  // Return the index of the matching card
      }
    }
  }
  return -1;  // No match found
}

// Function to activate the buzzer and authorized LED
void activateAuthorizedLED() {
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(LED_PIN_A0, HIGH);  // Turn on Authorized LED (A0)
  delay(500);  // Buzzer and LED on for 500ms
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN_A0, LOW);  // Turn off Authorized LED (A0)
}

// Function to activate the Unauthorized LED
void activateUnauthorizedLED() {
  digitalWrite(LED_PIN_A1, HIGH);  // Turn on Unauthorized LED (A1)
  delay(500);  // LED stays on for 500ms
  digitalWrite(LED_PIN_A1, LOW);   // Turn off Unauthorized LED (A1)
}

