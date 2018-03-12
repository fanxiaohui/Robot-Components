#include "button.h"
#include "debug.h"

button_struct_t s_testButton;
gpio_struct_t s_ledPin;

void testButtonPressed()
{
	gpio_out_set(s_ledPin);
	debug_writeString("Z");
}

void testButton_init()
{
	
	s_ledPin.port = PD;
	s_ledPin.number = 5;
	s_ledPin.direction = OUTPUT;
	s_ledPin.pullUp = NO_PULL;

	gpio_init(s_ledPin);


	s_testButton.pin.port = PD;
	s_testButton.pin.number = 7;
	s_testButton.pin.pullUp = NO_PULL;
	s_testButton.active = BUTTON_ACTIVE_HIGH;
	s_testButton.pressedFunction = testButtonPressed;
	
	button_init(s_testButton);
	button_start(s_testButton);
}

