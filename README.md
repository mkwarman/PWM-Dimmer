# PWM-Dimmer

### (My) Use Case
I run this code on Elegoo Nano along with some help from a Raspberry Pi (See my [enviro-manager](https://github.com/mkwarman/enviro-manager) repo). The Raspberry Pi takes in terrarium environment data and uses it to control relays to enable/disable a heat lamp, heat mat, and fogger. When the lamp or the mat is enabled, the Raspberry Pi sends a serial signal to this code in order to control the amount of power sent to the lamp and/or the mat, facillitating proportional heat control.

### Setup Info
This code is designed to run on an Arduino Nano or similar ATmega328 hardware. It is designed to control two seperate AC 120 volt 60Hz TRIAC dimmers with zero-cross detection via PWM. Since these two dimmers are on the same AC source, only one zero-cross signal input is used.
* The PWM signal to dimmer 1 is assigned to the PWM output pin 5 (PCINT21)
* The PWM signal to dimmer 2 is assigned to the PWM output pin 6 (PCINT22)
* The zero-cross detection signal is assigned to input pin 2 (PCINT18). You can use either dimmer's zero cross signal for this.

**If you decide to use different pins, make sure that your output pins are hardware PWM capable, that your zero-cross signal input pin is timer interrupt capable, and that you update the code to use the correct timer/interrupt associated with your new pins.** On the Arduino Nano/ATmega328, the pins I used are the only/best option.

### Controlling Via Serial
The code will listen for a serial string starting with a letter followed by three numbers. The letter must be either `L`, `M`, or `Z`. If you want, you can change this in the "For serial read target value detection" of the code. I choose "L" for "Light" and "M" for "Mat" since that matches my use case.:
* `L` refers to PWM signal to dimmer 1
* `M` refers to PWM signal to dimmer 2
* `Z` sets the delay to be used before acting on a zero cross signal. This is helpful because many zero-cross detection circuits fire a zero-cross event when the voltage is near zero, rather than when it actually crosses it. Having a small delay will allow you to manually tune how long to wait until actually opening the gate a true zero, though you will probably need an oscilloscope to accurately determine this.

The three numbers which follow the letter are to represent a value between 010 and 490. The larger the value, the greater the delay before a signal gate opens, and consequently the lower the duty cycle of the channel. 010 is very close to always on, and 490 is very close to always off. Numbers outside this range can result in some weird behavior, but you may have to adjust them for your hardware. These are the values that seem to work best for me.

### Misc Info
I wrote this code to be as simple as possible in order to make sure gate switching happened as quickly and accurately as possible. You could add logic to accept something like "0" to "100" over serial and calculate gate switching delay from that, but I opted to just have a Raspberry Pi do that calculation instead prior to sending a new duty cycle signal. That way the Arduino can just focus on doing its one job: precisely and reliably controlling phase clipping.
