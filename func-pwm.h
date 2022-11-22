#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#ifndef FUNC_PWM_H
#define FUNC_PWM_H

class pico_pwm{
	public:
		
		int setup(void);
		int duty(int channel, double duty);
};

#endif
