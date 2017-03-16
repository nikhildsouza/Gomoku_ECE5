#include "FastLED.h"
#define NUM_LEDS 64 // 8x8 grid
#define LED_PIN 10  // datapin
#define LED_TYPE    WS2812B

CRGB leds[NUM_LEDS];

const CRGB HalfRed = {100, 0, 0};    //half the intesity of red
const CRGB HalfBlue = {0, 0, 100};   //half the intesity of Blue
const CRGB HalfGreen = {30, 100, 30}; //half the intesity of Green
const CRGB HalfYellow = {100, 100, 0}; //half the intesity of Yellow
const CRGB Blank = {0, 0, 0};        //Black or no colour
CRGB Colour; // Keeps track of current colour - global variable

const int8_t SW1_pin = A4; // digital pin connected to SW
const int8_t X1_pin = A0;  // analog pin connected to VRx
const int8_t Y1_pin = A1;  // analog pin connected to VRy

const int8_t SW2_pin = A5; // digital pin connected to SW
const int8_t X2_pin = A2;  // analog pin connected to VRx
const int8_t Y2_pin = A3;  // analog pin connected to VRy

struct ledType {
  int8_t num;
  CRGB colour;
};

int8_t cursor_x = 0, cursor_y = 0, player = 1;
int8_t cursor_x_red = 0, cursor_y_red = 0,  cursor_x_blue = 0, cursor_y_blue = 0;
ledType board_colour[8][8]; // displays all colours on current board
ledType board_colour_highlight[8][8]; // blanks out winning LED combination
ledType player_O[8][8]; // displays P1
ledType player_T[8][8]; // displays P2
ledType rainbow_storage[8][8];// displays a temporary rainbow spectrum across board



void setup(); // To setup all relevant parts
void led_to_coord(int8_t ledno, int8_t*x, int8_t*y);// Converts LED no. to coordinates
int8_t coord_to_led(int8_t*x, int8_t*y); // Converts coordinates to LED no. and returns an int
CRGB draw_out(int8_t which_player, int8_t ledno); // Returns the colour of current player
int readPlayer(int8_t which_player); // Returns the switch choice of current player
void get_input(int8_t which_player, int*Switch, int8_t* _x, int8_t* _y ); // Initializes variables acccording to player choice
void finalCoord(int8_t which_player); // Obtains sum totol of cursor movevements for one turn
void boardColour(); // Displays the current position and colour of LED
void boardHighlightColour(); // Highlights the winning LED combination
void dispPO(); // Displays P1
void dispPT(); // Displays P2
void getPoint(int8_t which_player); // Obtains the position of final point and traces cursor movement
void resetColour(); // Resets board_colour_highlight[8][8] to equal board_colour[8][8]
boolean game_End(); // Determines conditions for succes anf returns a boolean value



// To setup all relevant parts
void setup() {

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  //initialize all the respective LED numbers of 8x8 Grid from 0 to 63
  //initialize all the colours for these 64 LEDs to be Blank/black
  for (int8_t i = 0; i < 8; i++) {

    for (int8_t j = 0; j < 8; j++) {
      board_colour[i][j].num = coord_to_led(&i, &j);
      board_colour_highlight[i][j].num = coord_to_led(&i, &j);
      player_O[i][j].num = coord_to_led(&i, &j);
      player_T[i][j].num = coord_to_led(&i, &j);
      board_colour[i][j].colour = Blank;
      board_colour_highlight[i][j].colour = Blank;
      player_O[i][j].colour = Blank;
      player_T[i][j].colour = Blank;
    }//column using inner for loop
  }//row using outer for loop

  int b = 64;

  for (uint8_t i = 0; i < 8; i++) {

    for (uint8_t j = 0; j < 8; j++) {
      rainbow_storage[i][j].colour = CRGB{i * 8 + 7, j * 8 + 2, (b--) / 2};
    }//column using inner for loop

  }//row using outer for loop

  Serial.begin(9600);//for higher speed - 115200
}



// To convert numbers to coordinates on a 8x8 Grid
void led_to_coord(int8_t ledno, int8_t*x, int8_t*y) {

  *y = ledno / 8;  //row
  *x = ledno % 8;  //column
  if ((*y % 2) == 1 ) //check for odd row numbers
    *x = 7 - *x; //8x8 grid is in a snake-like unidirectional form
}



// To convert numbers to coordinates on a 8x8 Grid
int8_t coord_to_led(int8_t*x, int8_t*y) {

  if ( ((*y) % 2) == 1 ) { //check for odd row numbers
    *x = 7 - (*x);//8x8 grid is in a snake-like unidirectional form
  }

  int8_t ledno = (*y) * 8 + *x;
  return ledno;
}



// To display the correct colour of LED for a choice of player
CRGB draw_out(int8_t which_player, int8_t ledno)
{

  if (which_player == -1) {
    Colour = HalfRed;
  } else {
    Colour = HalfBlue;
  }

  return Colour;
}



//Sets switch value depending on which player
int readPlayer(int8_t which_player) {

  int Exit_switch;
  if (which_player == -1) {
    Exit_switch = analogRead(SW1_pin);
  }
  else {
    Exit_switch = analogRead(SW2_pin);
  }

  return Exit_switch;

}



// To obtain the users' choice
int get_input(int8_t which_player, int8_t* _x, int8_t* _y ) {

  int8_t x, y;
  int x_raw, y_raw, Switch;

  if (which_player == -1) {
    x_raw = analogRead(X1_pin);
    y_raw = analogRead(Y1_pin);
    Switch = analogRead(SW1_pin);
  }
  else {
    x_raw = analogRead(X2_pin);
    y_raw = analogRead(Y2_pin);
    Switch = analogRead(SW2_pin);
  }
  //analogRead returns a range from 0 to 1023
  //500 is midpoint where nothing is done to joystick
  //0 is returned when the switch is pressed

  //arbitrary axis for x-value
  if (x_raw > 700)
    x = 1;
  else if (x_raw < 300)
    x = -1;
  else
    x = 0;

  //arbitrary axis for y-value
  if (y_raw > 700)
    y = -1;
  else if (y_raw < 300)
    y = 1;
  else
    y = 0;

  *_x = x; *_y = y;
  return Switch;

}



// Adds all the cursor movements till the joystick is clicked
void finalCoord(int8_t which_player) {

  // Initialize and reset all these vaiables for change in player
  int8_t x = 0, y = 0,  c = 0, d = 0;
  int Switch = 0;

  if (which_player == -1) {
    // Reset all these vaiables for player 1
    cursor_x = cursor_x_red, cursor_y = cursor_y_red;
  }
  else {
    // Reset all these vaiables for player 2
    cursor_x = cursor_x_blue, cursor_y = cursor_y_blue;
  }


  do {

    // Displays cursor on the 8x8 Grid
    // Displays all the set point of Red and Blue
    boardColour();

    Switch = get_input(which_player, &x, &y); // obtains input
    cursor_x += x, cursor_y += y; //sum total movements for each player's turn
    // Setting upper and lower boundaries to prevent out of bound acces of 8x8 Grid
    if (cursor_x > 7) cursor_x = 7;
    if (cursor_x < 0) cursor_x = 0;
    if (cursor_y > 7) cursor_y = 7;
    if (cursor_y < 0) cursor_y = 0;

    // To restart each player from their last chosen spot
    if (which_player == -1) {
      // Keeps track of last move of player 1
      cursor_x_red = cursor_x, cursor_y_red = cursor_y ;
    }
    else {
      // Keeps track of last move of player 2
      cursor_x_blue = cursor_x, cursor_y_blue = cursor_y;
    }
    delay(200);// to provide a good response time for the player

  } while (!((board_colour[cursor_x][cursor_y].colour != HalfRed) && (board_colour[cursor_x][cursor_y].colour != HalfBlue) && (Switch == 0)));

}



//Set the permanent colour of the led board
void boardColour() {

  for (int8_t i = 0; i < 8; i++) {

    for (int8_t j = 0; j < 8; j++) {
      leds[board_colour[i][j].num] = board_colour[i][j].colour;
    }//column using inner for loop

  }//row using outer for loop

  if ( board_colour[cursor_x][cursor_y].colour == Blank) {
    leds[board_colour[cursor_x][cursor_y].num] = HalfGreen;
  }
  else { // if Grid is occupied by a player colour
    leds[board_colour[cursor_x][cursor_y].num] =  HalfYellow;
  }
  FastLED.show();

}



//displays colour of the board_colour_highlight[][]
void boardHighlightColour() {

  for (int8_t i = 0; i < 8; i++) {

    for (int8_t j = 0; j < 8; j++) {
      leds[board_colour_highlight[i][j].num] = board_colour_highlight[i][j].colour;
    }//column using inner for loop

  }//row using outer for loop

  FastLED.show();

}



//Display P1 for player 1 winning
void  dispPO() {
  for (int8_t i = 1; i < 7; i++) {
    player_O[1][i].colour = HalfRed;
    player_O[5][i].colour = HalfRed;
    if (i > 3) player_O[3][i].colour = HalfRed;
  }
  player_O[2][6].colour = HalfRed;
  player_O[2][4].colour = HalfRed;

  for (int8_t i = 0; i < 8; i++) {

    for (int8_t j = 0; j < 8; j++) {
      leds[player_O[i][j].num] = player_O[i][j].colour;
    }//column using inner for loop

  }//row using outer for loop
  FastLED.show();
}



//Display P2 for player 2 winning
void  dispPT() {
  for (int8_t i = 2; i < 8; i++) {
    player_T[0][i].colour = HalfBlue;
    if (i < 5)player_T[4][i - 1].colour = HalfBlue;
    if (i > 2 && i < 6)player_T[7][i].colour = HalfBlue;
    if (i > 4) player_T[2][i].colour = HalfBlue;
  }

  for (int j = 4; j < 8; j++) {
    player_T[j][1].colour = HalfBlue;
    player_T[j][3].colour = HalfBlue;
    player_T[j][5].colour = HalfBlue;
  }
  player_T[1][7].colour = HalfBlue;
  player_T[1][5].colour = HalfBlue;

  for (int8_t i = 0; i < 8; i++) {

    for (int8_t j = 0; j < 8; j++) {
      leds[player_T[i][j].num] = player_T[i][j].colour;
    }//column using inner for loop

  }//row using outer for loop
  FastLED.show();
}



//execute the game once
void getPoint(int8_t which_player) {

  finalCoord(which_player); // obtains final coordinate of each player's turn
  board_colour[cursor_x][cursor_y].colour = draw_out(which_player, board_colour[cursor_x][cursor_y].num); //assigns colour based on player
  board_colour_highlight[cursor_x][cursor_y].colour = board_colour[cursor_x][cursor_y].colour;

}



//Resets the colours of board_colour_highlight[][]
void resetColour() {
  for (int8_t i = 0; i < 8; i++) {
    for (int8_t j = 0; j < 8; j++) {
      board_colour_highlight[i][j].colour = board_colour[i][j].colour;
    }
  }
}



// Conditions to win the game!!
boolean game_End() {

  boolean status = false;
  int8_t ctr_row = 0, ctr_diagonal = 0, ctr_column = 0;
  ledType temp;

  //CHECKING ROW WIN CONDITION
  for (int8_t i = 1; i < 9; i++) { //CHECKING ROW WIN CONDITION

    for (int8_t j = 0; j < 7; j++) {

      if (board_colour[i - 1][j].colour == board_colour[i - 1][j + 1].colour && board_colour[i - 1][j + 1].colour != Blank) {
        ctr_row++;
        board_colour_highlight[i - 1][j].colour = Blank;
        board_colour_highlight[i - 1][j + 1].colour = Blank;
      }
      else {
        ctr_row = 0;
        resetColour();
      }

      if (ctr_row == 4) {
        status = true;
        return status;
      }

    }//column using inner for loop

  }//row using outer for loop

  //CHECKING Column WIN CONDITION
  for (int8_t i = 1; i < 9; i++) { //invert j and i counters
    for (int8_t j = 0; j < 7; j++) {

      if (board_colour[j][i - 1].colour == board_colour[j + 1][i - 1].colour && board_colour[j + 1][i - 1].colour != Blank) {
        ctr_column++;
        board_colour_highlight[j][i - 1].colour = Blank;
        board_colour_highlight[j + 1][i - 1].colour = Blank;
      }
      else {
        ctr_column = 0;
        resetColour();
      }

      if (ctr_column == 4) {
        status = true;
        return status;
      }

    }//column using inner for loop
  }

  //CHECKING FORWARD DIAGONAL WIN CONDITION FOR +VE
  for (int8_t i = 0; i < 4; i++) {

    int8_t j = 0, temp_i = i;
    do {

      if (board_colour[temp_i][j].colour == board_colour[temp_i + 1][j + 1].colour && board_colour[temp_i + 1][j + 1].colour != Blank) {
        ctr_diagonal++;
        board_colour_highlight[temp_i][j].colour = Blank;
        board_colour_highlight[temp_i + 1][j + 1].colour = Blank;
      }
      else {
        ctr_diagonal = 0;
        resetColour();
      }

      if (ctr_diagonal == 4) {
        status = true;
        return status;
      }
      j++;
      temp_i++;

    } while (temp_i < 7);

  }//row using outer for loop

  //CHECKING FORWARD DIAGONAL WIN CONDITION FOR -VE
  for (int8_t j = 0; j < 4; j++) {

    int8_t i = 0, temp_j = j;
    do {

      if (board_colour[i][temp_j].colour == board_colour[i + 1][temp_j + 1].colour && board_colour[i + 1][temp_j + 1].colour != Blank) {
        ctr_diagonal++;
        board_colour_highlight[i][temp_j].colour = Blank;
        board_colour_highlight[i + 1][temp_j + 1].colour = Blank;
      }
      else {
        ctr_diagonal = 0;
        resetColour();
      }

      if (ctr_diagonal == 4) {
        status = true;
        return status;
      }
      i++;
      temp_j++;

    } while (temp_j < 7);

  }//row using outer for loop

  //CHECKING NEGATIVE DIAGONAL WIN CONDITION FOR +VE
  for (int8_t i = 0; i < 4; i++) {

    int8_t j = 7, temp_i = i;
    do {

      if (board_colour[temp_i][j].colour == board_colour[temp_i + 1][j - 1].colour && board_colour[temp_i][j].colour != Blank) {
        ctr_diagonal++;
        board_colour_highlight[temp_i][j].colour = Blank;
        board_colour_highlight[temp_i + 1][j - 1].colour = Blank;
      }
      else {
        ctr_diagonal = 0;
        resetColour();
      }

      if (ctr_diagonal == 4) {
        status = true;
        return status;
      }
      j--;
      temp_i++;

    } while (temp_i < 7);

  }//row using outer for loop

  //CHECKING NEGATIVE DIAGONAL WIN CONDITION FOR -VE
  for (int8_t j = 7; j > 3; j++) {

    int8_t i = 7, temp_j = j;
    do {

      if (board_colour[i][temp_j].colour == board_colour[i + 1][temp_j - 1].colour && board_colour[i][temp_j].colour != Blank) {
        ctr_diagonal++;
        board_colour_highlight[i][temp_j].colour = Blank;
        board_colour_highlight[i + 1][temp_j - 1].colour = Blank;
      }
      else {
        ctr_diagonal = 0;
        resetColour();
      }

      if (ctr_diagonal == 4) {
        status = true;
        return status;
      }
      i--;
      temp_j++;

    } while (temp_j < 7);

  }//row using outer for loop

  return status;

}



//To check fro draw condition
//boolean gameDraw(){
//}
// To repeatedly run the desired code
void loop() {

  player = -1 * player;

  ledType rainbow[8][8];  // displays a rainbow spectrum across board

  for (int8_t i = 0; i < 8; i++) {
    for (int8_t j = 0; j < 8; j++) {
      rainbow[i][j].num = coord_to_led(&i, &j);
      rainbow[i][j].colour = Blank;
    }//column using inner for loop
  }//row using outer for loop

  getPoint(player);

  int Exit_switch;

  if (game_End()) {

    do {

      //Displays a moving wave of rainbow spectrum 
      for (int h = 0; h < 25; h++) {

        //shifts only the columns of rainbow_storage[][] by one to the right
        for (int8_t i = 0; i < 8 ; i++) {
          for (int8_t j = 0; j < 8; j++) {
            rainbow[i][j].colour = rainbow_storage[7 - (i + h) % 8][j].colour;
          }
        }

        //diplays new shifted rainbow
        for (int8_t i = 0; i < 8; i++) {
          for (int8_t j = 0; j < 8; j++) {
            leds[rainbow[i][j].num] = rainbow[i][j].colour;
          }//column using inner for loop
        }//row using outer for loop
        FastLED.show();
        delay(200);
      }

      for (int8_t i = 0; i < 5; i++) {
        boardColour();
        delay(200);
        boardHighlightColour();
        delay(200);
      }

      for (int8_t i = 0; i < 3; i++) {
        if (player == -1) {
          dispPO();
        }
        else {
          dispPT();
        }
        delay(200);

        for (int8_t i = 0; i < 64; i++) {
          leds[i] = Blank;
        }
        FastLED.show();
        delay(200);
      }

      Exit_switch = readPlayer(player);
    } while ((Exit_switch != 0)); //do-while loop
  }//if

  //if(gameDraw()){

  //}
}

