//  ------------------------------------------------------------
//  Update Buttons
//  ------------------------------------------------------------
//
//  These are the individual buttons
//                                    
//  button_plus     - upper left  
//  button_minus    - lower left  
//  button_set      - upper right 
//  button_clr      - lower right 

//  Separate counters are the memories for the button pushes and holds thru each loop
//
//  A push is judged to occur when the related counter value > push_action  AND  < hold_action
//  A hold is judged to occur when the related counter value > hold_action 

//  Here's how it works....

//  There are only certain allowable buuttons that will work

      //const byte  button_set        = TSButtonUpperRight;  //set time
      //const byte  button_clr        = TSButtonLowerRight;  //clear buzzer 
      //const byte  button_plus       = TSButtonUpperLeft;   //increment time value
      //const byte  button_minus      = TSButtonLowerLeft;   //decrement time value


//  There is also a counter for each button

      //byte  button_set_counter;         //single button counters
      //byte  button_clr_counter;         
      //byte  button_plus_counter;       
      //byte  button_minus_counter;


//  The counters count "up" for each program "loop"
//  Counter   0....1....2....3....4.........

//  Once again, each button is unique, so only the function that handles it is affected.
//  And each button has its own function 
//        button_set_action();                     //do push & hold actions for single button       
//        button_clr_action();
//        button_plus_action();
//        button_minus_action();

//  The trigger for a "push" or "hold" is defined by the variables below

      //const byte  push_action = 0;  //counter value to trigger push action
      //const byte  hold_action = 3;  //counter value to trigger hold action
      //                              //based on loop_time = 250 milliseconds

//  A "push" action occurs when (what's detected is the release of a particular button)
//  The appropriate counter > push_action AND < hold_action  AND  no buttons are being pushed (buttons_none == 1)
//  A flag is set or a function() is called at this point
//  And the counters are reset

//  A "hold" action occurs when
//  The appropriate counter > hold_action  
//  A flag is set or a function() is called at this point
//  And the counters are reset


//  These are the other variables associated with the Button Actions

      //byte  buttons_get;            //value for this sample of buttons
      //byte  buttons_last;           //last button_get value
      //const byte  buttons_none      = 0; //no buttons pushed value


//  Another circumstance involves the hold_action_flag
//  Here's what's said below

//        if (hold_action_flag == 1 && buttons_get == buttons_none)
//        {                                       //here all the buttons are released after a hold event...
//          hold_action_flag = 0;                 //when this happens then the counter may have 
//          clear_button_counters();              //incremented beyond what the user wanted in a "hold" event
//                                                //so clear the counter (all of them for that matter)
//                                                //in order NOT to falsely trigger a "push" event
//                                                //when the counter is non-zero
//        }
//
//


void update_buttons()
{
        buttons_get = display.getButtons();     //get button value for this time around
                                                

        if (hold_action_flag == 1 && buttons_get == buttons_none)
        {                                       //if we're here all the buttons are released AFTER a hold event...
          hold_action_flag = 0;                 //when this happens then the selected counter may have 
          clear_button_counters();              //incremented beyond what the user wanted in a "hold" event
                                                //so clear the counter (all of them for that matter)
                                                //in order NOT to falsely trigger a "push" event
                                                //if the counter is non-zero at that time
        }

                                                
        button_set_action();                    //do push & hold actions for single button       
        button_clr_action();
        button_plus_action();
        button_minus_action();
                             
}

//  ------------------------------------------------------------
//  button_set_action
//  ------------------------------------------------------------

void  button_set_action()
{
  if (buttons_get == button_set)
  {
    ++button_set_counter;
  }

//push
  if (buttons_get == buttons_none && button_set_counter > push_action && button_set_counter < hold_action)
  {
    button_set_flag = 1;      //set the flag  
    clear_button_counters();  //below
  }

//hold
  if (buttons_get == button_set && button_set_counter > hold_action)
  {
    hold_action_flag = 1;     //see why in button_actions()
    clear_button_counters();  //below
  }
}



//  ------------------------------------------------------------
//  button_clr_action
//  ------------------------------------------------------------

void  button_clr_action()
{
  if (buttons_get == button_clr)
  {
    ++button_clr_counter;
  }  

//push
  if (buttons_get == buttons_none && button_clr_counter > push_action && button_clr_counter < hold_action)
  {
    button_clr_flag = 1;      //set the flag  
    clear_button_counters();  //below
  }

//hold
  if (buttons_get == button_clr && button_clr_counter > hold_action)
  {
     hold_action_flag = 1;     //see why in button_actions()
     clear_button_counters();  //below
  }
}


//  ------------------------------------------------------------
//  button_plus_action
//  ------------------------------------------------------------

void button_plus_action()
{
  if (buttons_get == button_plus)
  {
    ++button_plus_counter;
  }   

//push
  if (buttons_get == buttons_none && button_plus_counter > push_action && button_plus_counter < hold_action)
  {
    button_plus_flag = 1;       //set the flag                               
    clear_button_counters();    //below
  }

//hold
  if (buttons_get == button_plus && button_plus_counter > hold_action)
  {
    button_plus_flag = 1;       //set the flag  
    hold_action_flag = 1;       //see why in button_actions()
    clear_button_counters();    //below
  }
}




//  ------------------------------------------------------------
//  button_minus_action
//  ------------------------------------------------------------

void  button_minus_action()
{
  if (buttons_get == button_minus)
  {
    ++button_minus_counter;
  }  

//push
  if (buttons_get == buttons_none && button_minus_counter > push_action && button_minus_counter < hold_action)
  {
    button_minus_flag = 1;      //set the flag                                
    clear_button_counters();    //below
  }

//hold
  if (buttons_get == button_minus && button_minus_counter > hold_action)
  {
    button_minus_flag = 1;      //set the flag    
    hold_action_flag = 1;       //see why in button_actions()
    clear_button_counters();    //below
  }
}



//  ------------------------------------------------------------
//  clear_button_counters
//  ------------------------------------------------------------

void  clear_button_counters()
{
    button_set_counter        = 0;   //clear the button counters
    button_clr_counter        = 0;   //so we can start from scratch next time around
    button_plus_counter       = 0;
    button_minus_counter      = 0; 
}
