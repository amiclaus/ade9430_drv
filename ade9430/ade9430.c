/***************************************************************************//**
 *   @file   ade9430.c
 *   @brief  Implementation of ADE9430 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <errno.h>
#include "ade9430.h"
#include "no_os_delay.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9430_read(struct ade9430_dev *dev, uint8_t reg_addr, uint8_t *reg_data)
{
	int ret;
	uint8_t buff[2] = {0};

	buff[0] = ADE9430_SPI_READ | reg_addr;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, 2);
	if (ret)
		return ret;

	*reg_data = buff[1];

	return 0;
}

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9430_write(struct ade9430_dev *dev, uint8_t reg_addr, uint8_t reg_data)
{
	uint8_t buff[2] = {0};

	buff[0] = reg_addr;
	buff[1] = reg_data;

	return no_os_spi_write_and_read(dev->spi_desc, buff, 2);
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9430_update_bits(struct ade9430_dev *dev, uint8_t reg_addr,
			uint8_t mask, uint8_t reg_data)
{
	int ret;
	uint8_t data;

	ret = ade9430_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data;

	return ade9430_write(dev, reg_addr, data);
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9430_init(struct ade9430_dev **device,
		 struct ade9430_init_param init_param)
{
	struct ade9430_dev *dev;
	int ret;

	dev = (struct ade9430_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization*/
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto error_dev;

	*device = dev;

	return 0;

error_dev:
	free(dev);

	return ret;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9430_remove(struct ade9430_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	free(dev);

	return ret;
}