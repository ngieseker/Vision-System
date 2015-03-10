#include <iostream>
#include <stdint.h>

extern "C" {
#include "uart.h"
}

using namespace std;

int main(int, char**)
{
	int fd = serialOpen("/dev/ttyACM0");
	serialSetBaud(fd, 115200);
	cout << sizeof(uint32_t) << endl;
	while (1)
	{
		/*for (int i = 0; i < 4; i++)
		{
			int fuck = serialReadData(fd, 1, (char *)&damnit);
			cout << i+1 << ": " << (int)damnit << endl;
		}*/
		long temp = serialReadLong(fd);
		cout << temp << endl;
		cout << endl;
	}
	serialClose(fd);
	return 0;
}
