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
    Page(PageType,   // Page Type
         uint32_t,   // Page identifier
         uint32_t,   // Page size
         uint32_t);  // Record size

    void serialize(void *) const;
    void deserialize(void *);
    bool Insert(const char *record);
    const char* Read(uint64_t rid);
    void setNext(uint32_t);
    void writeToFile(int file_describtor);

    static Page *CreatePage(uint32_t page_size, uint32_t record_size);
    static uint32_t nextIdentifier();

private:
    PageHeader page_header;

    uint32_t cursor;
    mutex cursor_lock;

    inline static uint32_t identifiers = 0;
    inline static mutex identifier_lock;
};

#endif // PAGE_HPP
