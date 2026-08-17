#include <stdio.h>
#include <time.h>

#include "obclog.h"

static FILE *arquivo = NULL;
static struct timespec inicio;

/* Milissegundos desde obclog_init(). Relogio monotonico porque a Pi Zero nao
   tem RTC: a hora de parede so fica correta depois do NTP, e pode dar salto
   no meio da missao. O monotonico nunca anda para tras, entao ordenar por
   t_ms e sempre valido (HLR-ADS-07) e medir latencia tambem (HLR-ADS-08). */
static long agora_ms(void){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)(t.tv_sec - inicio.tv_sec) * 1000L
         + (t.tv_nsec - inicio.tv_nsec) / 1000000L;
}

/* Abre o registro comum a toda linha. Nenhum campo vem de entrada externa,
   so nomes fixos de enum, entao nao ha string para escapar. */
static void abre(const char *tipo){
    fprintf(arquivo, "{\"t_ms\":%ld,\"tipo\":\"%s\"", agora_ms(), tipo);
}

static void fecha(void){
    fprintf(arquivo, "}\n");
    fflush(arquivo);
}

int obclog_init(const char *caminho){
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    arquivo = fopen(caminho, "a");
    return arquivo == NULL ? 1 : 0;
}

void obclog_close(void){
    if (arquivo != NULL){
        fclose(arquivo);
        arquivo = NULL;
    }
}

void obclog_boot(unsigned int periodo_ms){
    time_t agora = time(NULL);
    char utc[32];

    /* Unico registro com hora de parede, so para ancorar a missao em uma
       data. Se a Pi subiu sem NTP este valor esta errado - t_ms nao. */
    if (strftime(utc, sizeof utc, "%Y-%m-%dT%H:%M:%SZ", gmtime(&agora)) == 0){
        utc[0] = '\0';
    }

    abre("boot");
    fprintf(arquivo, ",\"utc\":\"%s\",\"periodo_ms\":%u", utc, periodo_ms);
    fecha();
}

void obclog_transition(State de, State para){
    abre("transicao");
    fprintf(arquivo, ",\"de\":\"%s\",\"para\":\"%s\"",
            fsm_state_name(de), fsm_state_name(para));
    fecha();
}

void obclog_tc(Event ev){
    abre("tc");
    fprintf(arquivo, ",\"evento\":\"%s\"", fsm_event_name(ev));
    fecha();
}

void obclog_telemetry(const Telemetry *tm){
    abre("telemetria");
    fprintf(arquivo,
            ",\"bateria_v\":%.3f,\"corrente_a\":%.3f,\"temp_c\":%.2f"
            ",\"roll_deg\":%.2f,\"pitch_deg\":%.2f,\"yaw_deg\":%.2f"
            ",\"adsb_msgs\":%lu",
            tm->bateria_v, tm->corrente_a, tm->temp_c,
            tm->roll_deg, tm->pitch_deg, tm->yaw_deg, tm->adsb_msgs);
    fecha();
}
