#define SerComm Serial1
#define TX_INT 50
#define A_INT 0
#define Z_INT 1
#define _GET_CONTROL_BIT(b) (control_bits & (1 << (b)))
#define _CLEAR_CONTROL_BIT(b) (control_bits &= ~(1<<(b)));
#define _SET_CONTROL_BIT(b) (control_bits |= (1<<(b)));
#define FIRE_COMMAND_ISSUED 0
#define ISR_SET_AND_CALCUALTED 1
#define RELEASE_ENABLE 2
#define RELEASE_TRIGGERED 3
/*
   Control Bits:
0: Fire command issued
1: ISR_A registration and calculations confirmed
2: Release enable
3: Release mechanism triggered

 */
uint8_t control_bits=0;
uint16_t ticks,releasetick;
long period=millis(),txtime=millis();
int mpr; //millis per revolution
int calcTick(){
	//Calculate and return tick at which release is to occur at 

}
void fire(){
	//stuff to actually fire goes here 

}
void ISR_Z(){
	mpr = millis() - period;
	period = millis();
	if(_GET_CONTROL_BIT(ISR_SET_AND_CALCULATED)){
		//Launch parameters are ready! Enable Release!
		ticks=0;
		_SET_CONTROL_BIT(RELEASE_ENABLE)
	}
}
void ISR_A(){
	ticks++;
	if((ticks >= releasetick) && _GET_CONTROL_BIT(RELEASE_ENABLE)){
		fire();
		_SET_CONTROL_BIT(RELEASE_TRIGGERED);
		detachInterrupt(A_INT);
		_CLEAR_CONTROL_BIT(RELEASE_ENABLE);
		_CLEAR_CONTROL_BIT(ISR_SET_AND_CALCULATED);
	}
}

void setup(){
	attachInterrupt(Z_INT,ISR_Z,RISING);
#ifdef DEBUG
	SerComm.begin(57600);
#endif
}
void loop(){
	int rpm  = 60000 / mpr; // 60000 = 1000 milliseconds * 60 seconds
	period=millis();
	if((millis()-txtime)>TX_INT){
		SerComm.print('S');
		SerComm.write(control_bits);
		SerComm.println(rpm);
		txtime=millis();
	}
	if(SerComm.available()&&!_GET_CONTROL_BIT(FIRE_COMMAND_ISSUED)){
		if(SerComm.read()=='F'){
			_SET_CONTROL_BIT(FIRE_COMMAND_ISSUED);
		}
	}
	//We have a fire command! Attach the A_Interrupt!
	if(_GET_CONTROL_BIT(FIRE_COMMAND_ISSUED)&&(!_GET_CONTROL_BIT(ISR_SET_AND_CALCULATED))){
		//calculate RPM here
		releasetick=calcTick();
		attachInterrupt(A_INT,ISR_A,RISING);
		_SET_CONTROL_BIT(ISR_SET_AND_CALCULATED);
	}
}



