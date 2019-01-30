#include "datapage.hpp"

DataPage::DataPage(Byte* ptr,       // Memory for that page
                   uint32_t page_identifier,   // Page identifier
                   uint32_t page_size,   // Page size
                   uint32_t record_size)  // Record size
    : Page (ptr,
            PageType::DATA,
            page_identifier,
            page_size,
            record_size)
{

}

DataPage::DataPage(uint32_t page_identifier,   // Page identifier
                   uint32_t page_size,   // Page size
                   uint32_t record_size)   // Record size
    : Page(PageType::DATA,
           page_identifier,
           page_size,
           record_size)
{

}

DataPage::DataPage(Byte* ptr)
    : Page(ptr)
{

}

Page* DataPage::CreatePage(uint32_t page_size, uint32_t record_size, uint32_t identifier)
{
    return new DataPage(identifier,
                        page_size,
                        record_size);
}

/* Important
 * This API will always succeed unless the page has been filled.
 */
bool DataPage::Insert(const char *record)
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
    if((cursor + page_header.getRecordSize()) >= page_header.getPageSize())
        return false;

    memcpy(&raw_page[cursor], record, page_header.getRecordSize());
    page_header.increaseRecordCountByOne();
    cursor += page_header.getRecordSize();
    return true;
}

const char* DataPage::Read(uint64_t rid)
{
    pair<uint32_t, uint32_t> id = dissociate(rid);
    assert(page_header.page_identifier != id.first && "Page identifier and first part of rid should be equal.");
    uint32_t offset = page_header.page_header_size_raw;
    offset += (page_header.getRecordSize() * id.second);
    return reinterpret_cast<char *>(&raw_page[offset]);
}
