#ifndef DIRECTORYPAGE_H
#define DIRECTORYPAGE_H

#include "page.hpp"

class DirectoryPage : public Page
{
public:

    DirectoryPage(Byte*,       // Memory for that page
                     uint32_t,   // Page identifier
                     uint32_t,   // Page size
                     uint32_t);  // Record size

    DirectoryPage(uint32_t,   // Page identifier
                  uint32_t,   // Page size
                  uint32_t);  // Record size

    DirectoryPage(Byte*);
    bool Insert(uint32_t,     // Page identifier
                uint32_t);    // Page location in file

    Page* CreatePage(uint32_t, uint32_t, uint32_t) override;

};

#endif // DIRECTORYPAGE_H
