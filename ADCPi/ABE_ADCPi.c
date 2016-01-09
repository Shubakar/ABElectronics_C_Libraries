/*
 ================================================
 ABElectronics UK ADC Pi 8-Channel Analogue to Digital Converter
 Version 1.1 Created 23/01/2015 - Updated 27/05/2015
 ================================================

Reads from the MCP3424 ADC on the ADC Pi and ADC Pi Plus.

Two functions are available to use.

read_raw(address,channel,bitrate,pga,conversionmode) returns the raw number from the ADC

read_voltage(address,channel,bitrate,pga,conversionmode) returns the voltage present at the ADC input


 Required package{
 apt-get install libi2c-dev
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

static int i2cbus;
const char *fileName = "/dev/i2c-1"; // change to /dev/i2c-0 if you are using a revision 0002 or 0003 model B
unsigned char writebuffer[10] = { 0 };
unsigned char readbuffer[10] = { 0 };

// local methods

static void read_byte_array(char address, char reg, char length) {

	if ((i2cbus = open(fileName, O_RDWR)) < 0) {
		printf("Failed to open i2c port for read %s \n", strerror(errno));

		exit(1);
	}

	if (ioctl(i2cbus, I2C_SLAVE, address) < 0) {
		printf("Failed to write to i2c port for read\n");
		exit(1);
	}

	writebuffer[0] = reg;

	if ((write(i2cbus, writebuffer, 1)) != 1) {
		printf("Failed to write to i2c device for read\n");
		exit(1);
	}

	read(i2cbus, readbuffer, 4);

	//if (read(i2cbus, readbuffer, 4) != 1) { // Read back data into buf[]
	//	printf("Failed to read from slave\n");
	//	exit(1);
	//}

	close(i2cbus);
}


static char update_byte(char byte, char bit, char value) {
	/*
	 internal method for setting the value of a single bit within a byte
	 */
	if (value == 0) {
		return (byte &= ~(1 << bit));

	} else {
		return (byte |= 1 << bit);
	}

}

static char check_bit(char byte, char bit) {
	/*
	 internal method for reading the value of a single bit within a byte
	 */
	if (byte & (1 << bit)) {
		return (1);
	} else {
		return (0);
	}
}

static long check_long_bit(long val, char bit) {
	/*
	 internal method for reading the value of a single bit within a byte
	 */
	if (val & (1 << bit)) {
		return (1);
	} else {
		return (0);
	}
}


static char set_pga(char config, char gain) {
	/*
	internal method for Programmable Gain Amplifier gain selection
	*/
	switch (gain) {
	case 1:
		config = update_byte(config, 0, 0);
		config = update_byte(config, 1, 0);
		break;
	case 2:
		config = update_byte(config, 0, 1);
		config = update_byte(config, 1, 0);
		break;
	case 4:
		config = update_byte(config, 0, 0);
		config = update_byte(config, 1, 1);
		break;
	case 8:
		config = update_byte(config, 0, 1);
		config = update_byte(config, 1, 1);
		break;
	default:
		break;
	}
	return (config);
}


static char set_bit_rate(char config, char rate) {
	/*
internal method for bit rate selection
*/
	switch (rate) {
	case 12:
		config = update_byte(config, 2, 0);
		config = update_byte(config, 3, 0);
		break;
	case 14:
		config = update_byte(config, 2, 1);
		config = update_byte(config, 3, 0);
		break;
	case 16:
		config = update_byte(config, 2, 0);
		config = update_byte(config, 3, 1);

		break;
	case 18:
		config = update_byte(config, 2, 1);
		config = update_byte(config, 3, 1);

		break;
	default:
		break;
	}
	return (config);
}


static char set_conversion_mode(char config, char mode) {
	/*
	internal method for setting the conversion mode
	*/
	if (mode == 1) {
		config = update_byte(config, 4, 1);
	} else {
		config = update_byte(config, 4, 0);
	}

	return (config);
}

static char set_channel(char config, char channel) {
	/*
	internal method for setting the channel
	*/
	switch (channel) {
	case 1:
		config = update_byte(config, 5, 0);
		config = update_byte(config, 6, 0);
		break;
	case 2:
		config = update_byte(config, 5, 1);
		config = update_byte(config, 6, 0);
		break;
	case 3:
		config = update_byte(config, 5, 0);
		config = update_byte(config, 6, 1);
		break;
	case 4:
		config = update_byte(config, 5, 1);
		config = update_byte(config, 6, 1);
		break;
	}

	return (config);
}

/// <summary>
/// Reads the raw value from the selected ADC channel
/// </summary>
/// <param name="address">I2C Address e.g.  0x68</param>
/// <param name="channel">1 to 4</param>
/// <param name="bitrate">12, 14, 16 or 18</param>
/// <param name="pga">1, 2, 4 or 8</param>
/// <param name="conversionmode">0 = one shot conversion, 1 = continuous conversion</param>
/// <returns>raw long value from ADC buffer</returns>
long read_raw(char address, char channel, int bitrate, int pga, char conversionmode) {
	// variables for storing the raw bytes from the ADC
	char h = 0;
	char l = 0;
	char m = 0;
	char s = 0;
	char config = 0x9C;
	long t = 0;

	// set the config based on the provided parameters
	config = set_channel(config, channel);
	config = set_conversion_mode(config, conversionmode);
	config = set_bit_rate(config, bitrate);
	config = set_pga(config, pga);

	// keep reading the ADC data until the conversion result is ready
	int timeout = 1000; // number of reads before a timeout occurs
	int x = 0;

	do {
		if (bitrate == 18) {
			read_byte_array(address, config, 3);
			h = readbuffer[0];
			m = readbuffer[1];
			l = readbuffer[2];
			s = readbuffer[3];
		} else {
			read_byte_array(address, config, 2);
			h = readbuffer[0];
			m = readbuffer[1];
			s = readbuffer[2];
		}

		// check bit 7 of s to see if the conversion result is ready
		if (!check_bit(s, 7)) {
			break;
		}

		if (x > timeout) {
			// timeout occurred
			return (0);
		}

		x++;
	} while (1);

	// extract the returned bytes and combine in the correct order
	switch (bitrate) {
	case 18:

		t = ((h & 3) << 16) | (m << 8) | l;
		if (check_long_bit(t, 17) == 1) {
			t = t * -1;
		}
		break;
	case 16:
		t = (h << 8) | m;
		if (check_long_bit(t, 15) == 1) {
			t = t * -1;
		}
		break;
	case 14:

		t = ((h & 63) << 8) | m;
		if (check_long_bit(t, 13) == 1) {
			t = t * -1;
		}
		break;
	case 12:
		t = ((h & 15) << 8) | m;
		if (check_long_bit(t, 11) == 1) {
			t = t * -1;
		}
		break;
	default:
		break;
	}

	return (t);
}

/// <summary>
/// Returns the voltage from the selected ADC channel
/// </summary>
/// <param name="address">I2C Address e.g.  0x68</param>
/// <param name="channel">1 to 4</param>
/// <param name="bitrate">12, 14, 16 or 18</param>
/// <param name="pga">1, 2, 4 or 8</param>
/// <param name="conversionmode">0 = one shot conversion, 1 = continuous conversion</param>
/// <returns>double voltage value from ADC</returns>
double read_voltage(char address, char channel, int bitrate, int pga, char conversionmode) {
	long raw = read_raw(address, channel, bitrate, pga, conversionmode); // get the raw value

	// calculate the gain based on the pga value
	double gain = (double)pga / 2;

	// set the lsb value based on the bitrate
	double lsb = 0;

	switch (bitrate) {
	case 12:
		lsb = 0.0005;
		break;
	case 14:
		lsb = 0.000125;
		break;
	case 16:
		lsb = 0.00003125;
		break;
	case 18:
		lsb = 0.0000078125;
		break;
	default:
		return (9999);
		break;
	}

	if (raw < 0) // if the voltage is negative it can be ignored.
			{
		return (0.0); // returned a negative voltage so return 0
	} else {
		double voltage = ((double)raw * (lsb / gain)) * 2.471; // calculate the voltage and return it
		return (voltage);
	}
}


