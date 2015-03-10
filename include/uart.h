#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0])) //define ARRAY_LEN for the number of elements in that array

/*
 * This is the baud rate table, you can add custom ones at the bottom of the list.
 * This is declared static, which means it will be treated as seperate instances between source codes
 * however it is not suppose to change, it is only featured here so you can change it easily.
 */
struct baudTable
{
    speed_t     speed;
    unsigned    baudRate;
} const gBaudTable[] =
{
    { B50,          50 },
    { B75,          75 },
    { B110,        110 },
    { B134,        134 },
    { B150,        150 },
    { B200,        200 },
    { B300,        300 },
    { B600,        600 },
    { B1200,      1200 },
    { B1800,      1800 },
    { B2400,      2400 },
    { B4800,      4800 },
    { B9600,      9600 },
    { B19200,    19200 },
    { B38400,    38400 },
    { B57600,    57600 },
    { B115200,  115200 },
    { B230400,  230400 }
};

ssize_t serialOpen(const char*);
ssize_t serialSetBaud(int, unsigned int);
ssize_t serialReadData(int, size_t, char*);
long serialReadLong(int);
ssize_t serialClose(int);
#endif
