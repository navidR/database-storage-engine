#include "table.hpp"

Table::Table(const char *filename) :
    filename(filename)
{

#ifdef WITH_O_DIRECT
    file_descriptor = open(filename,
                           O_DIRECT | O_RDWR | O_CREAT,
                           S_IRWXU | S_IRWXG);

#else
    file_descriptor = open(filename,
                           O_RDWR | O_CREAT,
                           S_IRWXU | S_IRWXG);
#endif
    if(file_descriptor == -1)
    {
        int save_errno = errno;
        LOG(FATAL) << "Cannot open file : " << filename
                   << ", error : " << strerror(save_errno);
    }
//    This requires access to device
//    get_block_sector_size(file_descriptor, &block_size);
#ifndef NDEBUG
    LOG(FATAL) << "Block size in this device is : " << DBLOCK_SIZE;
#endif
    tie(system_avail_mem, system_page_size) = getTotalSystemMemory();
//    struct stat st;
//    stat(filename, &st);
//    uint64_t size = static_cast<uint64_t>(st.st_size);
//    if(!size){

//    }

}

Table::Table(const char *filename,
             uint32_t page_size,
             uint32_t record_size) :
    Table(filename)
{
    Page *page = new DirectoryPage(allocatePage(page_size),
                                      nextIdentifier(),
                                      page_size,
                                      record_size);
    main_map[page->getMetaData().getPageIdentifier()] = page;
    head_directory = dynamic_cast<DirectoryPage*>(page);
    page = new DataPage(allocatePage(page_size),
                            nextIdentifier(),
                            page_size,
                            record_size);
    main_map[page->getMetaData().getPageIdentifier()] = page;
    head_data = dynamic_cast<DataPage*>(page);
    head_directory->Insert(head_data->getMetaData().getPageIdentifier(),
                           head_data->getMetaData().getPageIdentifier());
    this->page_size = page_size;
    this->record_size = record_size;
    this->page_header_size_raw = page->getMetaData().page_header_size_raw;
}

Table::~Table()
{
    clear();

    delete head_data;
    delete head_directory;

    // Close the file
    if(close(file_descriptor))
    {
        int save_errno = errno;
        LOG(FATAL) << "Cannot close file : " << filename
                   << ", error : " << strerror(save_errno);
    }
}

int Table::get_file_descriptor()
{
    return file_descriptor;
}

// Should check for BadAlloc and then flush to disk if there is exception
bool Table::Insert(const char *record)
{
    if(head_data->Insert(record))
        return true;
    Byte* ptr = allocatePage(page_size);
    Page* page = new DataPage(ptr,
                              nextIdentifier(),
                              page_size,
                              record_size);

    head_data->setNext(page->getMetaData().getPageIdentifier());
    head_data = dynamic_cast<DataPage*>(page);
    head_data->Insert(record);
    main_map[head_data->getMetaData().getPageIdentifier()] = head_data;
    return true;
}

uint64_t Table::InsertWithReturnRID(const char *record)
{
    Insert(record);
    return Page::concatenate(head_data->getMetaData().getPageIdentifier(),
                             head_data->getMetaData().getRecordCount() - 1);
}

void Table::Delete(uint32_t rid)
{
    uint8_t* last_element_ptr = head_data->DeleteLastElement();
    uint32_t page_id, record_id;
    tie(page_id, record_id) = Page::dissociate(rid);
    if(main_map.find(page_id) != main_map.end())
    {
        // Check the hash map
        Page *page = main_map[page_id];
        if(page->getPageType() != PageType::DATA)
        {
            LOG(WARNING) << "Trying to read data from DirectoryPage. page_id : " << page_id;
            return;
        }
        page->Replace(rid, last_element_ptr);
        return;
    }
    else
    {
        // Read that page from file
        uint32_t offset = (page_id * page_size);
        auto i = lseek(file_descriptor, offset, SEEK_SET);
        if(i == -1){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot seek file : " << filename
                       << " to " << offset
                       << " error : " << strerror(save_errno);
        }
        Byte* page_buffer = allocatePage(page_size);
        i = read(file_descriptor, page_buffer, page_size);
        if(i == -1 || i != page_size){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot read file : " << filename <<
                          " to " << offset <<
                          " error : " << strerror(save_errno);
        }
        Page *page = new DataPage(page_buffer);
        if(page->getPageType() != PageType::DATA)
        {
            LOG(WARNING) << "Trying to read data from DirectoryPage. page_id : " << page_id;
            return;
        }
        main_map[page->getMetaData().getPageIdentifier()] = page;
        page->Replace(rid, last_element_ptr);
        return;
    }


}

bool Table::Read(uint64_t rid, char *buf)
{
    uint32_t page_id, record_id;
    tie(page_id, record_id) = Page::dissociate(rid);
    if(record_id > head_data->getMetaData().getPageMaxRecordCount())
    {
        return false;
    }
    if(main_map.find(page_id) != main_map.end())
    {
        // Check the hash map
        Page *page = main_map[page_id];
        if(page->getPageType() != PageType::DATA)
        {
            LOG(WARNING) << "Trying to read data from DirectoryPage. page_id : " << page_id;
            return false;
        }
        memcpy(buf, dynamic_cast<DataPage*>(page)->Read(rid), page->getMetaData().getRecordSize());
        return true;
    }
    else
    {
        // Read that page from file
        uint32_t offset = (page_id * page_size);
        auto i = lseek(file_descriptor, offset, SEEK_SET);
        if(i == -1){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot seek file : " << filename
                       << " to " << offset
                       << " error : " << strerror(save_errno);
        }
        Byte* page_buffer = allocatePage(page_size);
        i = read(file_descriptor, page_buffer, page_size);
        if(i == -1 || i != page_size){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot read file : " << filename <<
                          " to " << offset <<
                          " error : " << strerror(save_errno);
        }
        Page *page = new DataPage(page_buffer);
        if(page->getPageType() != PageType::DATA)
        {
            LOG(WARNING) << "Trying to read data from DirectoryPage. page_id : " << page_id;
            return false;
        }
        main_map[page->getMetaData().getPageIdentifier()] = page;
        memcpy(buf, dynamic_cast<DataPage*>(page)->Read(rid), page->getMetaData().getRecordSize());
        return true;
    }
}

// TODO: Fix memory function
pair<uint64_t, uint64_t> Table::getTotalSystemMemory()
{
    const uint64_t system_num_pages = static_cast<uint64_t>(sysconf(_SC_PHYS_PAGES));
    const uint64_t system_page_size = static_cast<uint64_t>(sysconf(_SC_PAGE_SIZE));
    const uint64_t avail_mem = system_num_pages * system_page_size;
#ifndef NDEBUG
    LOG(INFO) << "Page size in the system memory is : " << system_page_size;
    LOG(INFO) << "Number of pages in system memory : " << system_num_pages;
    LOG(INFO) << "Available memory in the system : " << avail_mem;
#endif
    return make_pair(avail_mem, system_page_size);
}

// This need access to the device.
void Table::get_block_sector_size(int fd, int *sector_size)
{
    if(ioctl(fd, BLKSSZGET, sector_size) == -1)
    {
        auto save_errno = errno;
        LOG(FATAL) << "Cannot get the block size in this device, error : " << strerror(save_errno);
    }
}

uint32_t Table::nextIdentifier()
{
    uint32_t the_identifier = identifiers;
    identifiers++;
    return the_identifier;
}

Byte* Table::allocatePage(uint32_t page_size)
{
    Byte* ptr = static_cast<Byte*>(aligned_alloc(DBLOCK_SIZE, page_size));
    memset(ptr, 0, page_size);
    return ptr;
}

void Table::clear()
{
    LOG(INFO) << "Flushing " << main_map.size() << " Number of pages.";

    for (const auto& item : main_map)
    {
        auto offset = item.second->getMetaData().getPageIdentifier() * item.second->getMetaData().getPageSize();
//        printf("offset : %d , page_id : %d, page_size : %d, page_header_size_raw : %d\n", offset, item.second->getMetaData().getPageIdentifier(), item.second->getMetaData().getPageSize(), item.second->getMetaData().page_header_size_raw);
        auto i = lseek(file_descriptor, offset, SEEK_SET);
        if(i == -1){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot seek file : " << filename
                       << " to " << offset
                       << " error : " << strerror(save_errno);
        }
        i = item.second->writeToFile(file_descriptor);
        if(i == -1){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot write to file : " << filename
                       << " to " << offset
                       << " error : " << strerror(save_errno);
        }
    };

    for(const auto& item : main_map)
    {
        if(item.second == head_data ||
                item.second == head_directory)
            continue;
        delete item.second;
    }

    main_map.clear();
    main_map[head_directory->getMetaData().getPageIdentifier()] = head_directory;
    main_map[head_data->getMetaData().getPageIdentifier()] = head_data;
}

void Table::writeToDisk()
{
    LOG(INFO) << "Writing to disk " << main_map.size() << " Number of pages.";

    for (const auto& item : main_map)
    {
        auto offset = item.second->getMetaData().getPageIdentifier() * item.second->getMetaData().getPageSize();
//        printf("offset : %d , page_id : %d, page_size : %d, page_header_size_raw : %d\n", offset, item.second->getMetaData().getPageIdentifier(), item.second->getMetaData().getPageSize(), item.second->getMetaData().page_header_size_raw);
        auto i = lseek(file_descriptor, offset, SEEK_SET);
        if(i == -1){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot seek file : " << filename
                       << " to " << offset
                       << " error : " << strerror(save_errno);
        }
        i = item.second->writeToFile(file_descriptor);
        if(i == -1){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot write to file : " << filename
                       << " to " << offset
                       << " error : " << strerror(save_errno);
        }
    };

    main_map[head_directory->getMetaData().getPageIdentifier()] = head_directory;
    main_map[head_data->getMetaData().getPageIdentifier()] = head_data;
}
