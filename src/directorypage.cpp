#include "directorypage.hpp"

DirectoryPage::DirectoryPage(Byte* ptr,                  // Memory for that page
                             uint32_t page_identifier,   // Page identifier
                             uint32_t page_size,         // Page size
                             uint32_t record_size)
    : Page(ptr,
           PageType::DIRECTORY,
           page_identifier,
           page_size,
           record_size)
{

}

DirectoryPage::DirectoryPage(uint32_t page_identifier,
                             uint32_t page_size,
                             uint32_t record_size)
    : Page(PageType::DIRECTORY,
           page_identifier,
           page_size,
           record_size)
{

}

Page* DirectoryPage::CreatePage(uint32_t identifier,
                                uint32_t page_size,
                                uint32_t record_size)
{
    return new DirectoryPage(identifier,
                             page_size,
                             record_size);
}

DirectoryPage::DirectoryPage(Byte* ptr)
    : Page(ptr)
{
    if(this->getPageType() != PageType::DIRECTORY)
        LOG(WARNING) << "Constructing DirectoryPage from DataPage. page_id : " << this->page_header.getPageIdentifier();

}


bool DirectoryPage::Insert(uint32_t page_identifier,
                           uint32_t page_location)
{
    lock_guard<mutex> lock(record_count_lock);
    if((cursor + sizeof(uint64_t)) >= page_header.getPageSize())
        return false;

    uint64_t record = Page::concatenate(page_identifier, page_location);
    memcpy(&raw_page[cursor], &record, page_header.getRecordSize());
    page_header.increaseRecordCountByOne();
    cursor += page_header.getRecordSize();
    return true;
}
