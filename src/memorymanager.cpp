#include "memorymanager.hpp"

MemoryManager::MemoryManager(uint32_t coalesce_ratio,
                             uint32_t page_size) :
    coalesce_ratio(coalesce_ratio),
    page_size(page_size),
    cursor(0)
{

}
