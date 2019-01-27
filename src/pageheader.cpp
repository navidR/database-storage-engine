#include "pageheader.hpp"


PageHeader::PageHeader(PageType page_type,
           uint32_t page_size,
           uint32_t record_size,
           uint32_t record_count) :
    page_type(page_type),
    page_size(page_size),
    record_size(record_size),
    record_count(record_count)
{

}

PageHeader::PageHeader(void *ptr)
{
    deserialize(ptr);
}

PageType PageHeader::getPageType() const
{
    return page_type;
}

uint32_t PageHeader::getPageSize() const
{
    return page_size;
}

uint32_t PageHeader::getRecordSize() const
{
    return record_size;
}

uint32_t PageHeader::getRecordCount() const
{
    return record_count;
}

void PageHeader::serialize(void *ptr) const
{

}

void PageHeader::deserialize(void *ptr)
{

}
