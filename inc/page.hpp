#ifndef PAGE_HPP
#define PAGE_HPP

#include "pageheader.hpp"

class Page
{
public:
    Page(uint32_t);
    void serialize(void *) const;
    void deserialize(void *);

    PageHeader *pageHeader;

private:
    uint32_t page_identifier;

};

#endif // PAGE_HPP
