#ifndef SDR_H
#define SDR_H

/* SDR dedicado a missao ADS-B, conectado a OBC pelo modulo USB proprio
   da equipe: nao e um dongle SDR generico com decodificador em rede, o
   modulo ja entrega os frames 1090ES decodificados por uma porta serial
   sobre USB, uma linha por mensagem, no formato SBS-1 ("MSG,...").
   Este driver so conta as linhas recebidas; o contrato de log por
   mensagem ainda esta pendente (ver docs/log_schema.md, pendencias). */

#define SDR_USB_DEVICE_DEFAULT "/dev/ttyACM0"
#define SDR_BAUD_DEFAULT       115200

/* Abre a porta serial do modulo USB (8N1, sem controle de fluxo).
   dispositivo == NULL usa SDR_USB_DEVICE_DEFAULT.
   Retorna 0 em sucesso, != 0 em erro. */
int  sdr_init(const char *dispositivo);
void sdr_close(void);

/* Le o que estiver disponivel na porta neste ciclo, sem bloquear, e
   retorna quantas mensagens ADS-B novas chegaram (pode ser 0).
   Retorna -1 se a porta caiu ou nunca foi aberta - quem chama decide se
   tenta reabrir ou so segue registrando o que ja tinha. */
int sdr_poll_messages(void);

#endif