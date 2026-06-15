#include <colors.h>
#include <stdint.h>
#include <fontManager.h>
#include <font_ubuntu_mono.h>
#include <idtLoader.h>
#include <interrupts.h>
#include <lib.h>
#include <memory_layout.h>
#include <moduleLoader.h>
#include <memory_manager.h>
#include <naiveConsole.h>
#include <process.h>
#include <scheduler.h>
#include <semaphores.h>
#include <time.h>
#include <video_driver.h>

#define for_ever for (;;)
#define TIME_FMT_LENGTH 7

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;
extern void init_syscalls(void);
extern void setup_user_segments(void);
extern void setup_tss(void);
extern void jump_to_userland(void *entry_point);

static const uint64_t PageSize           = PAGE_SIZE;
static void *const userCodeModuleAddress = (void *)USER_CODE_MODULE_ADDR;
static void *const userDataModuleAddress = (void *)USER_DATA_MODULE_ADDR;
static void *const heapRegion2Start      = (void *)HEAP_REGION2_START;
static const uint64_t heapRegion2Size    = HEAP_REGION2_SIZE;

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
        memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
        return (void *)((uint64_t)&endOfKernel + PageSize * 8 -
                        sizeof(uint64_t));
}

void *initializeKernelBinary() {
        ncPrintOld("[DEBUG 1] initializeKernelBinary START");
        ncNewline();

        ncPrintOld("[x64BareBones]");
        ncNewline();

        ncPrintOld("[DEBUG 2] Getting CPU Vendor");
        ncNewline();
        ncPrintOld("CPU Vendor:");
        char buffer[10];
        ncPrintOld(cpuVendor(buffer));
        ncNewline();

        ncPrintOld("[DEBUG 3] Loading modules");
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

        ncPrintOld("[DEBUG 4] Clearing BSS");
        ncNewline();
        clearBSS(&bss, &endOfKernel - &bss);

        ncPrintOld("[DEBUG 5] initializeKernelBinary END");
        ncNewline();
        return getStackBase();
}

static inline void restore_cursor() {
        gfxCursorX = 0;
        gfxCursorY = 0;
}

static void enable_interrupts(void) {
        picMasterMask(PIC_MASK_TIMER_KBD);
        picSlaveMask(PIC_MASK_ALL);
        _sti();
}

static void init_heap(void) {
        uint8_t *heap_r1_start =
            (uint8_t *)((uint64_t)&endOfKernel + PageSize * 8);
        size_t heap_r1_size =
            (size_t)((uint8_t *)userCodeModuleAddress - heap_r1_start);

        heap_region_t regions[HEAP_REGION_COUNT] = {
            {heap_r1_start, heap_r1_size},
            {(uint8_t *)heapRegion2Start, heapRegion2Size},
        };
        mem_init(regions, HEAP_REGION_COUNT);
}

int main() {
        ncPrintOld("[DEBUG 6] main() START");
        ncNewline();

        ncPrintOld("[DEBUG 7] video_init()");
        ncNewline();
        video_init();

        ncPrintOld("[DEBUG 8] timer_init()");
        ncNewline();
        timer_init();

        ncPrintOld("[DEBUG 9] load_idt()");
        ncNewline();
        load_idt();

        ncPrintOld("[DEBUG 10] init_syscalls()");
        ncNewline();
        init_syscalls();

        ncPrintOld("[DEBUG 11] setup_user_segments()");
        ncNewline();
        setup_user_segments();

        ncPrintOld("[DEBUG 12] setup_tss()");
        ncNewline();
        setup_tss();

        ncPrintOld("[DEBUG 14] init_heap()");
        ncNewline();
        init_heap();

        ncPrintOld("[DEBUG 15] process_init()");
        ncNewline();
        process_init();

        ncPrintOld("[DEBUG 16] scheduler_init()");
        ncNewline();
        scheduler_init();

        ncPrintOld("[DEBUG 17] sem_system_init()");
        ncNewline();
        sem_system_init();

        ncPrintOld("[DEBUG 18] clearScreen()");
        ncNewline();
        clearScreen(BLACK);

        ncPrintOld("[DEBUG 19] setFont()");
        ncNewline();
        bmp_font_t *font = &font_ubuntu_mono;
        setFont(font);

        ncPrintOld("[DEBUG 20] restore_cursor()");
        ncNewline();
        restore_cursor();

        ncPrintOld("[DEBUG 13] enable_interrupts()");
        ncNewline();
        enable_interrupts();

        ncPrintOld("[DEBUG 21] LLEGAMOS A SALTAR A USERLAND!");
        ncNewline();
        jump_to_userland(userCodeModuleAddress);

        ncPrintOld("[DEBUG 22] NUNCA DEBERIA LLEGAR AQUI");
        ncNewline();
        for_ever _hlt();
        return 0;
}
