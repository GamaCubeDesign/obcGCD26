#ifndef GPIO_H
#define GPIO_H

void gpio_init(void);                  
void gpio_antenna_deploy_start(void);  
void gpio_antenna_deploy_update(void); 
int  gpio_antenna_deploy_done(void);   

/* Stubs para controle de energia das cargas uteis */
void gpio_esp32_power_set(int state);
void gpio_sdr_power_set(int state);

#endif