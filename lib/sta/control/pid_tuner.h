#ifndef _PID_TUNER_
#define _PID_TUNER_

#include "sta.h"

BEGIN_CONTROL_BLOCK

class pid_autotune {
public:
//commonly used functions **************************************************************************
// * Constructor.  links the pid_autotune to a given PID
	pid_autotune(sta::f64* Input, sta::f64* Output) {
		input = Input;
		output = Output;
		controlType =0 ; //default to PI
		noiseBand = 0.5;
		running = false;
		oStep = 30;
		SetLookbackSec(10);
		lastTime = millis();
	}           

	// * Similar to the PID Compue function, returns non 0 when done        	
	sta::int32 Runtime() {
		justevaled=false;
		if(peakCount>9 && running) {
			running = false;
			FinishUp();
			return 1;
		}

		unsigned long now = millis();
		
		if((now-lastTime)<sampleTime) return false;
		lastTime = now;
		sta::f64 refVal = *input;
		justevaled=true;
		if(!running) { //initialize working variables the first time around
			peakType = 0;
			peakCount=0;
			justchanged=false;
			absMax=refVal;
			absMin=refVal;
			setpoint = refVal;
			running = true;
			outputStart = *output;
			*output = outputStart+oStep;
		}
		else {
			if(refVal>absMax)absMax=refVal;
			if(refVal<absMin)absMin=refVal;
		}
		
		//oscillate the output base on the input's relation to the setpoint
		
		if(refVal>setpoint+noiseBand) *output = outputStart-oStep;
		else if (refVal<setpoint-noiseBand) *output = outputStart+oStep;
			
			
		//bool isMax=true, isMin=true;
		isMax=true;isMin=true;
		//id peaks
		for(sta::int32 i=nLookBack-1;i>=0;i--) {
			sta::f64 val = lastInputs[i];
			if(isMax) isMax = refVal>val;
			if(isMin) isMin = refVal<val;
			lastInputs[i+1] = lastInputs[i];
		}
		lastInputs[0] = refVal;  
		if(nLookBack<9) {  //we don't want to trust the maxes or mins until the inputs array has been filled
			return 0;
		}
		
		if(isMax) {
			if(peakType==0)peakType=1;
			if(peakType==-1)
			{
			peakType = 1;
			justchanged=true;
			peak2 = peak1;
			}
			peak1 = now;
			peaks[peakCount] = refVal;
		
		}
		else if(isMin) {
			if(peakType==0)peakType=-1;
			if(peakType==1)
			{
			peakType=-1;
			peakCount++;
			justchanged=true;
			}
			
			if(peakCount<10)peaks[peakCount] = refVal;
		}
		
		if(justchanged && peakCount>2) { //we've transitioned.  check if we can pid_autotune based on the last peaks
			sta::f64 avgSeparation = (abs(peaks[peakCount-1]-peaks[peakCount-2])+abs(peaks[peakCount-2]-peaks[peakCount-3]))/2;
			if( avgSeparation < 0.05*(absMax-absMin))
			{
				FinishUp();
			running = false;
			return 1;
			
			}
		}
		justchanged=false;
		return 0;
	}						   			   	
	
	// * Stops the pid_autotune	
	void Cancel() {
		running = false;
	}								   	

	// * how far back are we looking to identify peaks
	void SetLookbackSec(sta::int32 value) {
		if (value<1) value = 1;
		
		if(value<25)
		{
			nLookBack = value * 4;
			sampleTime = 250;
		}
		else
		{
			nLookBack = 100;
			sampleTime = value*10;
		}
	}

	sta::int32 GetLookbackSec() {
		return nLookBack * sampleTime / 1000;
	}
public:
	inline sta::f64 GetKp() const {
		return controlType==1 ? 0.6 * Ku : 0.4 * Ku;
	}

	inline sta::f64 GetKi() const {
		return controlType==1? 1.2*Ku / Pu : 0.48 * Ku / Pu;  // Ki = Kc/Ti
	}

	inline sta::f64 GetKd() const {
		return controlType==1? 0.075 * Ku * Pu : 0;  //Kd = Kc * Td
	}

	// * the pid_autotune will ignore signal chatter smaller than this value
	//   this should be acurately set	
	inline void SetNoiseBand(sta::f64 Band) {
		noiseBand = Band;
	}

	inline sta::f64 GetNoiseBand() const {
		return noiseBand;
	}

	// * Determies if the tuning parameters returned will be PI (D=0)
	//   or PID.  (0=PI, 1=PID)	
	inline void SetControlType(sta::int32 Type) { //0=PI, 1=PID 
		controlType = Type;
	}

	inline sta::int32 GetControlType() const {
		return controlType;
	}

	// * how far above and below the starting value will the output step?	
	inline void SetOutputStep(sta::f64 Step) {
		oStep = Step;
	}

	inline sta::f64 GetOutputStep() const {
		return oStep;
	}
private:
	void FinishUp() {
		*output = outputStart;
      	//we can generate tuning parameters!
      	Ku = 4*(2*oStep)/((absMax-absMin)*3.14159);
      	Pu = (sta::f64)(peak1-peak2) / 1000;
	}
private:
	bool isMax, isMin;
	sta::f64 *input, *output;
	sta::f64 setpoint;
	sta::f64 noiseBand;
	sta::int32 controlType;
	bool running;
	unsigned long peak1, peak2, lastTime;
	sta::int32 sampleTime;
	sta::int32 nLookBack;
	sta::int32 peakType;
	sta::f64 lastInputs[101];
	sta::f64 peaks[10];
	sta::int32 peakCount;
	bool justchanged;
	bool justevaled;
	sta::f64 absMax, absMin;
	sta::f64 oStep;
	sta::f64 outputStart;
	sta::f64 Ku, Pu;
};

#endif

END_CONTROL_BLOCK