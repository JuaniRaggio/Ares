#include <fontManager.h>
#include <font_ubuntu_mono.h>
#include <idtLoader.h>
#include <interrupts.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <time.h>
#include <video_driver.h>

#define ever (;;)
#define TIME_FMT_LENGTH 7

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;
extern void init_syscalls(void);
extern void setup_user_segments(void);
extern void jump_to_userland(void *entry_point);

static const uint64_t PageSize           = 0x1000;
static void *const userCodeModuleAddress = (void *)0x400000;
static void *const userDataModuleAddress = (void *)0x500000;

typedef int (*EntryPoint)();

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
        void *moduleAddresses[] = {
            userCodeModuleAddress,
            userDataModuleAddress,
        };
        loadModules(&endOfKernelBinary, moduleAddresses);
        ncPrintOld("[Done]");
        ncNewline();
        ncNewline();

        clearBSS(&bss, &endOfKernel - &bss);
        return getStackBase();
}

static inline void restore_cursor() {
        gfxCursorX = 0;
        gfxCursorY = 0;
}

int main() {
        video_init();
        load_idt();
        timer_init();
        init_syscalls();
        setup_user_segments();

        clearScreen(0x000000);

        bmp_font_t *font = &font_ubuntu_mono;
        setFont(font);

        char buffer[TIME_FMT_LENGTH];
        s_time time = get_current_time();
        buffer[0]   = time.hours / 10 + '0';
        buffer[1]   = (time.hours % 10 + '0');
        buffer[2]   = ':';
        buffer[3]   = time.minutes / 10 + '0';
        buffer[4]   = time.minutes % 10 + '0';
        buffer[5]   = '\n';
        buffer[6]   = 0;
        ncPrint(buffer, VGA_WHITE);
        ncPrint(buffer, VGA_WHITE);
        ncPrint(buffer, VGA_WHITE);
        ncPrint(buffer, VGA_WHITE);
        ncPrint(buffer, VGA_WHITE);
        ncPrint(buffer, VGA_WHITE);

        restore_cursor();

        ncPrint("UserCodeModule begins at: ", VGA_WHITE);
        ncPrintHex((uint64_t)userCodeModuleAddress);
        ncPrint("\n", VGA_WHITE);
        ncPrint("OK!", VGA_WHITE);

        jump_to_userland(userCodeModuleAddress);

        while (1)
                haltcpu();
        return 0;
}
