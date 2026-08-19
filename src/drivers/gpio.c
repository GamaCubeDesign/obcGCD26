#include <wiringPi.h>
#include <stdio.h>
#include "gpio.h"

#define MOSFET_PIN       19     
#define ANTENNA_BURN_MS  5000   

/* Substituir -1 pelos pinos reais quando o esquematico for congelado */
#define ESP32_POWER_PIN  -1
#define SDR_POWER_PIN    -1

static int deploying = 0;
static int deployed  = 0;
static unsigned int inicio_ms = 0;

void gpio_init(void){
    wiringPiSetupGpio();
    
    pinMode(MOSFET_PIN, OUTPUT);
    digitalWrite(MOSFET_PIN, LOW);

    /* Inicializar pinos de energia aqui 
    if (ESP32_POWER_PIN != -1) {
        pinMode(ESP32_POWER_PIN, OUTPUT);
        digitalWrite(ESP32_POWER_PIN, HIGH); // Estado padrao (ligado)
    }
    if (SDR_POWER_PIN != -1) {
        pinMode(SDR_POWER_PIN, OUTPUT);
        digitalWrite(SDR_POWER_PIN, HIGH); // Estado padrao (ligado)
    }
    */
}

void gpio_antenna_deploy_start(void){
    if (deployed || deploying) return;  
    digitalWrite(MOSFET_PIN, HIGH);
    inicio_ms = millis();
    deploying = 1;
}

void gpio_antenna_deploy_update(void){
    if (!deploying) return;
    if (millis() - inicio_ms >= ANTENNA_BURN_MS){
        digitalWrite(MOSFET_PIN, LOW);
        deploying = 0;
        deployed  = 1;
    }
}

int gpio_antenna_deploy_done(void){
    return deployed;
}


void gpio_esp32_power_set(int state){
    printf("[STUB GPIO] Alimentacao do ESP32 alterada para: %s\n", state ? "ON" : "OFF");
    
    /* Descomentar quando o esquematico estiver pronto
    if (ESP32_POWER_PIN != -1) {
        digitalWrite(ESP32_POWER_PIN, state ? HIGH : LOW);
    }
    */
}

void gpio_sdr_power_set(int state){
    printf("[STUB GPIO] Alimentacao do SDR alterada para: %s\n", state ? "ON" : "OFF");
    
    /* 
    if (SDR_POWER_PIN != -1) {
        digitalWrite(SDR_POWER_PIN, state ? HIGH : LOW);
    }
    */
}