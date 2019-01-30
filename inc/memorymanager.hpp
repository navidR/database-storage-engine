#ifndef MEMORYMANAGER_HPP
#define MEMORYMANAGER_HPP

#include <types.hpp>

class MemoryManager
{
public:
    MemoryManager(uint32_t,  // Coalesce ratio
                  uint32_t); // Page size


private:
    const uint32_t coalesce_ratio;
    const uint32_t page_size;
    uint32_t cursor;
};

#endif // MEMORYMANAGER_HPP
