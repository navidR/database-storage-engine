#ifndef PAGEHEADER_HPP
#define PAGEHEADER_HPP

#include <inttypes.h>

#include "pagetype.hpp"

struct PageHeader {
public:
    PageHeader(PageType page_type,
               uint32_t page_size,
               uint32_t record_size,
               uint32_t record_count);
    PageHeader(void *);
    PageType getPageType() const;
    uint32_t getPageSize() const;
    uint32_t getRecordSize() const;
    uint32_t getRecordCount() const;
    void serialize(void *ptr) const;
    void deserialize(void *ptr);

private:
    PageType page_type;
    uint32_t page_size;     // Total size of the page, including header
    uint32_t record_size;   // Size of each record
    uint32_t record_count;  // Number of records stored in the page
//    PageHeader *next;       // The next page in the same file (described later)
};


#endif // PAGEHEADER_HPP
