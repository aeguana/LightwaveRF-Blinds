//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <EEPROM.h>

/////////////////////////////////////////////////// Config //////////////////////////////////////////////////////

int             motion_time_open_ms                     = 4000;           // Time for blinds to open 
int             motion_time_close_ms                    = 3800;           // Time for blinds to close 
int             pulse_time_ms                           = 20;             // Pulse time for pressing buttons . 

const int       left_blind_output_pins[]                = {12, 10, 11};
const int       right_blind_output_pins[]               = {9,  7,  8};

const int       lightwave_input_pins[]                  = {A2, A3};  
int             lightwave_last_state;  

typedef enum blind_control{
  CTRL_UP, 
  CTRL_DOWN,  
  CTRL_STOP 
};

typedef enum machine_state {
  OPEN,
  CLOSED,
  UNDEFINED
};

typedef enum blind_reference {
  LEFT, 
  RIGHT
};

typedef enum lightwave_control {
  LW_OPEN, 
  LW_CLOSE, 
  LW_STOP
};


machine_state STATE = UNDEFINED;  

/////////////////////////////////////////////////// Setup //////////////////////////////////////////////////////

void setup() {

  for (int i = 0; i < 3; i++) {                            // OUTPUTS 
    pinMode(left_blind_output_pins[i],        OUTPUT);     // Set blind output pins as output.
    pinMode(right_blind_output_pins[i],       OUTPUT);     // Set blind output pins as output.
    digitalWrite(left_blind_output_pins[i],   HIGH);       // Start with all controls turned off.  
    digitalWrite(right_blind_output_pins[i],  HIGH);       // Start with all controls turned off.  
  }

  for (int i = 0; i < 2; i++) {                            // INPUTS  
    pinMode(lightwave_input_pins[i],          INPUT);      // Set lightwaverf input pins as input.
    digitalWrite(lightwave_input_pins[i],     HIGH);       // Enable pull-up resistor (Active = LOW).   
  }
     
  int saved_state = EEPROM.read(0);                        // Read state from eeprom.
  if (saved_state > 0 and saved_state < 3) {
    STATE = machine_state(saved_state); 
  } else {
    STATE = UNDEFINED; 
  }

}


/////////////////////////////////////////////////// Main Loop //////////////////////////////////////////////////////


void loop () {
  
  poll_lightwave();   

}


/////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////


void send_blind_command(int command, int blind){
  if(blind == LEFT){
    digitalWrite(left_blind_output_pins[command],  LOW);        // Send control pulse.    
    delay(pulse_time_ms);  
    digitalWrite(left_blind_output_pins[command],  HIGH);        
  }else if(blind == RIGHT){
    digitalWrite(right_blind_output_pins[command], LOW);        // Send control pulse.    
    delay(pulse_time_ms);  
    digitalWrite(right_blind_output_pins[command], HIGH);  
  }
}

void open_blinds(){
  send_blind_command(CTRL_UP, LEFT); 
  delay(500);
  send_blind_command(CTRL_UP, RIGHT);  
  delay(motion_time_open_ms); 
  send_blind_command(CTRL_STOP, LEFT); 
  delay(500);
  send_blind_command(CTRL_STOP, RIGHT);    
  STATE = OPEN; 
  EEPROM.write(0, STATE);  
}

void close_blinds(){
  send_blind_command(CTRL_DOWN, LEFT); 
  delay(500);
  send_blind_command(CTRL_DOWN, RIGHT);  
  delay(motion_time_close_ms); 
  send_blind_command(CTRL_STOP, LEFT); 
  delay(500);
  send_blind_command(CTRL_STOP, RIGHT);    
  STATE = CLOSED;  
  EEPROM.write(0, STATE);  
}


void poll_lightwave(){
  if(digitalRead(lightwave_input_pins[LW_OPEN]) == LOW and lightwave_last_state != LW_OPEN){  
    lightwave_last_state = LW_OPEN;         
    if(STATE != OPEN){
      open_blinds();   
    }
  }else if(digitalRead(lightwave_input_pins[LW_CLOSE]) == LOW and lightwave_last_state != LW_CLOSE){
    lightwave_last_state = LW_CLOSE;  
    if(STATE != CLOSED){
      close_blinds();  
    }
  }
}


