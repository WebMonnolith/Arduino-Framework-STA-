#ifndef _STA_CONTROL_pid_controller_
#define _STA_CONTROL_pid_controller_

#include "sta.h"

#define AUTOMATIC	1
#define MANUAL	0
#define DIRECT  0
#define REVERSE  1
#define P_ON_M 0
#define P_ON_E 1

BEGIN_CONTROL_BLOCK

class pid_controller {
public:
  //commonly used functions **************************************************************************

  // * constructor.  links the pid_controller to the Input, Output, and 
  //   Setpoint.  Initial tuning parameters are also set here.
  //   (overload for specifying proportional mode)

  /*Constructor (...)*********************************************************
  *    The parameters specified here are those for for which we can't set up
  *    reliable defaults, so we need to have the user set them.
  ***************************************************************************/
  pid_controller(sta::f64* Input, sta::f64* Output, sta::f64* Setpoint, sta::f64 Kp, sta::f64 Ki, sta::f64 Kd, sta::int32 POn, sta::int32 ControllerDirection) {
      this->myOutput = Output;
      this->myInput = Input;
      this->mySetpoint = Setpoint;
      this->inAuto = false;

      this->SetOutputLimits(0, 255);
      this->SampleTime = 100;

      this->SetControllerDirection(ControllerDirection);
      this->SetTunings(Kp, Ki, Kd, POn);

      this->lastTime = millis() - this->SampleTime;
  }

                                          
  // * constructor.  links the pid_controller to the Input, Output, and
  //   Setpoint.  Initial tuning parameters are also set here
  /*Constructor (...)*********************************************************
  *    To allow backwards compatability for v1.1, or for people that just want
  *    to use Proportional on Error without explicitly saying so
  ***************************************************************************/

  pid_controller(sta::f64* Input, sta::f64* Output, sta::f64* Setpoint, sta::f64 Kp, sta::f64 Ki, sta::f64 Kd, sta::int32 ControllerDirection)
      :pid_controller::pid_controller(Input, Output, Setpoint, Kp, Ki, Kd, P_ON_E, ControllerDirection)
  {}


  // * sets pid_controller to either Manual (0) or Auto (non-0)
  void SetMode(sta::int32 Mode) {
      bool newAuto = (Mode == AUTOMATIC);
      if(newAuto && !this->inAuto) {  /*we just went from manual to auto*/
        this->Initialize();
      }
      this->inAuto = newAuto;
  }             

  // * performs the pid_controller calculation.  it should be
  //   called every time loop() cycles. ON/OFF and
  //   calculation frequency can be set using SetMode
  //   SetSampleTime respectively
  bool Compute() {
   if(!this->inAuto) return false;
   unsigned long now = millis();
   unsigned long timeChange = (now - this->lastTime);
   if(timeChange>=SampleTime) {
      /*Compute all the working error variables*/
      sta::f64 input = *myInput;
      sta::f64 error = *mySetpoint - input;
      sta::f64 dInput = (input - lastInput);
      this->outputSum+= (ki * error);

      /*Add Proportional on Measurement, if P_ON_M is specified*/
      if(!this->pOnE) this->outputSum-= this->kp * dInput;

      if(this->outputSum > this->outMax) this->outputSum= this->outMax;
      else if(this->outputSum < this->outMin) this->outputSum= this->outMin;

      /*Add Proportional on Error, if P_ON_E is specified*/
	    sta::f64 output;
      if(this->pOnE) output = this->kp * error;
      else output = 0;

      /*Compute Rest of pid_controller Output*/
      output += outputSum - kd * dInput;

	    if(output > this->outMax) output = outMax;
      else if(output < this->outMin) output = outMin;
	    *this->myOutput = output;

      /*Remember some variables for next time*/
      this->lastInput = input;
      this->lastTime = now;
	return true;
   }
   else return false;
  }                

  // * clamps the output to a specific range. 0-255 by default, but
  //   it's likely the user will want to change this depending on
  //   the application
  void SetOutputLimits(sta::f64 Min, sta::f64 Max) {
    if(Min >= Max) return;
    this->outMin = Min;
    this->outMax = Max;

    if(this->inAuto) {
      if(*this->myOutput > this->outMax) *this->myOutput = this->outMax;
      else if(*this->myOutput < this->outMin) *this->myOutput = this->outMin;

      if(this->outputSum > this->outMax) this->outputSum= this->outMax;
      else if(this->outputSum < this->outMin) this->outputSum= this->outMin;
    }
  }
										                      
  //available but not commonly used functions ********************************************************

  // * While most users will set the tunings once in the 
  //   constructor, this function gives the user the option
  //   of changing tunings during runtime for Adaptive control
  // * overload for specifying proportional mode
  void SetTunings(sta::f64 Kp, sta::f64 Ki, sta::f64 Kd){
    this->SetTunings(Kp, Ki, Kd, pOn); 
  }        	    
                                        
  void SetTunings(sta::f64 Kp, sta::f64 Ki, sta::f64 Kd, sta::int32 POn) {
    if (Kp<0 || Ki<0 || Kd<0) return;

    this->pOn = POn;
    this->pOnE = POn == P_ON_E;

    this->dispKp = Kp; this->dispKi = Ki; this->dispKd = Kd;

    sta::f64 SampleTimeInSec = ((sta::f64)this->SampleTime)/1000;
    this->kp = Kp;
    this->ki = Ki * SampleTimeInSec;
    this->kd = Kd / SampleTimeInSec;

    if(controllerDirection ==REVERSE) {
        this->kp = (0 - this->kp);
        this->ki = (0 - this->ki);
        this->kd = (0 - this->kd);
    }
  }       	  

  // * Sets the Direction, or "Action" of the controller. DIRECT
	//   means the output will increase when error is positive. REVERSE
	//   means the opposite.  it's very unlikely that this will be needed
	//   once it is set in the constructor.
	void SetControllerDirection(sta::int32 Direction) {
    if(inAuto && Direction !=controllerDirection) {
        this->kp = (0 - this->kp);
        this->ki = (0 - this->ki);
        this->kd = (0 - this->kd);
    }
    controllerDirection = Direction;
  }

  // * sets the frequency, in Milliseconds, with which 
  //   the pid_controller calculation is performed.  default is 100
  void SetSampleTime(sta::int32 NewSampleTime) {
    if (NewSampleTime > 0) {
        sta::f64 ratio  = (sta::f64)NewSampleTime
                        / (sta::f64)SampleTime;
        ki *= ratio;
        kd /= ratio;
        SampleTime = (unsigned long)NewSampleTime;
    }
  }														  
										  
  //Display functions ****************************************************************
  // These functions query the pid_controller for interal values.
  //  they were created mainly for the pid_controller front-end,
  // where it's important to know what is actually inside the pid_controller.
  sta::f64 GetKp(){ return  dispKp; }
  sta::f64 GetKi(){ return  dispKi;}
  sta::f64 GetKd(){ return  dispKd;}
  sta::int32 GetMode(){ return  inAuto ? AUTOMATIC : MANUAL;}
  sta::int32 GetDirection(){ return controllerDirection;}				

private:
	void Initialize() {
        outputSum = *myOutput;
        lastInput = *myInput;
        if(outputSum > outMax) outputSum = outMax;
        else if(outputSum < outMin) outputSum = outMin;
    }
private:
    sta::f64 dispKp;				// * we'll hold on to the tuning parameters in user-entered 
    sta::f64 dispKi;				//   format for display purposes
    sta::f64 dispKd;				//
    
    sta::f64 kp;                  // * (P)roportional Tuning Parameter
    sta::f64 ki;                  // * (I)ntegral Tuning Parameter
    sta::f64 kd;                  // * (D)erivative Tuning Parameter

	sta::int32 controllerDirection;
	sta::int32 pOn;

    /* Pointers to the Input, Output, and Setpoint variables
      This creates a hard link between the variables and the 
      pid_controller, freeing the user from having to constantly tell us
      what these values are.  with pointers we'll just know.*/
    sta::f64 *myInput;              
    sta::f64 *myOutput;             
    sta::f64 *mySetpoint;           
			  
	unsigned long lastTime;
	sta::f64 outputSum, lastInput;

	unsigned long SampleTime;
	sta::f64 outMin, outMax;
	bool inAuto, pOnE;
};

#endif

END_CONTROL_BLOCK