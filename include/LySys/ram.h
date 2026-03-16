#pragma once

#include <LySys/pmm.h>
#include <LySys/types.h>

static uint64_t GetTotalRam() {
    return (uint64_t)total_blocks * PAGE_SIZE;
}

static uint32_t GetTotalRamMB() {
    return (uint32_t)(GetTotalRam() / (1024 * 1024));
}

static uint64_t GetUsedRam(uint32_t used_blocks) {
    return (uint64_t)used_blocks * PAGE_SIZE;
}

static uint32_t GetUsedRamMB(uint32_t used_blocks) {
    return (uint32_t)(GetUsedRam(used_blocks) / (1024 * 1024));
}

static uint32_t GetFreeRamMB(uint32_t used_blocks) {
    return GetTotalRamMB() - GetUsedRamMB(used_blocks);
}