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

Page* Page::CreatePage(uint32_t page_size, uint32_t record_size, uint32_t identifier)
{
    return new Page(PageType::DATA, // The api does not define which kind of page should get created
                    identifier,
                    page_size,
                    record_size);
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

/* Important
 * This API will always succeed unless the page has been filled.
 */
bool Page::Insert(const char *record)
{

// Remove this mess
//    for(uint8_t j = 0; j < this->getMetaData().getRecordSize(); ++j) {
//        printf("%d,", record[j]);
//    }
//    printf("\n header_size : %d, record_size : %d, record_count : %d, data size : %d\n", page_header.page_header_size_raw,
//           page_header.record_size,
//           page_header.record_count,
//           page_header.getPageDataSize());

    lock_guard<mutex> lock(record_count_lock);
    if((cursor + page_header.record_size) >= page_header.getPageSize())
        return false;

    memcpy(&raw_page[cursor], record, page_header.record_size);
    page_header.record_count++;
    cursor += page_header.record_size;
    return true;
}

const char* Page::Read(uint64_t rid)
{
    pair<uint32_t, uint32_t> id = dissociate(rid);
    assert(page_header.page_identifier != id.first && "Page identifier and first part of rid should be equal.");
    uint32_t offset = page_header.page_header_size_raw;
    offset += (page_header.record_size * id.second);
    printf("page_id : %d, record_id : %d\n", id.first, id.second);
    printf("Page::Read (record size : %d, offset : %d ): ", this->getMetaData().getRecordSize(), offset);
    for (uint32_t i = 0; i < this->getMetaData().getRecordSize(); ++i) {
        printf("%d,", raw_page[offset + i]);
    }
    printf("\n");
    return reinterpret_cast<char *>(&raw_page[offset]);
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

uint32_t Page::getNext()
{
    return this->page_header.getNext();
}

uint64_t Page::writeToFile(int file_describtor)
{
    return write(file_describtor, raw_page, page_header.page_size);
}

PageHeader& Page::getMetaData()
{
    return page_header;
}

Byte* Page::getRawData()
{
    return raw_page;
}

