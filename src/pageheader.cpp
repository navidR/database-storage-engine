#include "pageheader.hpp"


PageHeader::PageHeader(PageType page_type,
           uint32_t page_identifier,
           uint32_t page_size,
           uint32_t record_size) :
    page_type(page_type),
    page_identifier(page_identifier),
    page_size(page_size),
    record_size(record_size),
    record_count(0),
    next(0)
{

}

PageHeader::PageHeader(Byte *ptr)
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

uint32_t PageHeader::nextPage()
{
    return next;
}

void PageHeader::serialize(Byte *ptr) const
{
    // Converting ptr to page header data

    uint8_t i = 0;

    uint8_t page_type_raw = page_type == PageType::DATA ? 1 : 0;
    memcpy(&ptr[i], &page_type_raw, sizeof(uint8_t));

    i += sizeof(uint8_t);
    memcpy(&ptr[i], &page_identifier, sizeof(uint32_t));

    i += sizeof(uint32_t);
    memcpy(&ptr[i], &page_identifier, sizeof(uint32_t));

    i += sizeof(uint32_t);
    memcpy(&ptr[i], &page_size, sizeof(uint32_t));

    i += sizeof(uint32_t);
    memcpy(&ptr[i], &record_size, sizeof(uint32_t));

    i += sizeof(uint32_t);
    memcpy(&ptr[i], &record_count, sizeof(uint32_t));

    i += sizeof(uint32_t);
    memcpy(&ptr[i], &record_count, sizeof(uint32_t));
}

void PageHeader::deserialize(Byte *ptr)
{
    uint8_t i = 0;

    uint8_t page_type_raw = 0;
    memcpy(&ptr[i], &page_type_raw, sizeof(uint8_t));
    page_type = page_type_raw == 1 ? PageType::DATA : PageType::DIRECTORY;

    i += sizeof(uint8_t);
    memcpy(&page_identifier, &ptr[i], sizeof(uint32_t));

    i += sizeof(uint32_t);
    memcpy(&page_identifier, &ptr[i], sizeof(uint32_t));

    i += sizeof(uint32_t);
    memcpy(&page_size, &ptr[i], sizeof(uint32_t));

    i += sizeof(uint32_t);
    memcpy(&record_size, &ptr[i], sizeof(uint32_t));

    i += sizeof(uint32_t);
    memcpy(&record_count, &ptr[i], sizeof(uint32_t));

    i += sizeof(uint32_t);
    memcpy(&record_count, &ptr[i], sizeof(uint32_t));
}
