#include <WiFi.h>
#include <ESP_Mail_Client.h>

// WiFi credentials
#define WIFI_SSID "Your Wifi Name"
#define WIFI_PASSWORD "Your WIfi password"

// SMTP credentials
#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465
#define sender_email "sender gmail"
#define sender_password "sender password" //This password is not your gmail password. 
#define Recipient_email "receiver gmail"
#define Recipient_name ""

// Motion sensor and buzzer pins
#define MOTION_SENSOR_PIN 22
#define BUZZER_PIN 21

SMTPSession smtp;

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to WiFi...");
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nWiFi connected.");
  Serial.println("IP address: " + WiFi.localIP().toString());

  // Set up motion sensor and buzzer
  pinMode(MOTION_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Enable debugging for SMTP
  smtp.debug(1);
}

void loop() {
  // Check if motion is detected
  if (digitalRead(MOTION_SENSOR_PIN) == HIGH) {
    Serial.println("Motion detected!");

    // Activate the buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500); // Buzzer sound duration
    digitalWrite(BUZZER_PIN, LOW);

    // Send email notification
    sendEmail();
    delay(10000); // Delay to prevent multiple emails in quick succession
  }
  delay(100); // Short delay to avoid bouncing issues
}

void sendEmail() {
  ESP_Mail_Session session;
  session.server.host_name = SMTP_server;
  session.server.port = SMTP_Port;
  session.login.email = sender_email;
  session.login.password = sender_password;
  session.login.user_domain = "";

  SMTP_Message message;
  message.sender.name = "HOME SECURITY";
  message.sender.email = sender_email;
  message.subject = "Alert!";
  message.addRecipient(Recipient_name, Recipient_email);

  // HTML message content
  String htmlMsg = "<div style=\"color:#000000;\"><h1>Security Breached!</h1><p>Motion Detected!</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  // Connect to the SMTP server and send the email
  if (!smtp.connect(&session)) {
    Serial.println("Failed to connect to SMTP server.");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error sending Email: " + smtp.errorReason());
  } else {
    Serial.println("Email sent successfully!");
  }
}
