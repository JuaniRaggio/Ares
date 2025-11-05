#ifndef MODULELOADER_H
#define MODULELOADER_H

/**
 * Loads kernel modules from the bootloader payload
 * @param payloadStart Pointer to the start of the payload section
 * @param moduleTargetAddress Array of pointers where modules will be loaded
 */
void loadModules(void *payloadStart, void **moduleTargetAddress);

#endif
