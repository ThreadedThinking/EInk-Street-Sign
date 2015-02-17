#include <i2c.h>


enum tps_registers {
	TMST_VALUE, 
	ENABLE, 
	VADJ,
	VCOM1,
	VCOM2,
	INT_EN1, 
	INT_EN2,
	INT1,
	INT2,
	UPSEQ0,
	UPSEQ1,
	DWNSEQ0,
	DWNSEQ1,
	TMST1,
	TMST2,
	PG,
	REVID
};


void write_ti(uint8_t reg, uint8_t value) {

	uint8_t temp;

	while(i2c_busy()) /* wait */;
	*I2CCR = 0;
	// assume being master, thus no addres has to be set
	*I2CCR = I2C_MEN | I2C_MSTA | I2C_MTX; // start condition is triggered
	
	// write out address of slave
	*I2CDR = 0x68 << 1;

	while(!(*I2CSR & I2C_MCF) || !(*I2CSR & I2C_MIF)) /*wait*/;

	if (*I2CSR & I2C_RXAK ) {
		// NO acknoledge byte received
		printf("*** ERROR I2C: No ack received 1\n");
	}


	if (*I2CSR & I2C_MCF) {
		*I2CDR = (reg & 0xFF);

		// clear MIF
		*I2CSR &= ~I2C_MIF;

		while(!(*I2CSR & I2C_MCF) || !(*I2CSR & I2C_MIF)) /*wait*/;

		if (*I2CSR & I2C_RXAK) {
			// NO acknoledge byte received
			printf("*** ERROR I2C: No ack received 2\n");
		}

	}
	if (*I2CSR & I2C_MCF) {
		*I2CDR = (value & 0xFF);

		// clear MIF
		*I2CSR &= ~I2C_MIF;

		while(!(*I2CSR & I2C_MCF) || !(*I2CSR & I2C_MIF)) /*wait*/;

		if (*I2CSR & I2C_RXAK) {
			// NO acknoledge byte received
			printf("*** ERROR I2C: No ack received 2\n");
		}

	}
	*I2CCR &= ~I2C_MSTA; // stop condition
	//*I2CCR = 0;
}
void read_ti(uint8_t reg) {
	uint8_t temp;

	while(i2c_busy()) /* wait */;
	// assume being master, thus no addres has to be set
	*I2CCR |= I2C_MSTA | I2C_MTX | I2C_TXAK; // start condition is triggered
	
	// write out address of slave
	*I2CDR = 0x68 <<1;
	while(!(*I2CSR & I2C_MCF) || !(*I2CSR & I2C_MIF)) /*wait*/;
	*I2CSR &= ~I2C_MIF;

	if (*I2CSR & I2C_RXAK ) {
		// NO acknoledge byte received
		printf("*** ERROR I2C: No ack received 1\n");
	}

	*I2CDR = (reg & 0xFF);
	while(!(*I2CSR & I2C_MCF) || !(*I2CSR & I2C_MIF)) /*wait*/;
	*I2CSR &= ~I2C_MIF;

	if (*I2CSR & I2C_RXAK) {
		// NO acknoledge byte received
		printf("*** ERROR I2C: No ack received 2\n");
	}

	*I2CCR |= I2C_RSTA;

	// write out address of slave
	*I2CDR = ((0x68) <<1) | 0x01;
	while(!(*I2CSR & I2C_MCF) || !(*I2CSR & I2C_MIF)) /*wait*/;
	*I2CSR &= ~I2C_MIF;

	*I2CCR &= ~I2C_MTX;
	//*I2CCR &= ~I2C_TXAK;

	temp = *I2CDR;
	while(!(*I2CSR & I2C_MCF) || !(*I2CSR & I2C_MIF)) /*wait*/;
	*I2CSR &= ~I2C_MIF;

	*I2CCR &= ~I2C_MSTA; // stop condition
	
	temp = *I2CDR;
	//printf("0x%x\n",temp);
}