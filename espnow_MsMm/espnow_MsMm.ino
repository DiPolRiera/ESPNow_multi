#include <esp_now.h>
#include <WiFi.h>


// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress2[] = {0xC4, 0x4F, 0x33, 0x08, 0x3F, 0x59};   // PORT 11 AZUL
uint8_t broadcastAddress3[] = {0xCC, 0x50, 0xE3, 0xAB, 0xB8, 0xB8};   // PORT 5 NEGRO
//uint8_t broadcastAddress3[] = {0x9C, 0x9C, 0x1F, 0xC6, 0x15, 0xA8};   // PORT 12 ROTO
//uint8_t broadcastAddress[] = {0xFF, , , , , };

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id; // must be unique for each sender board
    int x;
    int y;
} struct_message;

//Create a struct_message called myData
struct_message myData;
//struct_message incomingData;


// send data one to many
struct_message send_data1;
struct_message send_data2;
struct_message send_data3;
  
// Create a structure to hold the readings from each board (read data many to one)
struct_message board1;
struct_message board2;
struct_message board3;

// Create a structure to hold the readings from each board
struct_message boardsStruct[3] = {board1, board2, board3};


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println();
}


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].x = myData.x;
  boardsStruct[myData.id-1].y = myData.y;
  Serial.printf("x value received: %d \n", boardsStruct[myData.id-1].x);
  Serial.printf("y value received: %d \n", boardsStruct[myData.id-1].y);
  Serial.println();
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // register first peer  
  /*memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }*/
  // register second peer  
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  /// register third peer
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

}
 
void loop() {

  send_data1.x = random(40,60);
  send_data1.y = random(40,60);
  send_data2.x = random(40,60);
  send_data2.y = random(40,60);
  send_data3.x = random(40,60);
  send_data3.y = random(40,60);  
  /*
  // Set values to send
  myData.id = 1;
  myData.x = random(0,50);
  myData.y = random(0,50);*/

  // Send message via ESP-NOW
  esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *) &send_data3, sizeof(send_data3));
   
  if (result3 == ESP_OK) {
    Serial.printf("Data X 3 sent: %d ,Sent 3 with success \n", send_data3.x);
    Serial.printf("Data Y 3 sent: %d ,Sent 3 with success \n", send_data3.y);
  }
  else {
    Serial.println("Error 3 sending the data");
  }
  delay(500);
  // Send message via ESP-NOW
  esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &send_data2, sizeof(send_data2));
   
  if (result2 == ESP_OK) {
    Serial.printf("Data X 2 sent: %d ,Sent 2 with success \n", send_data2.x);
    Serial.printf("Data Y 2 sent: %d ,Sent 2 with success \n", send_data2.y);
  }
  else {
    Serial.println("Error 2 sending the data");
  }
  
  delay(60000);
}
