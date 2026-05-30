//gcc mecanismo_antena.c -o mecanismo -lwiringPi
#include <stdio.h>
#include <wiringPi.h>

#define MOSFET_PIN  19   // GPIO19

int main() {
    if (wiringPiSetupGpio() == -1) {
        printf("Erro ao inicializar WiringPi\n");
        return 1;
    }

    pinMode(MOSFET_PIN, OUTPUT);

    printf("Esquentando o fio por 10s\n");
    digitalWrite(MOSFET_PIN, HIGH);

    delay(5000);   // 5000ms = 5s

    printf("Desligando MOSFET\n");
    digitalWrite(MOSFET_PIN, LOW);

    return 0;
}