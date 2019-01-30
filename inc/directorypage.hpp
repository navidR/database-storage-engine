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

    DirectoryPage(PageType,   // Page Type
                     uint32_t,   // Page identifier
                     uint32_t,   // Page size
                     uint32_t);  // Record size
};

#endif // DIRECTORYPAGE_H
