#ifndef OBCLOG_H
#define OBCLOG_H

#include "fsm.h"

/* Pacote de telemetria - HLR-COMM-02.
   A OBC so empacota: os valores vem dos outros subsistemas.
   Campos e unidades ainda a confirmar com EPS e ADCS. */
typedef struct {
    double bateria_v;    /* tensao da bateria, V */
    double corrente_a;   /* corrente consumida, A */
    double temp_c;       /* temperatura, graus C */
    double roll_deg;
    double pitch_deg;
    double yaw_deg;
    unsigned long adsb_msgs;  /* mensagens ADS-B acumuladas na missao */
} Telemetry;

/* Abre o arquivo de log. Retorna 0 em sucesso, != 0 em erro. */
int  obclog_init(const char *caminho);
void obclog_close(void);

void obclog_boot(unsigned int periodo_ms);
void obclog_transition(State de, State para);
void obclog_tc(Event ev);
void obclog_telemetry(const Telemetry *tm);

#endif
