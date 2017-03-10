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
  
  pinMode(SW2_pin, INPUT);
  
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
    *x = 7 - (*x);//8x8 grid is in a snake-like unidirectional form
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
  
  int x, y, x_raw, y_raw, num;

  if(which_player == 0) {
    x_raw = analogRead(X1_pin);
    y_raw = analogRead(Y1_pin);
    *Switch = digitalRead(SW1_pin);
  }
  else {
    x_raw = analogRead(X2_pin);
    y_raw = analogRead(Y2_pin);
    *Switch = digitalRead(SW2_pin);
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

  // Display cursor on the 8x8 Grid  
  num = coord_to_led(&x, &y); // led number
  leds[num] = HalfGreen; // cursor colour is green
  FastLED.show();
  
  *_x = x; *_y = y;
  
}



// Adds all the cursor movements till the joystick is clicked
void finalCoord(int which_player, int*a, int*b){
  
  int x, y, Switch;
  do{
    
    get_input(which_player, &Switch, &x, &y);
    a += x;
    b += y;
    
  }while(Switch != 1);
  
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



// Conditions to win the game!!
boolean game_End(){
  
  boolean status = false;
  int ctr_row = 0, ctr_diagonal = 0;
  ledType temp;
  
  for(int i = 1; i<8; i++){
    
    for(int j = 0; j<7; j++){
      
      if(board_colour[i-1][j].num < board_colour[i][j+1].num){
        
        temp = board_colour[i-1][j];
        board_colour[i-1][j] = board_colour[i][j+1];
        board_colour[i][j+1] = temp;
        
      }//if condition to put the loop in acsending order
      
    }//column using inner for loop
    
  }//row using outer for loop


  for(int i = 1; i<9; i++){
    
    for(int j = 0; j<7; j++){
      
      if(board_colour[i-1][j].num = board_colour[i-1][j+1].num){
        ctr_row++;
      }
      else ctr_row = 0;
      
      if(ctr_row == 4){
        status = true;
        return status;  
      }
      
    }//column using inner for loop
    
  }//row using outer for loop

 
  for(int i = 0; i<4; i++){
 
    int j = 0, temp_i = i;
    do{
  
      if(board_colour[temp_i][j].num = board_colour[temp_i+1][j+1].num) {
        ctr_diagonal++;
      }
      else ctr_diagonal = 0;

      if(ctr_diagonal == 4){
        status = true;
        return status;  
      }
      j++;
      temp_i++;
      
    }while(temp_i<7);    
     
  }//row using outer for loop

  for(int i = 0; i<4; i++){
 
    int j = 7, temp_i = i;
    do{
  
      if(board_colour[temp_i][j].num = board_colour[temp_i+1][j-1].num) {
        ctr_diagonal++;
      }
      else ctr_diagonal = 0;

      if(ctr_diagonal == 4){
        status = true;
        return status;  
      }
      j--;
      temp_i++;
      
    }while(temp_i<7);    
     
  }//row using outer for loop
  
  return status;

}



// To repeatedly run the desired code
void loop() {
  
  for(int i = 0; i<8; i++){
    
    for(int j = 0; j<8; j++){
      leds[board_colour[i][j].num] = board_colour[i][j].colour;
    }//column using inner for loop
    
  }//row using outer for loop

  if(game_End()){
    //display win
  }
  
  FastLED.show();
  
}
