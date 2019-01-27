#ifndef PAGE_HPP
#define PAGE_HPP

#include "pageheader.hpp"

class Page
{
public:
    Page(uint32_t);
    void serialize(void *) const;
    void deserialize(void *);

    /* Create a page with specified page size and record size */
    static Page *CreatePage(uint32_t page_size, uint32_t record_size);

    /* Insert a record. Returns true if the insertion was successful, false
     * otherwise. There is no need to check for whether the record already
     * exists in the page (i.e., duplication is allowed).
     */
    bool Insert(const char *record);

    /* Read a record with a given RID. Returns a pointer to the record,
     * NULL if it does not exist. Note that the higher order bits in rid refer
     * to the page ID, and the lower 32 bits refer to the slot ID.
     */
    const char *Read(uint64_t rid);

    PageHeader *pageHeader;

private:
    uint32_t page_identifier;

};

#endif // PAGE_HPP
