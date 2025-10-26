#include <fontManager.h>
#include <font_ubuntu_mono.h>
#include <idtLoader.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <stdint.h>
#include <time.h>
#include <video.h>
#include <video_driver.h>

#define ever (;;)
#define TIME_FMT_LENGTH 6

// ======================================================
// Secciones del kernel y módulos
// ======================================================
extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize             = 0x1000;
static void *const userCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;

typedef int (*EntryPoint)();

// ======================================================
// Funciones auxiliares de inicialización
// ======================================================
void clearBSS(void *bssAddress, uint64_t bssSize) {
        memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
        return (void *)((uint64_t)&endOfKernel + PageSize * 8 -
                        sizeof(uint64_t));
}

void *initializeKernelBinary() {
        ncPrintOld("[x64BareBones]");
        ncNewline();

        ncPrintOld("CPU Vendor:");
        char buffer[10];
        ncPrintOld(cpuVendor(buffer));
        ncNewline();

        ncPrintOld("[Loading modules]");
        ncNewline();
        void *moduleAddresses[] = {userCodeModuleAddress,
                                   sampleDataModuleAddress};
        loadModules(&endOfKernelBinary, moduleAddresses);
        ncPrintOld("[Done]");
        ncNewline();
        ncNewline();

        clearBSS(&bss, &endOfKernel - &bss);
        return getStackBase();
}

// ======================================================
// MAIN DEL KERNEL
// ======================================================

int main() {
        video_init(); // Inicializa el modo gráfico (o VGA)
        load_idt();   // Inicializa la IDT

        if (videoMode == 1) {
                clearScreen(0x000000); // Pantalla negra limpia
                ncClear();             // Limpia buffer de texto del ncPrint

                // Texto principal
                const char *msg1 = "[MODO VIDEO ACTIVADO]";
                const char *msg2 = "Kernel funcionando correctamente.";

                bmp_font_t *font =
                    &font_ubuntu_mono; // usamos la fuente directamente
                int msg1_len = strlen(msg1);
                int msg2_len = strlen(msg2);

                int startX1 = (screenWidth - msg1_len * font->width) / 2;
                int startX2 = (screenWidth - msg2_len * font->width) / 2;
                int startY  = (screenHeight / 2) - font->height;

                // Banner verde oscuro de fondo
                for (int y = startY - 10; y < startY + 2 * font->height + 20;
                     y++) {
                        for (int x = startX1 - 20;
                             x < startX1 + msg1_len * font->width + 20; x++) {
                                drawPixel(x, y, 0x003300);
                        }
                }

                // Mensajes en pantalla
                for (int i = 0; msg1[i]; i++)
                        drawChar(msg1[i], startX1 + i * font->width, startY,
                                 0x00FF00, font);

                for (int i = 0; msg2[i]; i++)
                        drawChar(msg2[i], startX2 + i * font->width,
                                 startY + font->height + 10, 0xCCCCCC, font);

        } else {
                ncPrintOld("[Kernel Main]");
                ncNewline();
                ncPrintOld("  Sample code module at 0x");
                ncPrintHex((uint64_t)userCodeModuleAddress);
                ncNewline();
                ncPrintOld("  Calling the sample code module returned: ");
                ncPrintHex(((EntryPoint)userCodeModuleAddress)());
                ncNewline();
                ncNewline();

                ncPrintOld("  Sample data module at 0x");
                ncPrintHex((uint64_t)sampleDataModuleAddress);
                ncNewline();
                ncPrintOld("  Sample data module contents: ");
                ncPrintOld((char *)sampleDataModuleAddress);
                ncNewline();

                ncPrintOld("[Finished]");

                sleep(3);
                ncClear();
        }

        // Lo hago asi porque sabemos que no va a cambiar el formato, siempre
        // van a ser dos posiciones para las horas y dos para los minutos a
        // menos de que seas un enfermo mental
        char buffer[TIME_FMT_LENGTH];
        s_time time = get_current_time();
        buffer[0]   = time.hours / 10 + '0';
        buffer[1]   = (time.hours % 10 + '0');
        buffer[2]   = ':';
        buffer[3]   = time.minutes / 10 + '0';
        buffer[4]   = time.minutes % 10 + '0';
        buffer[5]   = 0;
        printLn(buffer, BLACK_WHITE);

        for
                ever;
        return 0;
}
