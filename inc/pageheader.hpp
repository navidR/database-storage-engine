#ifndef PAGEHEADER_HPP
#define PAGEHEADER_HPP

#include <cstring>

#include "types.hpp"
#include "pagetype.hpp"


using std::memcpy;

struct PageHeader {
public:
    PageHeader(PageType,   // page_type
               uint32_t,   // page_identifier
               uint32_t,   // page_size
               uint32_t);   // record_size
    PageHeader(Byte *);
    PageType getPageType() const;
    uint32_t getPageSize() const;
    uint32_t getRecordSize() const;
    uint32_t getRecordCount() const;
    uint32_t getPageIdentifier() const;
    uint32_t nextPage();
    void serialize(Byte *ptr) const;
    void deserialize(Byte *ptr);
    void setNext(uint32_t next);
    uint32_t getNext();


    // C++ definitely need introspection
    const inline static uint32_t page_header_size_raw = sizeof(uint8_t)   // PageType as uint8_t
                + sizeof(uint32_t)  // page_identifier as uint32_t
                + sizeof(uint32_t)  // page_size as uint32_t
                + sizeof(uint32_t)  // record_size as uint32_t
                + sizeof(uint32_t)  // record_count as uint32_t
                + sizeof(uint32_t); // next as uint32_t

private:
    PageType page_type;
    uint32_t page_identifier;
    uint32_t page_size;     // Total size of the page, including header
    uint32_t record_size;   // Size of each record
    uint32_t record_count;  // Number of records stored in the page
    uint32_t next;
};


#endif // PAGEHEADER_HPP
