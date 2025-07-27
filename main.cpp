
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

#define ESPNOW_DEFTXCH 1
#define SERIAL_DEFBAUD 9600

#define PREFKEY_WIFICH "espnow-ch"
#define PREFKEY_BAUDRATE "serial-baud"

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

void setup() {

}

void loop() {

}