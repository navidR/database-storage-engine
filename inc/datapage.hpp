#ifndef DATAPAGE_H
#define DATAPAGE_H

#include "page.hpp"

class DataPage : private Page
{
public:
    DataPage(Byte*,       // Memory for that page
             uint32_t,   // Page identifier
             uint32_t,   // Page size
             uint32_t);  // Record size

    DataPage(uint32_t,   // Page identifier
             uint32_t,   // Page size
             uint32_t);  // Record size

    Page* CreatePage(uint32_t,  // Page size
                     uint32_t,  // Record size
                     uint32_t) override; // Page identifier

    bool Insert(const char *);
    const char* Read(uint64_t rid);
};

#endif // DATAPAGE_H
