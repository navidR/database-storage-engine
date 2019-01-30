#include "page.hpp"


Page::Page(Byte* ptr,
           PageType page_type,
           uint32_t page_identifier,
           uint32_t page_size,
           uint32_t record_size) :
    raw_page(ptr),
    page_header(PageHeader(page_type,
                           page_identifier,
                           page_size,
                           record_size)),
    cursor(page_header.page_header_size_raw)
{
    page_header.serialize(raw_page);
};


Page::Page(PageType page_type,
           uint32_t page_identifier,
           uint32_t page_size,
           uint32_t record_size) :
    raw_page(new Byte[page_size]),
    page_header(PageHeader(page_type,
                           page_identifier,
                           page_size,
                           record_size)),
    cursor(page_header.page_header_size_raw)
{
    page_header.serialize(raw_page);
};

Page::Page(Byte* ptr) :
    raw_page(ptr),
    page_header(raw_page),
    cursor(page_header.page_header_size_raw)
{

}

Page::~Page()
{
    delete[] raw_page;
}

void Page::serialize(Byte *ptr) const
{
    page_header.serialize(ptr);
}

void Page::deserialize(Byte *ptr)
{
    page_header.deserialize(ptr);
    this->cursor = page_header.page_header_size_raw
            + (page_header.record_count * page_header.record_size);
}


uint64_t Page::concatenate(uint32_t pid, uint32_t rid)
{
    uint64_t id = pid;
    id = (id << UINT32_WIDTH);
    return id | rid;
}

pair<uint32_t, uint32_t> Page::dissociate(uint64_t id)
{
    uint32_t pid, rid;
    rid = static_cast<uint32_t>(id);
    pid = (id >> UINT32_WIDTH);
    return make_pair(pid, rid);
}

//void Page::Delete(uint64_t rid, const uint8_t *record)
//{
//    pair<uint32_t, uint32_t> id = dissociate(rid);
//    uint32_t offset = page_header.page_header_size_raw;
//    offset += (page_header.record_size * id.second);
//    memcpy(&raw_page[offset], record, this->getMetaData().getRecordSize());
//}

//// This is very dangerous
//uint8_t *Page::DeleteLastElement()
//{
//    uint32_t id = this->getMetaData().record_count;
//    uint32_t offset = page_header.page_header_size_raw;
//    offset += (page_header.record_size * id);
//    this->getMetaData().record_count -= 1;
//    return &raw_page[offset];
//}

void Page::setNext(uint32_t next)
{
    this->page_header.setNext(next);
}

uint32_t Page::getNext() const
{
    return this->page_header.getNext();
}

uint64_t Page::writeToFile(int file_describtor) const
{
    return write(file_describtor, raw_page, page_header.page_size);
}

PageHeader& Page::getMetaData()
{
    return page_header;
}

Byte* Page::getRawData() const
{
    return raw_page;
}

PageType Page::getPageType() const
{
    return page_header.getPageType();
}
