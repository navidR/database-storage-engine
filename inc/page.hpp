#ifndef PAGE_HPP
#define PAGE_HPP

#include "pageheader.hpp"

#include <mutex>
#include <memory>

using std::mutex;
using std::unique_ptr;
using std::make_unique;

class Page
{
public:
    Page(uint32_t, uint32_t, uint32_t); /* (page_identifier, page_size, record_size) */
    void serialize(void *) const;
    void deserialize(void *);
    bool Insert(const char *record);
    const char* Read(uint64_t rid);
    void setNext(Page*);
    void writeToFile(int file_describtor);

    static Page *CreatePage(uint32_t page_size, uint32_t record_size);
    static uint32_t nextIdentifier();

    PageHeader *pageHeader;

private:
    Page *next;

    const uint32_t page_identifier;
    const uint32_t page_size;
    const uint32_t record_size;

    uint32_t cursor;
    mutex cursor_lock;

    static uint32_t identifiers;
    static mutex identifier_lock;
};

#endif // PAGE_HPP
