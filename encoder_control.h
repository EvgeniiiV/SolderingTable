#ifndef ENCODER_CONTROL_H
#define ENCODER_CONTROL_H

void initEncoderAndButton();
void handleEncoderRotation();
void handleButton(); 
void shortBeep();
void longBeepSequence();
void encoderButton_ISR_CHANGE(); 

#endif // ENCODER_CONTROL_H