#include "uart.h"
#include <string.h>
#include <stdlib.h>

ssize_t serialOpen(const char *sname)
{
	int fd;
	fd = open(sname, O_RDWR);

	if (fd < 0)
	{
		perror("Unable to open serial port");
		return -1;
	}

	return fd;
}

ssize_t serialClose(int fd)
{
	close(fd);
	return 0;
}

ssize_t serialSetBaud(int fd, unsigned int baudin)
{
	speed_t baud; //the baud rate
	unsigned int baudi; //increment for baud loop later on
	struct termios options; //create the options struct
	
	baud = B0;

	/*
	This checks to see if the input baud rate is in the baud rate table,
	if it is then it sets the baud rate to that else it prints an error and returns -1;
	*/
	for (baudi = 0; baudi < ARRAY_LEN(gBaudTable); baudi++)
	{
		if (gBaudTable[baudi].baudRate == baudin)
		{
			baud = gBaudTable[baudi].speed;
			break;
		}
	}

	if (baud == B0)
	{
		fprintf(stderr, "Unrecognized baud rate: '%d'\n", baudin);
		return -1;
	}

	tcgetattr(fd, &options); //get the current options on port
	cfsetispeed(&options, baud); //set input baud rate
	cfsetospeed(&options, baud); //set output baud rate
	options.c_cc[VMIN] = 1;
	options.c_cflag |= (baud | CS8 | CREAD | CLOCAL);	 //Device not to become master
	options.c_cflag &= ~CSTOPB;	 // 1 stop bit
	options.c_cflag &= ~PARENB;	 // No parity bit
	options.c_cflag &= ~CRTSCTS;	 // No hardware control
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);	 //Set echo off
	options.c_lflag &= ~(IXON | IXOFF | IXANY);	 // No software control
	tcsetattr(fd, TCSAFLUSH, &options);
	return 0;
}

ssize_t serialReadData(int fd, size_t len, char* buf)
{
	size_t total_read = 0;
	ssize_t n_read;

	while (total_read < len)
	{
		n_read = read(fd, buf + total_read, len - total_read);
		fprintf(stderr, "%lu %lu %lu\n", len, total_read, len - total_read);
		
		if (n_read < 0) 
		{
			perror("Serial Read");
			return -1;
		}
		else if (n_read == 0)
		{
			perror("read");
		}

		total_read += n_read;
	}

	return total_read;
}

long serialReadLong(int fd)
{
	char buf[1000] = {0};
	size_t bufidx = 0;

	while(1)
	{
		char temp;
		int nread = read(fd, &temp, 1);
		if (nread > 0)
		{
			if (temp == '\n')
				return atol(buf);	
			buf[bufidx] = temp;
			bufidx++;
		}
		else
		{
			perror("read");
		}
	}

	/* should never reach this point */
	return 0;
}
