/*
 * 
 * runs different patterns on different strips connected to different pins on the same WeMOS
 * 
 * based (a lot) on the FastLED DemoReel100 example
 * 
 * 
 */
 # include <NTPClient.h>
 #include <ESP8266WiFi.h>
 #include <WiFiUdp.h>

const char *ssid     = "<YOUR SSID>";
const char *password = "<YOUR PASSWORD>";
IPAddress staticIP(192,168,1,91);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#include <TimeLib.h>

#include <FastLED.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

// define the WeMOS pins for each strip
#define DATA_PIN1 D8
#define DATA_PIN2 D7
#define DATA_PIN3 D6
#define DATA_PIN4 D5

#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

// a 2D array of leds
#define NUM_LEDS    30
#define NUM_STRIPS 4
CRGB leds[NUM_STRIPS][NUM_LEDS];
CLEDController *strip[NUM_STRIPS]; // each strip has a different controller
bool ledsOn=false;

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND   30

// used by animation methods so they know which strip they are working on
uint8_t curStrip=0;

// open for business during these hours
#define ANIM_START_HOUR     18  // 6pm 24 hour clock
#define ANIM_END_HOUR       23


// connect to WiFi and init NTP, add the leds to FastLED

void setup() {
 Serial.begin(115200);

  // establish a wifi connection
  if (!WiFi.config(staticIP, gateway, subnet,gateway,gateway))
    {
      Serial.println("Unable to configure WiFi");
      while(1) ;
    }
  WiFi.begin(ssid, password);

  uint8_t lineBreak=0;
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
    lineBreak=(lineBreak+1)%40;
    if (lineBreak==0) Serial.println("");
  }

  Serial.print("\nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
  Serial.print("DNS #1, #2 IP: ");
  WiFi.dnsIP().printTo(Serial);
  Serial.print(", ");
  WiFi.dnsIP(1).printTo(Serial);
  Serial.println();
  
  WiFi.setAutoConnect(true);

  timeClient.begin();
  timeClient.forceUpdate();

  Serial.print("\nTime client updated Hour="); Serial.println(timeClient.getHours());
  
  // tell FastLED about the LED strip configuration
  strip[0]=&FastLED.addLeds<LED_TYPE,DATA_PIN1,COLOR_ORDER>(leds[0],NUM_LEDS).setCorrection(TypicalLEDStrip);
  strip[1]=&FastLED.addLeds<LED_TYPE,DATA_PIN2,COLOR_ORDER>(leds[1],NUM_LEDS).setCorrection(TypicalLEDStrip);
  strip[2]=&FastLED.addLeds<LED_TYPE,DATA_PIN3,COLOR_ORDER>(leds[2],NUM_LEDS).setCorrection(TypicalLEDStrip);
  strip[3]=&FastLED.addLeds<LED_TYPE,DATA_PIN4,COLOR_ORDER>(leds[3],NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // show we have started ok
  // also confirms all LEDs are working
  for (uint8_t thisStrip=0;thisStrip<NUM_STRIPS;thisStrip++){
          fill_solid(leds[thisStrip],NUM_LEDS,CRGB(255,255,255));
  }
  FastLED.show();
  delay(500);

  for (uint8_t thisStrip=0;thisStrip<NUM_STRIPS;thisStrip++){
          fill_solid(leds[thisStrip],NUM_LEDS,CRGB(0,0,0));
  }
  FastLED.show();

  ledsOn=false;

  Serial.println("Running");
}


// List of patterns to cycle through.  Each is defined as a separate function below.
// I guess this could be an array of groups of patterns so you could cycle through
// different patttern sets on each strip

void rainbow();
void confetti();
void blackWithGlitter();
void rainbowWithGlitter();
void sinelon();
void juggle();
void bpm();

typedef void (*SimplePatternList[])();

// blackWithGlitter added 4 times so that the patterm will switch from strip to strip
SimplePatternList gPatterns = { rainbow, confetti,rainbowWithGlitter,  sinelon, blackWithGlitter, blackWithGlitter, blackWithGlitter, blackWithGlitter,juggle, bpm };

// start pattern (index) to display on each strip first
uint8_t stripPattern[NUM_STRIPS]={0,1,2,3};

uint8_t gHue = 0; // rotating "base color" used by many of the patterns


void loop()
{
  timeClient.update();

  uint8_t now=timeClient.getHours();
  Serial.print("Now-Hours="); Serial.println(now);
  
  // only run the sketch between fixed hours
  //Serial.print("Led status "); Serial.println(ledsOn);
  
  if ( (now<ANIM_START_HOUR) || (now>=ANIM_END_HOUR)) {
      //Serial.println("Out of hours");
        if (ledsOn){
          ledsOn=false;
          Serial.print("Turning off the Leds at hour="); Serial.println(now);
          for (uint8_t thisStrip=0;thisStrip<NUM_STRIPS;thisStrip++){
            fill_solid(leds[thisStrip],NUM_LEDS,CRGB(0,0,0));
          }
        }
   }
   else {
    //Serial.println("On hours");
    if (!ledsOn){
      
      ledsOn=true;
      Serial.print("Switching leds back on at hour="); Serial.println(now);
      for (uint8_t thisStrip=0;thisStrip<NUM_STRIPS;thisStrip++){
          fill_solid(leds[thisStrip],NUM_LEDS,CRGB(255,255,255));
          stripPattern[thisStrip]=thisStrip;  // staggered andimation
        }
      
      gHue=0;
    }
    
    // Call the current pattern function once, updating the respective 'leds' array
  
    for (uint8_t thisStrip=0;thisStrip<NUM_STRIPS;thisStrip++){
        curStrip=thisStrip; // selects the led array in the animations
        gPatterns[stripPattern[thisStrip]]();
      }
      
    }

   FastLED.show();
   FastLED.delay(1000/FRAMES_PER_SECOND);
  // do some periodic updates
  if (ledsOn){
    EVERY_N_MILLISECONDS( 20 ) { gHue++; }    // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 10 ) { nextPattern(); }  // change patterns periodically
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number for each strip, and wrap around at the end
  for (uint8_t thisStrip=0;thisStrip<NUM_STRIPS;thisStrip++)
    {
    stripPattern[thisStrip]=(stripPattern[thisStrip] + 1) % ARRAY_SIZE( gPatterns);
    }
}


/*
 * The following are the animations as listed above in the gPattern array
 * 
 * each must operate on leds[curStrip] 
 */
 
void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds[curStrip], NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void blackWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  fill_solid(leds[curStrip],NUM_LEDS,CRGB(0,0,0));
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[curStrip][ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds[curStrip], NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[curStrip][pos] += CHSV( gHue + random8(64), 200, 255);
  addGlitter(80);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds[curStrip], NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[curStrip][pos] += CHSV( gHue, 255, 192);
  addGlitter(80);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[curStrip][i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
    addGlitter(80);
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds[curStrip], NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[curStrip][beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
    addGlitter(80);
}

void CheckWifi(const char *ssid) {
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
}
