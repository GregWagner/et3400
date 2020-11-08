#include "memory_map.h"

memory_map::memory_map()
{
    for (int i = 0; i < 64; i++)
    {
        blocks[i].device = NULL;
    }
}

memory_map::~memory_map()
{
}

void memory_map::map(int block, memory_mapped_device *device)
{
    if (blocks[block].device == NULL)
    {
        blocks[block].device = device;
    }
    else
    {
        memory_mapped_device *current_device = blocks[block].device;
        while (current_device->next != NULL)
        {
            current_device = current_device->next;
        }
        current_device->next = device;
    }
}

uint8_t memory_map::read(offs_t addr)
{
    memory_mapped_device *device = get_block_device(addr);
    if (device == NULL)
        return 0;
    return device->read(addr);
};

void memory_map::write(offs_t addr, uint8_t data)
{
    memory_mapped_device *device = get_block_device(addr);
    if (device != NULL)
    {
        device->write(addr, data);
    }
};

memory_mapped_device *memory_map::get_block_device(off_t address)
{
    int block = address / 1024;
    memory_mapped_device *device = blocks[block].device;
    while (device != NULL && !device->is_mapped(address))
    {
        device = device->next;
    } 
    return device;
}

