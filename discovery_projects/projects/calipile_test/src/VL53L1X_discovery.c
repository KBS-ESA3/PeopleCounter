/* Includes ------------------------------------------------------------------*/
#include "VL53L1X_discovery.h"

/**
  * @brief  does something.
  * @param  a: puts something in.
  * @retval something or None.
  */
void VL53L1X_setAddress(VL53L1X *sensor,uint8_t new_addr)
{
  VL53L1X_writeReg(sensor,I2C_SLAVE__DEVICE_ADDRESS, new_addr & 0x7F);
  sensor->I2CAdress = new_addr;
}

uint8_t VL53L1X_init(VL53L1X *sensor, uint8_t io_2v8)
{
  // check model ID and module type registers (values specified in datasheet)
  if (VL53L1X_readReg16Bit(sensor, IDENTIFICATION__MODEL_ID) != 0xEACC) { return 0; }

  // VL53L1_software_reset() begin

  VL53L1X_writeReg(sensor, SOFT_RESET, 0x00);
  //delayMicroseconds(100);
  VL53L1X_writeReg(sensor, SOFT_RESET, 0x01);

  // give it some time to boot; otherwise the sensor NACKs during the readReg()
  // call below and the Arduino 101 doesn't seem to handle that well
  //delay(1);

  // VL53L1_poll_for_boot_completion() begin

  //startTimeout();

  // check last_status in case we still get a NACK to try to deal with it correctly
 // while ((VL53L1X_readReg(sensor, FIRMWARE__SYSTEM_STATUS) & 0x01) == 0 || sensor->last_status != 0)
  //{
  //  if (checkTimeoutExpired())
  //  {
  //    sensor->did_timeout = 1;
  //    return 0;
  //  }
  //}
  // VL53L1_poll_for_boot_completion() end

  // VL53L1_software_reset() end

  // VL53L1_DataInit() begin

  // sensor uses 1V8 mode for I/O by default; switch to 2V8 mode if necessary
  if (io_2v8)
  {
    VL53L1X_writeReg(sensor, PAD_I2C_HV__EXTSUP_CONFIG,
      VL53L1X_readReg(sensor, PAD_I2C_HV__EXTSUP_CONFIG) | 0x01);
  }

  // store oscillator info for later use
  sensor->fast_osc_frequency = VL53L1X_readReg16Bit(sensor, OSC_MEASURED__FAST_OSC__FREQUENCY);
  sensor->osc_calibrate_val = VL53L1X_readReg16Bit(sensor, RESULT__OSC_CALIBRATE_VAL);

  // VL53L1_DataInit() end

  // VL53L1_StaticInit() begin

  // Note that the API does not actually apply the configuration settings below
  // when VL53L1_StaticInit() is called: it keeps a copy of the sensor's
  // register contents in memory and doesn't actually write them until a
  // measurement is started. Writing the configuration here means we don't have
  // to keep it all in memory and avoids a lot of redundant writes later.

  // the API sets the preset mode to LOWPOWER_AUTONOMOUS here:
  // VL53L1_set_preset_mode() begin

  // VL53L1_preset_mode_standard_ranging() begin

  // values labeled "tuning parm default" are from vl53l1_tuning_parm_defaults.h
  // (API uses these in VL53L1_init_tuning_parm_storage_struct())

  // static config
  // API resets PAD_I2C_HV__EXTSUP_CONFIG here, but maybe we don't want to do
  // that? (seems like it would disable 2V8 mode)
  VL53L1X_writeReg16Bit(sensor, DSS_CONFIG__TARGET_TOTAL_RATE_MCPS, VL53L1X_TARGETRATE); // should already be this value after reset
  VL53L1X_writeReg(sensor, GPIO__TIO_HV_STATUS, 0x02);
  VL53L1X_writeReg(sensor, SIGMA_ESTIMATOR__EFFECTIVE_PULSE_WIDTH_NS, 8); // tuning parm default
  VL53L1X_writeReg(sensor, SIGMA_ESTIMATOR__EFFECTIVE_AMBIENT_WIDTH_NS, 16); // tuning parm default
  VL53L1X_writeReg(sensor, ALGO__CROSSTALK_COMPENSATION_VALID_HEIGHT_MM, 0x01);
  VL53L1X_writeReg(sensor, ALGO__RANGE_IGNORE_VALID_HEIGHT_MM, 0xFF);
  VL53L1X_writeReg(sensor, ALGO__RANGE_MIN_CLIP, 0); // tuning parm default
  VL53L1X_writeReg(sensor, ALGO__CONSISTENCY_CHECK__TOLERANCE, 2); // tuning parm default

  // general config
  VL53L1X_writeReg16Bit(sensor, SYSTEM__THRESH_RATE_HIGH, 0x0000);
  VL53L1X_writeReg16Bit(sensor, SYSTEM__THRESH_RATE_LOW, 0x0000);
  VL53L1X_writeReg(sensor, DSS_CONFIG__APERTURE_ATTENUATION, 0x38);

  // timing config
  // most of these settings will be determined later by distance and timing
  // budget configuration
  VL53L1X_writeReg16Bit(sensor, RANGE_CONFIG__SIGMA_THRESH, 360); // tuning parm default
  VL53L1X_writeReg16Bit(sensor, RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS, 192); // tuning parm default

  // dynamic config

  VL53L1X_writeReg(sensor, SYSTEM__GROUPED_PARAMETER_HOLD_0, 0x01);
  VL53L1X_writeReg(sensor, SYSTEM__GROUPED_PARAMETER_HOLD_1, 0x01);
  VL53L1X_writeReg(sensor, SD_CONFIG__QUANTIFIER, 2); // tuning parm default

  // VL53L1_preset_mode_standard_ranging() end

  // from VL53L1_preset_mode_timed_ranging_*
  // GPH is 0 after reset, but writing GPH0 and GPH1 above seem to set GPH to 1,
  // and things don't seem to work if we don't set GPH back to 0 (which the API
  // does here).
  VL53L1X_writeReg(sensor, SYSTEM__GROUPED_PARAMETER_HOLD, 0x00);
  VL53L1X_writeReg(sensor, SYSTEM__SEED_CONFIG, 1); // tuning parm default

  // from VL53L1_config_low_power_auto_mode
  VL53L1X_writeReg(sensor, SYSTEM__SEQUENCE_CONFIG, 0x8B); // VHV, PHASECAL, DSS1, RANGE
  VL53L1X_writeReg16Bit(sensor, DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT, 200 << 8);
  VL53L1X_writeReg(sensor, DSS_CONFIG__ROI_MODE_CONTROL, 2); // REQUESTED_EFFFECTIVE_SPADS

  // VL53L1_set_preset_mode() end

  // default to long range, 50 ms timing budget
  // note that this is different than what the API defaults to
  //setDistanceMode(Long);
  //setMeasurementTimingBudget(50000);

  // VL53L1_StaticInit() end

  // the API triggers this change in VL53L1_init_and_start_range() once a
  // measurement is started; assumes MM1 and MM2 are disabled
  VL53L1X_writeReg16Bit(sensor, ALGO__PART_TO_PART_RANGE_OFFSET_MM,
  VL53L1X_readReg16Bit(sensor, MM_CONFIG__OUTER_OFFSET_MM) * 4);

  return 1;
}


// Write an 8-bit register
void VL53L1X_writeReg(VL53L1X *sensor,uint16_t reg, uint8_t value)
{
	// Wait while I2C peripheral is not ready
  WaitForI2CFlag( I2C_ISR_BUSY );
  // Start I2C write transfer for 3 bytes
  I2C_TransferHandling( I2C1, sensor->I2CAdress, 3, I2C_AutoEnd_Mode, I2C_Generate_Start_Write );
  WaitForI2CFlag( I2C_ISR_TXIS );
  // Write eeprom address
  I2C_SendData(I2C1, (reg >> 8) & 0xFF);       // Address High byte
	WaitForI2CFlag( I2C_ISR_TXIS );
  // Write eeprom address
  I2C_SendData(I2C1, reg       & 0xFF);       // Address High byte
	WaitForI2CFlag( I2C_ISR_TXIS );
  // Write eeprom address
  I2C_SendData(I2C1, value);       // Address High byte
  WaitForI2CFlag(I2C_ISR_TC);
  
  // Wait for- and clear stop condition
  WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
}

// Write a 16-bit register
void VL53L1X_writeReg16Bit(VL53L1X *sensor,uint16_t reg, uint16_t value)
{
	// Wait while I2C peripheral is not ready
  WaitForI2CFlag( I2C_ISR_BUSY );
  I2C_TransferHandling( I2C1, sensor->I2CAdress, 4, I2C_AutoEnd_Mode, I2C_Generate_Start_Write );
  WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, (reg >> 8) & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, reg       & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, (value >> 8) & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, value       & 0xFF);
  WaitForI2CFlag(I2C_ISR_TC);
  WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
	
}

// Write a 32-bit register
void VL53L1X_writeReg32Bit(VL53L1X *sensor,uint16_t reg, uint32_t value)
{
	// Wait while I2C peripheral is not ready
  WaitForI2CFlag( I2C_ISR_BUSY );
  I2C_TransferHandling( I2C1, sensor->I2CAdress, 6, I2C_AutoEnd_Mode, I2C_Generate_Start_Write );
  WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, (reg >> 8) & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, reg       & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
	I2C_SendData(I2C1, (value >> 24) & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
	I2C_SendData(I2C1, (value >> 16) & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, (value >> 8) & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, value       & 0xFF);
  WaitForI2CFlag(I2C_ISR_TC);
  WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
}

// Read an 8-bit register
uint8_t VL53L1X_readReg(VL53L1X *sensor,uint16_t reg)
{
	uint8_t value;
	
  WaitForI2CFlag(I2C_ISR_BUSY);
  I2C_TransferHandling(I2C1, sensor->I2CAdress, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, (reg >> 8) & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, reg       & 0xFF);
  WaitForI2CFlag(I2C_ISR_TC);
  I2C_TransferHandling(I2C1, sensor->I2CAdress, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  WaitForI2CFlag(I2C_ISR_RXNE);
  value = I2C_ReceiveData(I2C1);
  WaitForI2CFlag(I2C_ISR_STOPF);
  I2C1->ICR = I2C_ICR_STOPCF;
	
  return value;
}

// Read a 16-bit register
uint16_t VL53L1X_readReg16Bit(VL53L1X *sensor,uint16_t reg)
{
  uint16_t value;

	WaitForI2CFlag(I2C_ISR_BUSY);
  I2C_TransferHandling(I2C1, sensor->I2CAdress, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, (reg >> 8) & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, reg       & 0xFF);
  WaitForI2CFlag(I2C_ISR_TC);
  I2C_TransferHandling(I2C1, sensor->I2CAdress, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  WaitForI2CFlag(I2C_ISR_RXNE);
  value  = (uint16_t)I2C_ReceiveData(I2C1) << 8;
	WaitForI2CFlag(I2C_ISR_RXNE);
  value  |= I2C_ReceiveData(I2C1);
  WaitForI2CFlag(I2C_ISR_STOPF);
  I2C1->ICR = I2C_ICR_STOPCF;

  return value;
}

// Read a 32-bit register
uint32_t VL53L1X_readReg32Bit(VL53L1X *sensor,uint16_t reg)
{
  uint32_t value;

	WaitForI2CFlag(I2C_ISR_BUSY);
  I2C_TransferHandling(I2C1, sensor->I2CAdress, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, (reg >> 8) & 0xFF);
	WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, reg       & 0xFF);
  WaitForI2CFlag(I2C_ISR_TC);
  I2C_TransferHandling(I2C1, sensor->I2CAdress, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  WaitForI2CFlag(I2C_ISR_RXNE);
  value  = (uint32_t)I2C_ReceiveData(I2C1) << 24;
	WaitForI2CFlag(I2C_ISR_RXNE);
  value  |= (uint32_t)I2C_ReceiveData(I2C1) << 16;
	WaitForI2CFlag(I2C_ISR_RXNE);
  value  |= (uint16_t)I2C_ReceiveData(I2C1) << 8;
	WaitForI2CFlag(I2C_ISR_RXNE);
  value  |= I2C_ReceiveData(I2C1);
  WaitForI2CFlag(I2C_ISR_STOPF);
  I2C1->ICR = I2C_ICR_STOPCF;

  return value;
}

uint8_t VL53L1X_setDistanceMode(VL53L1X *sensor, uint8_t mode)
{
  // save existing timing budget
  //uint32_t budget_us = getMeasurementTimingBudget();

  switch (mode)
  {
    case Short:
      // from VL53L1_preset_mode_standard_ranging_short_range()

      // timing config
      VL53L1X_writeReg(sensor,RANGE_CONFIG__VCSEL_PERIOD_A, 0x07);
      VL53L1X_writeReg(sensor,RANGE_CONFIG__VCSEL_PERIOD_B, 0x05);
      VL53L1X_writeReg(sensor,RANGE_CONFIG__VALID_PHASE_HIGH, 0x38);

      // dynamic config
      VL53L1X_writeReg(sensor,SD_CONFIG__WOI_SD0, 0x07);
      VL53L1X_writeReg(sensor,SD_CONFIG__WOI_SD1, 0x05);
      VL53L1X_writeReg(sensor,SD_CONFIG__INITIAL_PHASE_SD0, 6); // tuning parm default
      VL53L1X_writeReg(sensor,SD_CONFIG__INITIAL_PHASE_SD1, 6); // tuning parm default

      break;

    case Medium:
      // from VL53L1_preset_mode_standard_ranging()

      // timing config
      VL53L1X_writeReg(sensor,RANGE_CONFIG__VCSEL_PERIOD_A, 0x0B);
      VL53L1X_writeReg(sensor,RANGE_CONFIG__VCSEL_PERIOD_B, 0x09);
      VL53L1X_writeReg(sensor,RANGE_CONFIG__VALID_PHASE_HIGH, 0x78);

      // dynamic config
      VL53L1X_writeReg(sensor,SD_CONFIG__WOI_SD0, 0x0B);
      VL53L1X_writeReg(sensor,SD_CONFIG__WOI_SD1, 0x09);
      VL53L1X_writeReg(sensor,SD_CONFIG__INITIAL_PHASE_SD0, 10); // tuning parm default
      VL53L1X_writeReg(sensor,SD_CONFIG__INITIAL_PHASE_SD1, 10); // tuning parm default

      break;

    case Long: // long
      // from VL53L1_preset_mode_standard_ranging_long_range()

      // timing config
      VL53L1X_writeReg(sensor,RANGE_CONFIG__VCSEL_PERIOD_A, 0x0F);
      VL53L1X_writeReg(sensor,RANGE_CONFIG__VCSEL_PERIOD_B, 0x0D);
      VL53L1X_writeReg(sensor,RANGE_CONFIG__VALID_PHASE_HIGH, 0xB8);

      // dynamic config
      VL53L1X_writeReg(sensor,SD_CONFIG__WOI_SD0, 0x0F);
      VL53L1X_writeReg(sensor,SD_CONFIG__WOI_SD1, 0x0D);
      VL53L1X_writeReg(sensor,SD_CONFIG__INITIAL_PHASE_SD0, 14); // tuning parm default
      VL53L1X_writeReg(sensor,SD_CONFIG__INITIAL_PHASE_SD1, 14); // tuning parm default

      break;

    default:
      // unrecognized mode - do nothing
      return 0;
  }

  // reapply timing budget
  //setMeasurementTimingBudget(budget_us);

  // save mode so it can be returned by getDistanceMode()
  //sensor->distance_mode = mode;

  return 1;
}

