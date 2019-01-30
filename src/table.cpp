#include "table.hpp"

Table::Table(const char *filename) :
    filename(filename)
{
    file_descriptor = open(filename,
                           O_DIRECT | O_SYNC | O_RDWR | O_CREAT,
                           S_IRWXU | S_IRWXG);
    if(file_descriptor == -1)
    {
        int save_errno = errno;
        LOG(FATAL) << "Cannot open file : " << filename << ", error : " << strerror(save_errno);
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

    Page *page = new Page(allocatePage(page_size),
                          PageType::DIRECTORY,
                          nextIdentifier(),
                          page_size,
                          record_size);
    main_map[page->getMetaData().getPageIdentifier()] = page;
    head_directory = page;

    page = new Page(allocatePage(page_size),
                    PageType::DATA,
                    nextIdentifier(),
                    page_size,
                    record_size);
    main_map[page->getMetaData().getPageIdentifier()] = page;
    head_data = page;
}

Table::~Table()
{
    flush();

    for(const auto& item : main_map)
    {
        free(item.second);
    }
    LOG(INFO) << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__;

    // Close the file
    if(close(file_descriptor))
    {
        int save_errno = errno;
        LOG(FATAL) << "Cannot close file : " << filename << ", error : " << strerror(save_errno);
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
    Byte* ptr = allocatePage(main_map[0]->getMetaData().getPageSize());
    Page* page = new Page(ptr,
                          PageType::DATA,
                          nextIdentifier(),
                          main_map[0]->getMetaData().getPageSize(),
                          main_map[0]->getMetaData().getRecordSize());
    head_data = page;
    head_data->Insert(record);
    return true;
}

bool Table::Read(uint64_t rid, char *buf)
{
    uint32_t page_id, record_id;
    tie(page_id, record_id) = Page::dissociate(rid);
    if(page_id > head_data->getMetaData().getPageIdentifier() ||
            record_id > head_data->getMetaData().getPageMaxRecordCount())
    {
        return false;
    }

    // Check the hash map
    if(main_map.find(page_id) != main_map.end())
    {
        Page *page = main_map[page_id];

//        printf("Table::Read (record size : %d ): ", page->getMetaData().getRecordSize());
//        for (uint32_t i = 0; i < page->getMetaData().getRecordSize(); ++i) {
//            printf("%d,", page->Read(rid)[i]);
//        }
//        printf("\n");
        memcpy(buf, page->Read(rid), page->getMetaData().getRecordSize());
        return true;
    }
    // Read that page from file
    else
    {
        auto offset = page_id * main_map[0]->getMetaData().getPageSize();
        auto i = lseek(file_descriptor, offset, SEEK_SET);
        if(i == -1){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot seek file : " << filename << " to " << offset << " error : " << strerror(save_errno);
        }
        Byte* page_buffer = allocatePage(main_map[0]->getMetaData().getPageSize());
        Page *page = new Page(page_buffer);
        main_map[page->getMetaData().getPageIdentifier()] = page;
        memcpy(buf, page->Read(rid), page->getMetaData().getRecordSize());
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

void Table::flush()
{
    for (const auto& item : main_map)
    {
        auto offset = item.second->getMetaData().getPageIdentifier() * item.second->getMetaData().getPageSize();
        printf("offset : %d , page_id : %d, page_size : %d\n", offset, item.second->getMetaData().getPageIdentifier(), item.second->getMetaData().getPageSize());
        auto i = lseek(file_descriptor, offset, SEEK_SET);
        if(i == -1){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot seek file : " << filename << " to " << offset << " error : " << strerror(save_errno);
        }
        i = item.second->writeToFile(file_descriptor);
        if(i == -1){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot write to file : " << filename << " to " << offset << " error : " << strerror(save_errno);
        }
    };
}
