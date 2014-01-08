#define devicePayLoad 19        // number of sensors (8 x 2bytes) + device ID and sensor number (2bytes) + orb interaction (1 byte).
                                // sensors are - sound, light, potentiometer, voltage, 
                                // temperature, humidity, joystick (total: 7)

//===== analog pins
#define soundPin A0             // sound sensor
#define lightPin A1             // light sensor
#define potPin   A2             // potentiometer 
#define vmPin    A3             // voltage (battery) monitor pin
#define ab       A4             // mini-orb analog port one
#define aa       A5             // mini-orb analog port two

//===== digital pins

// (ports below for original configuration without mini-orb)
#define xrssi    2                // RSSI signal of XBee
#define xreset   3                // Reset pin of XBee
#define led1     4                // LED 1
#define notused1 5                // not defined
#define notused2 6                // not defined
int jstickpins[] = {8,9,10,11,12};// joystick pins - centre push, up, down, left, right

// (ports below for mini-orb configuration)
#define ledPin1  2                // mini-orb charlieplexed led 1
#define blPin    3                // mini-orb RGB blue
#define ledPin2  4                // mini-orb charlieplexed led 2
#define rePin    5                // mini-orb RGB red
#define grPin    6                // mini-orb RGB green
#define buzPin   8                // mini-orb buzzer 

// (for both configurations)
#define dhtPin   7                // Temp and Humidity Sensor
#define led2     13               // LED on Arduinos

//===== Setup a DHT instance and initialise
DHT22 myDHT22(dhtPin);
DHT22_ERROR_t errorCode;
unsigned long dhtTimer=0;
const int dhtInterval=2500;

//===== sensor variables
int light=0;                    // LDR average analog value of last 30 1/s samples
int lights[30];                 // 30 samples of light sensor value
int sound=0;                    // sound sensor average analog value of last 30 1/s samples
int sounds[30];                 // 30 samples of sound sensor value
int temp=0;                     // temperature sensor temperature value
int humid=0;                    // humidity sensor humidity value
int pot=0;                      // potentiometer value
int rssi=0;                     // XBee signal strength
int vm=0;                       // voltage (battery) value
int jsbyte = 0;                 // 5 bits representing which pin is switched
int jstick[] = {0,0,0,0,0};     // Joystick condition (centre, up, down, left, right)
unsigned long sensorTimer=0;    // timer for checking sensors
const int sensorInterval=1000;  // interval (ms) for checking sensors

//===== keeping some of previous sensor values
int pLight=0;

//===== XBee wireless communication
XBee xbee = XBee();
uint8_t payload[devicePayLoad]; // (number of sensors x 2) + 2. "+2" for device ID, sensor number, and orb interaction.
//XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x402D82CA);
XBeeAddress64 addr64 = XBeeAddress64(0x00, 0x00);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBRxResponse rx = ZBRxResponse();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
ModemStatusResponse msr = ModemStatusResponse();
int remoteData[10];

//===== Mini-Orb
int wheel = 0;                   // mini-orb potentiometer
int prevWheel = 0;               // previous state of mini-orb potentiometer
int _pp[] = {256,256,256,256};   // personal preferences [temp, light, sound, social] from own Orb setting
int _gp[] = {128,128,128,128};   // group preferences [temp, light, sound, social] from Xively
int bRise = 0;                   // buzzer: how many time the original pitch is to be doubled
int bPitch = 0;                  // buzzer: initial pitch - or to be more precise, duration of half-cycle in microseconds
int bDuration = 0;               // buzzer: how many times the pitch is to play
int interacted = 0;              // whether orb is interacted or not.

