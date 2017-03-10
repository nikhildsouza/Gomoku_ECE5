#include "FastLED.h"
#define NUM_LEDS 64 // 8x8 grid
#define LED_PIN 10  // datapin
#define LED_TYPE    WS2812B

CRGB leds[NUM_LEDS];

const CRGB HalfRed = {50,15,0};
const CRGB HalfBlue = {0,15,50};
const CRGB HalfGreen = {30,100,30};
CRGB Colour;

const int SW1_pin = 3; // digital pin connected to SW
const int X1_pin = A0; // analog pin connected to VRx
const int Y1_pin = A1; // analog pin connected to VRy

const int SW2_pin = 4; // digital pin connected to SW
const int X2_pin = A2; // analog pin connected to VRx
const int Y2_pin = A3; // analog pin connected to VRy

struct ledType{
  int num;
  CRGB colour;
};

int cursor_x, cursor_y;
ledType board_colour[8][8];



// To setup all relevant parts 
void setup() {
   
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  
  pinMode(SW1_pin, INPUT);
  digitalWrite(SW1_pin, HIGH);
  
  pinMode(SW2_pin, INPUT);
  digitalWrite(SW2_pin, HIGH);
  
  Serial.begin(9600);//for higher speed - 115200

}



// To convert numbers to coordinates on a 8x8 Grid
void led_to_coord(int ledno, int*x, int*y){
  
  *y = ledno /8;   //row
  *x = ledno % 8;  //column
  if ((*y%2) ==1 ) //check for odd row numbers
    *x = 7 - *x; //8x8 grid is in a snake-like unidirectional form
}



// To convert numbers to coordinates on a 8x8 Grid
int coord_to_led(int*x, int*y){
  
  if ( ((*y)%2) == 1 ){ //check for odd row numbers
    *x = 8 - (*x);//8x8 grid is in a snake-like unidirectional form
  }
  
  int ledno = (*y-1)*8 + *x;
}



// To display the correct LED choice
CRGB draw_out(int which_player,int ledno)
{
  
  if(which_player==1){
    Colour = HalfRed;
  }else{
    Colour = HalfBlue;
  }
  
  leds[ledno] = Colour;
  
  FastLED.show();

  return Colour;
}



// To obtain the users' choice
void get_input(int which_player, int*Switch, int* _x, int* _y )
{
  int x, y, x_raw, y_raw;

  if(which_player == 0) {
    x_raw = analogRead(X1_pin);
    y_raw = analogRead(Y1_pin);
  }else {
    x_raw = analogRead(X2_pin);
    y_raw = analogRead(Y2_pin);
  }
  //analogRead returns a range from 0 to 1023
  //500 is midpoint where nothing is done to joystick
  
  if(x_raw>700)
    x = -1;
  else if (x_raw<300)
    x = 1;
  else
    x =0;

  if(y_raw>700)
    y = -1;
  else if (y_raw <300)
    y = 1;
  else
    y=0;
    
  //read the switch
    
  *_x = x; *_y = y;
}


// Adds all the cursor movements till the joystick is clicked
void finalCoord(int which_player, int*a, int*b){
  int x, y, Switch;
  do{
    get_input(which_player, &Switch, &x, &y);
    a += x;
    b += y;
    // get input for Switch
  }while(Switch != 0);
}



//Set the permanent colour of the led board
void boardColour(int*x, int*y){
  
}


CRGB c;
//execute the game once
void getPoint(){

  int x, y, which_player, ledno;
  finalCoord(which_player, &x, &y);
  ledno = coord_to_led(&x, &y);
  c = draw_out(which_player, ledno);
  board_colour[x][y].num = ledno;
  board_colour[x][y].colour = c;
}



// To repeatedly run the desired code
void loop() {
  
  for(int i = 0; i<8; i++){
    
    for(int j = 0; j<8; j++){
      leds[board_colour[i][j].num] = board_colour[i][j].colour;
    }//column using inner for loop
    
  }//row using outer for loop

  FastLED.show();
  
}
