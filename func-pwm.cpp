#include "func-pwm.h"

static int sysClk = 125000000; 
static int pwmPin[2] = {20, 21};
		
static int pwmFreq[2] = {5000, 5000};
static pwm_config pwm_slice_config[2];

int pico_pwm::setup(void){

	
	int pwmWrap[2] = {sysClk/pwmFreq[0], sysClk/pwmFreq[1]};

	uint pwm_slice_num[2];

	gpio_set_function(pwmPin[0], GPIO_FUNC_PWM);
	gpio_set_function(pwmPin[1], GPIO_FUNC_PWM);
	pwm_slice_num[0] = pwm_gpio_to_slice_num(pwmPin[0]);
	pwm_slice_num[1] = pwm_gpio_to_slice_num(pwmPin[1]);

	pwm_slice_config[0] = pwm_get_default_config();
	pwm_slice_config[1] = pwm_get_default_config();

	pwm_config_set_wrap(&pwm_slice_config[0], pwmWrap[0]);
	pwm_config_set_wrap(&pwm_slice_config[1], pwmWrap[1]);
	
	pwm_config_set_clkdiv(&pwm_slice_config[0], 10);
	pwm_config_set_clkdiv(&pwm_slice_config[1], 10);

	pwm_init( pwm_slice_num[0], &pwm_slice_config[0], true );
	pwm_init( pwm_slice_num[1], &pwm_slice_config[1], true );

	pwm_set_gpio_level(pwmPin[0], (pwm_slice_config[0].top*0.5));
	pwm_set_gpio_level(pwmPin[1], (pwm_slice_config[1].top*0.5));

	return 0;
};

int pico_pwm::duty(int channel, double duty){
	if(duty > 1 || duty < 0){
		return 1;
	}
	else{
		pwm_set_gpio_level(pwmPin[channel], (pwm_slice_config[channel].top*duty));
		return 0;
	}
};
