#include "user_comm.h"

#define  CHARG_FINISHED       0x05


void EEWriteU8(uint16_t address, uint8_t value)
{
 // Check if the EEPROM is write-protected. If it is then unlock the EEPROM.
 if (FLASH_IAPSR_bits.DUL == 0) {
 FLASH_DUKR = 0xAE;
 FLASH_DUKR = 0x56;
 }
 // Write the data to the EEPROM.
 (*(uint8_t *) (0x4000 + address)) = value;
 // Now write protect the EEPROM.
 FLASH_IAPSR_bits.DUL = 0;
}