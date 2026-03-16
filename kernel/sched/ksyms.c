#include <LySys/LySystem.h>
#include <LySys/pmm.h>
#include <LySys/string.h>

typedef struct {
    const char* name;
    uint64_t addr;
} kernel_symbol_t;

kernel_symbol_t ksyms[] = {
    {"printk", (uint64_t)printk},
    {"memset", (uint64_t)memset},
    {"memcpy", (uint64_t)memcpy},
    {0, 0}
};

uint64_t find_ksym(const char* name) {
    for (int i = 0; ksyms[i].name != 0; i++) {
        if (strcmp(ksyms[i].name, name) == 0) return ksyms[i].addr;
    }
    return 0;
}