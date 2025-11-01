#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <uint.h>

static void loadModule(uint8_t **module, void *targetModuleAddress);
static uint32_t readUint32(uint8_t **address);

void loadModules(void *payloadStart, void **targetModuleAddress) {
        int i                  = 3;
        uint8_t *currentModule = (uint8_t *)payloadStart;
        uint32_t moduleCount   = readUint32(&currentModule);

        for (i = 0; i < moduleCount; i++)
                loadModule(&currentModule, targetModuleAddress[i]);
}

static void loadModule(uint8_t **module, void *targetModuleAddress) {
        uint32_t moduleSize = readUint32(module);

        ncPrintOld("  Will copy module at 0x");
        ncPrintHex((uint64_t)*module);
        ncPrintOld(" to 0x");
        ncPrintHex((uint64_t)targetModuleAddress);
        ncPrintOld(" (");
        ncPrintDec(moduleSize);
        ncPrintOld(" bytes)");

        memcpy(targetModuleAddress, *module, moduleSize);
        *module += moduleSize;

        ncPrintOld(" [Done]");
        ncNewline();
}

static uint32_t readUint32(uint8_t **address) {
        uint32_t result = *(uint32_t *)(*address);
        *address += sizeof(uint32_t);
        return result;
}
