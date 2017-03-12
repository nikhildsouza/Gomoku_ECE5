#include "FastLED.h"
#define NUM_LEDS 64 // 8x8 grid
#define LED_PIN 10  // datapin
#define LED_TYPE    WS2812B

CRGB leds[NUM_LEDS];

const CRGB HalfRed = {100,0,0};
const CRGB HalfBlue = {0,0,100};
const CRGB HalfGreen = {30,100,30};
const CRGB Blank = {0,0,0};
CRGB Colour;

const int SW1_pin = A4; // digital pin connected to SW
const int X1_pin = A0; // analog pin connected to VRx
const int Y1_pin = A1; // analog pin connected to VRy

const int SW2_pin = A5; // digital pin connected to SW
const int X2_pin = A2; // analog pin connected to VRx
const int Y2_pin = A3; // analog pin connected to VRy

struct ledType{
  int num;
  CRGB colour;
};


int cursor_x=0, cursor_y=0, player = 1;
ledType board_colour[8][8];
ledType player_O[8][8];
ledType player_T[8][8];



// To setup all relevant parts 
void setup() {
  
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  
  pinMode(SW1_pin, INPUT);
  
  pinMode(SW2_pin, INPUT);
  

  for(int i = 0; i<8; i++){
    
    for(int j = 0; j<8; j++){
      board_colour[i][j].num = coord_to_led(&i, &j);
      player_O[i][j].num = coord_to_led(&i, &j);
      player_T[i][j].num = coord_to_led(&i, &j);
      board_colour[i][j].colour = Blank;
      player_O[i][j].colour = Blank;
      player_T[i][j].colour = Blank;
    }//column using inner for loop
    
  }//row using outer for loop
  
  
  Serial.begin(9600);//for higher speed - 115200

  
  /*
  for(int i = 0; i<64; i++)
  {
    leds[i] = CHSV(i*3,240,100);
  }
  FastLED.show();
  while(1);
  */
}



// To convert numbers to coordinates on a 8x8 Grid
void led_to_coord(int ledno, int*x, int*y){
  
  *y = ledno /8;   //row
  *x = ledno % 8;  //column
  if ((*y%2) == 1 ) //check for odd row numbers
    *x = 7 - *x; //8x8 grid is in a snake-like unidirectional form
}



// To convert numbers to coordinates on a 8x8 Grid
int coord_to_led(int*x, int*y){
  
  if ( ((*y)%2) == 1 ){ //check for odd row numbers
    *x = 7 - (*x);//8x8 grid is in a snake-like unidirectional form
  }
  
  int ledno = (*y)*8 + *x;
  return ledno;
}



// To display the correct LED choice
CRGB draw_out(int which_player,int ledno)
{
  
  if(which_player == -1){
    Colour = HalfRed;
  }else{
    Colour = HalfBlue;
  }

  return Colour;
}



// To obtain the users' choice
void get_input(int which_player, int*Switch, int* _x, int* _y )
{
  
  int x, y, x_raw, y_raw;

  if(which_player == -1) {
    x_raw = analogRead(X1_pin);
    y_raw = analogRead(Y1_pin);
    *Switch = analogRead(SW1_pin);
  }
  else {
    x_raw = analogRead(X2_pin);
    y_raw = analogRead(Y2_pin);
    *Switch = analogRead(SW2_pin);
  }
  //analogRead returns a range from 0 to 1023
  //500 is midpoint where nothing is done to joystick
  
  if(x_raw>700)
    x = -1;
  else if (x_raw<300)
    x = 1;
  else
    x = 0;

  if(y_raw>700)
    y = -1;
  else if (y_raw <300)
    y = 1;
  else
    y= 0;
  
  *_x = x; *_y = y;
  
}



// Adds all the cursor movements till the joystick is clicked
void finalCoord(int which_player/*, int*a, int*b*/){
  
  int x = 0, y = 0, Switch=0, c=0, d=0;
      cursor_x = 0;
    cursor_y = 0;
  
  do{
    
    // Display cursor on the 8x8 Grid  

    boardColour();
  
    get_input(which_player, &Switch, &x, &y);
    /**a += x, *b += y; 
    if(*a>7) *a=7;
    if(*a<0) *a=0;
    if(*b>7) *b=7;
    if(*b<0) *b=0;
    c = *a, d = *b;*/
    cursor_x += x, cursor_y += y; 
    if(cursor_x>7) cursor_x=7;
    if(cursor_x<0) cursor_x=0;
    if(cursor_y>7) cursor_y=7;
    if(cursor_y<0) cursor_y=0;
    delay(200);
  }while(Switch != 0);
  
}



//Set the permanent colour of the led board
void boardColour(){

  for(int i = 0; i<8; i++){
    
    for(int j = 0; j<8; j++){
      leds[board_colour[i][j].num] = board_colour[i][j].colour;
    }//column using inner for loop
    
  }//row using outer for loop
  leds[board_colour[cursor_x][cursor_y].num] = HalfGreen;
  FastLED.show();
  
}



//execute the game once
void getPoint(int which_player){

  int x, y, ledno;
  finalCoord(which_player/*, &x, &y*/);
  //ledno = coord_to_led(&x, &y);
  //board_colour[x][y].num = ledno;
  board_colour[cursor_x][cursor_y].colour = draw_out(which_player, board_colour[cursor_x][cursor_y].num);
  
}



// Conditions to win the game!!
boolean game_End(){
  
  boolean status = false;
  int ctr_row = 0, ctr_diagonal = 0, ctr_column = 0;
  ledType temp;
  /*
  for(int i = 1; i<8; i++){
    
    for(int j = 0; j<7; j++){
      
      if(board_colour[i-1][j].num < board_colour[i][j+1].num){
        
        temp = board_colour[i-1][j];
        board_colour[i-1][j] = board_colour[i][j+1];
        board_colour[i][j+1] = temp;
        
      }//if condition to put the loop in acsending order
      
    }//column using inner for loop
    
  }//row using outer for loop*/

  //CHECKING ROW WIN CONDITION
  for(int i = 1; i<9; i++){//CHECKING ROW WIN CONDITION
    
    for(int j = 0; j<7; j++){
      
      if(board_colour[i-1][j].colour == board_colour[i-1][j+1].colour && board_colour[i-1][j+1].colour != Blank){
        ctr_row++;
      }
      else ctr_row = 0;
      
      if(ctr_row == 4){
        status = true;
        return status;  
      }
      
    }//column using inner for loop
    
  }//row using outer for loop



  //CHECKING Column WIN CONDITION 
  for(int j = 0; j<7; j++){
     for (int i = 0; i < 9; i++){
      
        if(board_colour[i-1][j].colour == board_colour[i-1][j+1].colour && board_colour[i-1][j+1].colour != Blank){
          ctr_column++;
        }
        else ctr_column = 0;
        
        if(ctr_column == 4){
          status = true;
          return status;  
        }
      
    }//column using inner for loop
  }


  //CHECKING FORWARD DIAGONAL WIN CONDITION FOR +VE
  for(int i = 0; i<4; i++){
 
    int j = 0, temp_i = i;
    do{
  
      if(board_colour[temp_i][j].colour == board_colour[temp_i+1][j+1].colour && board_colour[temp_i+1][j+1].colour != Blank) {
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


  //CHECKING FORWARD DIAGONAL WIN CONDITION FOR -VE
  for(int j = 0; j<4; j++){
 
    int i = 0, temp_j = j;
    do{
  
      if(board_colour[i][temp_j].colour == board_colour[i+1][temp_j+1].colour && board_colour[i+1][temp_j+1].colour != Blank) {
        ctr_diagonal++;
      }
      else ctr_diagonal = 0;

      if(ctr_diagonal == 4){
        status = true;
        return status;  
      }
      i++;
      temp_j++;
      
    }while(temp_j<7);    
     
  }//row using outer for loop



  //CHECKING NEGATIVE DIAGONAL WIN CONDITION FOR +VE
  for(int i = 0; i<4; i++){
 
    int j = 7, temp_i = i;
    do{
  
      if(board_colour[temp_i][j].colour == board_colour[temp_i+1][j-1].colour && board_colour[temp_i][j].colour != Blank) {
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


   //CHECKING NEGATIVE DIAGONAL WIN CONDITION FOR -VE
  for(int j = 0; j<4; j++){
 
    int i = 7, temp_j = j;
    do{
  
      if(board_colour[i][temp_j].colour == board_colour[i+1][temp_j-1].colour && board_colour[i][temp_j].colour != Blank) {
        ctr_diagonal++;
      }
      else ctr_diagonal = 0;

      if(ctr_diagonal == 4){
        status = true;
        return status;  
      }
      i--;
      temp_j++;
      
    }while(temp_j<7);    
     
  }//row using outer for loop

  return status;

}



// To repeatedly run the desired code
void loop() {

  player = -1*player;
  
  getPoint(player);
    
  if(game_End()){
    
    while(1){
      for(int i = 0; i<64; i++){
        
        leds[i] = CHSV(i*3,240,100);
        
      }
      FastLED.show();
    }
  }
  
  
}
