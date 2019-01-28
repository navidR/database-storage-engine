#include "page.hpp"

Page::Page(PageType page_type,
           uint32_t page_identifier,
           uint32_t page_size,
           uint32_t record_size) :
    page_header(PageHeader(page_type,
                           page_identifier,
                           page_size,
                           record_size,
                           0)),
    cursor(0)
{

};

void Page::serialize(void *ptr) const
{

}

void Page::deserialize(void *ptr)
{

}

Page* Page::CreatePage(uint32_t page_size, uint32_t record_size)
{
    return new Page(nextIdentifier(),
                    page_size,
                    record_size);
}

uint32_t Page::nextIdentifier()
{
    identifier_lock.lock();
    int the_identifier = identifiers;
    identifiers++;
    identifier_lock.unlock();
    return the_identifier;
}


bool Page::Insert(const char *record)
{
    cursor_lock.lock();

    cursor++;
    cursor_lock.unlock();
    return true; // Why true?
}

const char* Page::Read(uint64_t rid)
{

}

void Page::setNext(Page* next)
{
    this->next = next;
}

void Page::writeToFile(int file_describtor)
{

    return;
}


