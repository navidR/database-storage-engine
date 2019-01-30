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

}
