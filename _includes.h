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
int light=0;                    // LDR analog value
int sound=0;                    // sound sensor analog value
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
int pSound=0;

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
int sw = 0;                      // mini-orb switch
int swState = 0;                 // mini-orb switch state
int swCurrentState = 0;          // Monitor if states have been changed during each XBee communication
boolean swDown = false;          // switch state
boolean swBuzz = false;          // make feedback sound once when switch is pressed
boolean centreBuzz = false;      // make feedback sound when centre point of wheel is found
int wheel = 0;                   // mini-orb potentiometer
int prevWheel = 0;               // previous state of mini-orb potentiometer
boolean wheelMoved = false;      // wheel state (wheter it was moved or not)
int wheelPos = 0;                // position of wheel (0-3)
unsigned long wheelTimer = 0;    // timer to monitor change of wheel value
int s1=0, s2=0, s3=0, rgbState=0;// RGB led values and which led to change
int brightnessBeepPos = 0;       // beep position in the brightness setting 
int mode_matrix[4][3] = {        // sensor values for 3 modes (sensor, self-preference, group-preference)
    {128,128,128},
    {128,128,128},
    {128,128,128},
    {128,128,128}
  };
int _pp[] = {256,256,256,256};      // personal preferences [temp, light, sound, social] from own Orb setting
int _gp[] = {128,128,128,128};      // group preferences [temp, light, sound, social] from Xively
int matrixLocNumPrev = 0;        // previous sense value of where matrix position autoplay is at
unsigned long ledTimer = 0;      // timer to monitor RGB led transition time
unsigned long actionTimer = -10000;   // timer to monitor interaction
unsigned long switchTimer = 0;   // timer to monitor the last time the switch is pressed
unsigned long buzzTimer = 0;     // timer to randomise the instruciton buzz
int randomTime = 100;            // interval to play instruction buzz
int actionPeriod = 30000;        // time before actionTimer resets
int dimgrow = 0;                 // whether RGB LED is growing or dimming
int transition = 0;              // colour state of each RGB LED
int modeState = 0;               // which mode (sensor, you, others) in automatic display
int senseState = 0;              // which sensor info (temp, light, sound, social) in automatic display
int lightLevel = 255;            // leght level of RGB LED
int bRise = 0;
int bPitch = 0;
int bDuration = 0;               // alarm duration
int interacted = 0;                  // whether orb is interacted or not.

