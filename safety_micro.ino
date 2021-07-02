 /*
 * A simple safety system.  6/16/2021, Noah Vawter for Andy Cavatorta Studios
 * It reads the input pin regularly, e.g. 100 Hz.
 * It expects the state of the input pin to change at least even N time units, e.g. at least 1x per second  
 * As long as the input pin has changed state within the last interval, the output remains high.
 * Else: the output goes low, turning off external components.
 */

// Configuration
#define INPUT_PIN  ( D2 )
#define OUTPUT_PIN ( D3 )
float required_transition_window = 2.0f;  // seconds.  Input must change state within this window or else!

HardwareTimer *MyTim = new HardwareTimer( TIM2 );

int32_t input_pin_val;
float   time_this_window   = 0.0f;
float   interrupt_rate     = 100.0f;  // Hz
float   interrupt_period   = 1.0f / interrupt_rate;
int32_t input_state_prev   = 0;
int32_t output_state       = 0;

// Called periodically at e.g. 100 Hz
// Note: func argument not used
void interrupt_routine( HardwareTimer *p_ht )
{
  // Keep track of this time in this window
  time_this_window += interrupt_period;

  // Examine input from upstream system
  input_pin_val = digitalRead( INPUT_PIN );

  // Check if input transition has occurred
  if( input_pin_val != input_state_prev )
  {
    // Transition has occurred.
    input_state_prev = input_pin_val; // remember new input state  
    time_this_window = 0;             // reset window
    output_state     = 1;             // indicate system is safe
    digitalWrite( OUTPUT_PIN, output_state );    
  } 
  else
  {
    // No transition occurred.  Has it been too long?
    if( time_this_window > required_transition_window )
    {
      output_state = 0;  // indicate system is not safe
      digitalWrite( OUTPUT_PIN, output_state );  
    }
  }

  
}


void setup() 
{
  Serial.begin( 115200 );
  Serial.println( "Hello world." );
  
  // Input Pin
  pinMode( INPUT_PIN, INPUT_PULLDOWN  );

  // Output Pin
  pinMode( OUTPUT_PIN, OUTPUT );

  // Timer Interrupt
  MyTim->setOverflow( interrupt_rate, HERTZ_FORMAT);
  MyTim->attachInterrupt( interrupt_routine );
  MyTim->setMode(2, TIMER_OUTPUT_COMPARE);  // not needed in this app
  MyTim->resume();
}


void loop() 
{
  delay( 200 );  // nothing to do in main while loop
  Serial.print( input_pin_val );
  Serial.print(", " );
  Serial.print( input_state_prev );
  Serial.print(", " );
  Serial.print( time_this_window );
  Serial.print(", " );
  Serial.print( output_state);
  Serial.print(", " );
  Serial.println();
}
