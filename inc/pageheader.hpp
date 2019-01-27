#ifndef PAGEHEADER_HPP
#define PAGEHEADER_HPP

#include <inttypes.h>

#include "pagetype.hpp"

struct PageHeader {
public:
    PageHeader(PageType page_type,
               uint32_t page_size,
               uint32_t record_size,
               uint32_t record_count) :
        page_type(page_type),
        page_size(page_size),
        record_size(record_size),
        record_count(record_count)
    {

    }

    PageType getPageType() const
    {
        return page_type;
    }

    uint32_t getPageSize() const
    {
        return page_size;
    }

    uint32_t getRecordSize() const
    {
        return record_size;
    }

    uint32_t getRecordCount() const
    {
        return record_count;
    }

private:
    const PageType page_type;
    const uint32_t page_size;     // Total size of the page, including header
    const uint32_t record_size;   // Size of each record
    uint32_t record_count;  // Number of records stored in the page
//    PageHeader *next;       // The next page in the same file (described later)
};


#endif // PAGEHEADER_HPP
