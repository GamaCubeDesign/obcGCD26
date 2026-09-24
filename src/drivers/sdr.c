#include "sdr.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <termios.h>

#define SDR_LINE_BUF_LEN 256

static int    porta_fd = -1;
static char   linha_buf[SDR_LINE_BUF_LEN];
static size_t linha_len = 0;

/* So cobre as taxas realmente usadas neste projeto. */
static speed_t baud_para_termios(int baud) {
    switch (baud) {
        case 9600:   return B9600;
        case 19200:  return B19200;
        case 38400:  return B38400;
        case 57600:  return B57600;
        case 115200: return B115200;
        default:     return B115200;
    }
}

int sdr_init(const char *dispositivo) {
    struct termios tty;

    if (dispositivo == NULL) {
        dispositivo = SDR_USB_DEVICE_DEFAULT;
    }

    porta_fd = open(dispositivo, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (porta_fd < 0) {
        perror("[SDR] Falha ao abrir a porta USB");
        return 1;
    }

    if (tcgetattr(porta_fd, &tty) != 0) {
        perror("[SDR] Falha ao ler atributos da porta USB");
        close(porta_fd);
        porta_fd = -1;
        return 1;
    }

    cfsetispeed(&tty, baud_para_termios(SDR_BAUD_DEFAULT));
    cfsetospeed(&tty, baud_para_termios(SDR_BAUD_DEFAULT));

    /* 8N1, modo raw - o parsing de linha e feito por sdr_poll_messages,
       nao pelo driver da porta serial. */
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(porta_fd, TCSANOW, &tty) != 0) {
        perror("[SDR] Falha ao configurar a porta USB");
        close(porta_fd);
        porta_fd = -1;
        return 1;
    }

    linha_len = 0;

    return 0;
}

void sdr_close(void) {
    if (porta_fd >= 0) {
        close(porta_fd);
        porta_fd = -1;
    }
}

int sdr_poll_messages(void) {
    struct pollfd entrada;
    int mensagens_novas = 0;

    if (porta_fd < 0) {
        return -1;
    }

    entrada.fd = porta_fd;
    entrada.events = POLLIN;

    /* Drena tudo que estiver disponivel neste ciclo, sem bloquear. */
    while (poll(&entrada, 1, 0) > 0 && (entrada.revents & POLLIN)) {
        char c;
        ssize_t n = read(porta_fd, &c, 1);
        if (n <= 0) break;

        if (c == '\n') {
            linha_buf[linha_len] = '\0';
            /* Formato SBS-1: cada mensagem decodificada comeca com "MSG,".
               Outras linhas (status, keepalive do modulo) sao ignoradas.
               Campos decodificados (icao, callsign, lat/lon...) ainda nao
               tem contrato de log definido - ver pendencias do HLR-ADS-07. */
            if (strncmp(linha_buf, "MSG,", 4) == 0) {
                mensagens_novas++;
            }
            linha_len = 0;
        } else if (c != '\r' && linha_len + 1 < SDR_LINE_BUF_LEN) {
            linha_buf[linha_len++] = c;
        } else if (linha_len + 1 >= SDR_LINE_BUF_LEN) {
            linha_len = 0;
        }
    }

    return mensagens_novas;
}