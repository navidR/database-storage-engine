#ifndef PAGE_HPP
#define PAGE_HPP

#include "pageheader.hpp"

#include <glog/logging.h>

#include <mutex>
#include <memory>
#include <tuple>

using std::mutex;
using std::unique_ptr;
using std::make_unique;
using std::lock_guard;
using std::pair;
using std::make_pair;

class Page
{
public:
    Page(PageType,   // Page Type
         uint32_t,   // Page identifier
         uint32_t,   // Page size
         uint32_t);  // Record size

    Page(Byte*);
    ~Page();

    void serialize(Byte *) const;
    void deserialize(Byte *);
    bool Insert(const char *);
    const char* Read(uint64_t rid);
    void Delete(uint64_t, const uint8_t *);
    uint8_t* DeleteLastElement();
    void setNext(uint32_t);
    uint32_t getNext();
    void writeToFile(int);
    PageHeader& getMetaData();
    Byte* getRawData();

    static Page* CreatePage(uint32_t, uint32_t);
    static uint32_t nextIdentifier();
    static uint64_t concatenate(uint32_t, uint32_t);
    static pair<uint32_t, uint32_t> dissociate(uint64_t);

private:
//    Byte *raw_page;
    Byte* raw_page;

    PageHeader page_header;
    Byte *cursor_ptr;
    uint32_t max_record_count;
    mutex record_count_lock;

    inline static uint32_t identifiers = 0;
    inline static mutex identifier_lock;
};

#endif // PAGE_HPP
