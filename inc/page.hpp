#ifndef PAGE_HPP
#define PAGE_HPP

#include "pageheader.hpp"

class Page
{
public:
    Page(uint32_t);
    void serialize(void *) const;
    void deserialize(void *);
    bool Insert(const char *record);
    const char* Read(uint64_t rid);
//    bool writeToFile();

    static Page *CreatePage(uint32_t page_size, uint32_t record_size);

    PageHeader *pageHeader;

private:
    uint32_t page_identifier;
};

#endif // PAGE_HPP
