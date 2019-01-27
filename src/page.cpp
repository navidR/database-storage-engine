#include "page.hpp"

Page::Page(uint32_t page_identifier) :
    page_identifier(page_identifier)
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

}


bool Page::Insert(const char *record)
{

}

const char* Page::Read(uint64_t rid)
{

}
