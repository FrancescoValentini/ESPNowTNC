#include <esp_now.h>
#include <WiFi.h>
#include <Preferences.h>
/*
=================================
CONSTANTS
=================================
*/

#define CSMA_RADIO_DEFAULT_P 255
#define CSMA_RADIO_DEFAULT_SLOT_TIME 0

#define MAIN_LOOP_SLEEP_MS 10
#define BUFFER 192 // Maximum payload size of a single packet
#define KISS_BUFFER_SIZE 2048
#define MAX_MESSAGE_SIZE 4096 // Maximum size of receivable data

#define ESPNOW_DEFCHAN 1
#define SERIAL_DEFBAUD 9600

#define PREFERENCES_NAMESPACE "ESPNOWTNC"
#define PREFKEY_WIFICH "espnow-ch"
#define PREFKEY_BAUDRATE "serial-baud"

#define DEBUG true

/*
=================================
DATA STRUCTURES
=================================
*/
// KISS (Keep It Simple, Stupid) protocol markers used for frame encapsulation
enum KissMarker {
  Fend = 0xC0,  // Frame End marker - indicates the end of a KISS frame
  Fesc = 0xDB,  // Frame Escape marker - indicates that the next byte is escaped
  Tfend = 0xDC, // Transposed Frame End - escaped version of Fend
  Tfesc = 0xDD  // Transposed Frame Escape - escaped version of Fesc
};

enum KissState {
  Void = 0,     // Initial state, waiting for a frame start
  GetCmd,       // Received FEND, waiting for command byte
  GetData,      // Receiving data payload
  GetP,         // Receiving P (persistence) parameter
  GetSlotTime,  // Receiving slot time parameter
  Escape        // Received FESC, processing escaped byte
};

// States for the KISS protocol parser state machine
enum KissCmd {
  Data = 0x00,          // Data frame containing actual payload data
  P = 0x02,             // Set persistence parameter (P) for CSMA
  SlotTime = 0x03,      // Set slot time parameter for CSMA
  SetChannel = 0x04,    // Set radio channel
  RestartRadio = 0x05,  // Restart radio device
  NoCmd = 0x80          // No command/invalid command
};

// Packet that is sent with ESPNOW
typedef struct struct_message {
    uint16_t totalLen;      // Total length of the frame kiss
    uint16_t fragmentN;     // Number of the transmitted fragment
    char payload[BUFFER];   // Packet payload
} struct_message;

/*
=================================
GLOBAL VARIABLES
=================================
*/

// ESPNow Broadcast Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

struct_message packet;

// Variables used to reassemble the complete frame
uint8_t reassemblyBuffer[MAX_MESSAGE_SIZE];
int reassemblyExpectedLen = -1;
int reassemblyCurrentLen = 0;

// KISS related variables
KissState kissState_;
KissCmd kissCmd_;
uint8_t kissBuffer_[KISS_BUFFER_SIZE];
int kissBufferPosition_ = 0;

// CSMA related variables
byte csmaP_ = CSMA_RADIO_DEFAULT_P;
long csmaSlotTime_ = CSMA_RADIO_DEFAULT_SLOT_TIME;
long csmaSlotTimePrev_ = 0;

// Debug-only function to print data in hexadecimal format
#if DEBUG
    void printHex(const uint8_t *data, int len){
        for (int i = 0; i < len; i++){
            uint8_t n = data[i];
            if(n<0x10) Serial.print("0");
            Serial.print(n,HEX);
        }
    }
#endif

// Callback function for when data is sent via ESP-NOW
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    #if DEBUG
        Serial.println("[DEBUG] TX");
        Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    #endif
}

// Callback function for when data is received via ESP-NOW
void OnDataRecv(const esp_now_recv_info *recvInfo, const uint8_t *incomingData, int len) {
    #if DEBUG
        Serial.print("[DEBUG] RX - ");
        Serial.print(len);
        Serial.println(" Bytes.");
        printHex(incomingData, len);
        Serial.println("");
    #endif
}

// Function to transmit data by fragmenting it into smaller packets
void txData() {
    int totalLen = kissBufferPosition_;
    #if DEBUG
        Serial.print("[DEBUG] TX: ");
        Serial.print(totalLen);
        Serial.print(" Bytes");
    #endif

    // Calculate number of fragments needed
    int fragments = (totalLen + BUFFER - 1) / BUFFER;

    for (int i = 0; i < fragments; i++) {
        // Calculate offset position in the source buffer
        int offset = i * BUFFER;

        // Determine fragment length (either full BUFFER size or remaining data)
        int len = min(BUFFER, totalLen - offset);

        packet.totalLen = totalLen;    // Store total original data length
        packet.fragmentN = i;          // Store current fragment number
        
        // Copy data fragment from source buffer to transmission payload
        memcpy(packet.payload, kissBuffer_ + offset, len);

        // Send the fragment via ESP-NOW
        esp_now_send(broadcastAddress, (uint8_t *) &packet, sizeof(packet));

        #if DEBUG
            Serial.print("[DEBUG] TX Fragment n. ");
            Serial.print(i);
            Serial.print(" / ");
            Serial.print(fragments);
            Serial.println();
        #endif

        delay(1);
    }
}

// Initializes serial communication with configured baud rate.
void initSerial() {
    Preferences prefs;
    prefs.begin(PREFERENCES_NAMESPACE, true);  // true = read-only mode
    uint32_t baud = prefs.getUInt(PREFKEY_BAUDRATE, SERIAL_DEFBAUD);
    prefs.end();

    Serial.begin(baud);
    
    #if DEBUG
        Serial.print("[DEBUG] Initialized serial port, Baudrate: ");
        Serial.println(baud);
    #endif
}

/**
 * @brief Retrieves the WiFi channel from persistent storage
 * 
 * Reads the stored WiFi channel value from non-volatile storage. If no value is found,
 * returns the default ESPNOW channel.
 * 
 * @return uint8_t The WiFi channel to use for ESPNOW communication
 */
uint8_t getWifiChannel() {
    Preferences prefs;
    prefs.begin(PREFERENCES_NAMESPACE, true);  // true = read-only mode
    uint8_t wifiChannel = prefs.getUChar(PREFKEY_WIFICH, ESPNOW_DEFCHAN);
    prefs.end();

    #if DEBUG
        Serial.print("[DEBUG] ESPNOW Channel: ");
        Serial.println(wifiChannel);        
    #endif

    return wifiChannel;
}

/**
 * @brief Initializes ESP-NOW communication
 * 
 * Sets up WiFi in station mode, initializes ESP-NOW, registers callbacks,
 * and adds a broadcast peer for communication.
 */
void initESPNow(){
    WiFi.mode(WIFI_STA); // Set WiFi mode to station (no AP)
    WiFi.disconnect(); // Disconnect from any previously connected networks

    // Initialize ESP-NOW and check for errors
    if (esp_now_init() != ESP_OK) {
        Serial.println("[X] Error initializing ESP-NOW");
        return;
    }

    #if DEBUG
        Serial.println("[DEBUG] ESPNOW READY!");     
    #endif

    // Register callback functions for sent and received data
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // Configure peer information for broadcast communication
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));  // Zero out the peerInfo structure
    
    // Set broadcast address and channel
    memcpy(peerInfo.peer_addr, broadcastAddress, 6); // Copy broadcast MAC address
    peerInfo.channel = getWifiChannel();             // Get channel from storage
    peerInfo.encrypt = false;                        // No encryption
    
    // Add the broadcast peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("[X] Failed to add peer");
        return;
    }

    #if DEBUG
        Serial.println("[DEBUG] Added broadcast peer!");     
    #endif
}

void setup() {
    initSerial(); // Initializes the serial port
    while(!Serial);
    initESPNow(); // Initializes ESPNow
}

void loop() {
    long currentTime = millis();
    if (currentTime > csmaSlotTimePrev_ + csmaSlotTime_ && random(0, 255) < csmaP_) {
        if (Serial.available()) {
            onSerialDataAvailable();
        }
        csmaSlotTimePrev_ = currentTime;
    }
    delay(MAIN_LOOP_SLEEP_MS);
}

// Reset the KISS state machine
void kissResetState(){
    kissCmd_ = KissCmd::NoCmd;
    kissState_ = KissState::Void;
}

// The KISS parser state machine
// TODO: implement custom commands
void onSerialDataAvailable() {
    while (Serial.available()) {
        int rx = Serial.read();
        if (rx < 0) break;
        byte b = (byte) rx;

        switch (kissState_) {
        case KissState::Void:
            if (b == KissMarker::Fend) {
                kissCmd_ = KissCmd::NoCmd;
                kissState_ = KissState::GetCmd;
            }
            break;

        case KissState::GetCmd:
            if (b != KissMarker::Fend) {
                if (b == KissCmd::Data) {
                    kissCmd_ = KissCmd::Data;
                    kissState_ = KissState::GetData;
                } else if (b == KissCmd::P) {
                    kissCmd_ = KissCmd::P;
                    kissState_ = KissState::GetP;
                } else if (b == KissCmd::SlotTime) {
                    kissCmd_ = KissCmd::SlotTime;
                    kissState_ = KissState::GetSlotTime;
                } else kissResetState();
            }
            break;

        case KissState::GetP:
            csmaP_ = b;
            kissState_ = KissState::GetData;
            break;

        case KissState::GetSlotTime:
            csmaSlotTime_ = (long) b * 10;
            kissState_ = KissState::GetData;
            break;

        case KissState::GetData:
            if (b == KissMarker::Fesc) {
                kissState_ = KissState::Escape;
            } else if (b == KissMarker::Fend) { // TX
                if (kissCmd_ == KissCmd::Data && kissBufferPosition_ > 0) {
                    txData();
                }
                kissBufferPosition_ = 0;
                kissResetState();
            } else if (kissCmd_ == KissCmd::Data) {
                if (kissBufferPosition_ < KISS_BUFFER_SIZE) {
                    kissBuffer_[kissBufferPosition_++] = b;
                } else {
                    kissBufferPosition_ = 0;
                    kissResetState();
                }
            }
            break;

        case KissState::Escape:
            if (b == KissMarker::Tfend) {
                if (kissBufferPosition_ < KISS_BUFFER_SIZE)
                    kissBuffer_[kissBufferPosition_++] = KissMarker::Fend;
            } else if (b == KissMarker::Tfesc) {
                if (kissBufferPosition_ < KISS_BUFFER_SIZE)
                    kissBuffer_[kissBufferPosition_++] = KissMarker::Fesc;
            }
            kissState_ = KissState::GetData;
            break;

        default:
            break;
        }
    }
}
