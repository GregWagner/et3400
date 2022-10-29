#include "memory_map.h"

MemoryMapManager::MemoryMapManager() {
    for (int i = 0; i < 64; i++) {
        blocks[i].device = NULL;
    }
}

MemoryMapManager::~MemoryMapManager() {
}

void MemoryMapManager::map(memory_mapped_device* device) {
    int block_start = device->get_start() / BLOCK_SIZE;
    int block_end = device->get_end() / BLOCK_SIZE;

    for (int block = block_start; block <= block_end; block++) {
        if (blocks[block].device == NULL) {
            blocks[block].device = device;
        } else {
            memory_mapped_device* current_device = blocks[block].device;
            while (current_device->next != NULL) {
                current_device = current_device->next;
            }
            current_device->next = device;
        }
    }
}

uint8_t MemoryMapManager::read(offs_t addr) {
    memory_mapped_device* device = get_block_device(addr);
    if (device == NULL)
        return 0;
    return device->read(addr);
};

void MemoryMapManager::write(offs_t addr, uint8_t data) {
    memory_mapped_device* device = get_block_device(addr);
    if (device != NULL) {
        device->write(addr, data);
    }
};

memory_mapped_device* MemoryMapManager::get_block_device(off_t address) {
    int block = address / BLOCK_SIZE;
    memory_mapped_device* device = blocks[block].device;
    while (device != NULL && !device->is_mapped(address)) {
        device = device->next;
    }
    return device;
}
