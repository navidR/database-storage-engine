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
    flush();

    for(const auto& item : main_map)
    {
        free(item.second);
    }

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


bool Table::Read(uint64_t rid, char *buf)
{
    uint32_t page_id, record_id;
    tie(page_id, record_id) = Page::dissociate(rid);
    if(record_id > head_data->getMetaData().getPageMaxRecordCount())
    {
        return false;
    }

    // Check the hash map
    if(main_map.find(page_id) != main_map.end())
    {
        Page *page = main_map[page_id];
        if(page->getPageType() != PageType::DATA)
        {
            LOG(WARNING) << "Trying to read data from DirectoryPage. page_id : " << page_id;
            return false;
        }


//        printf("Table::Read (record size : %d ): ", page->getMetaData().getRecordSize());
//        for (uint32_t i = 0; i < page->getMetaData().getRecordSize(); ++i) {
//            printf("%d,", page->Read(rid)[i]);
//        }
//        printf("\n");
        memcpy(buf, dynamic_cast<DataPage*>(page)->Read(rid), page->getMetaData().getRecordSize());
        return true;
    }
    // Read that page from file
    else
    {
        uint32_t offset = (page_id * page_size);
//        LOG(INFO) << "Reading offset " << offset << " and page_id is " << page_id << " and record_id is " << record_id << " page_header_size_raw : " << page_header_size_raw;
//        google::FlushLogFiles(google::INFO);
        auto i = lseek(file_descriptor, offset, SEEK_SET);
        if(i == -1){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot seek file : " << filename << " to " << offset << " error : " << strerror(save_errno);
        }
        Byte* page_buffer = allocatePage(page_size);

        i = read(file_descriptor, page_buffer, page_size);
        if(i == -1 || i != page_size){
            auto save_errno = errno;
            LOG(FATAL) << "Cannot read file : " << filename << " to " << offset << " error : " << strerror(save_errno);
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

void Table::flush()
{
    LOG(INFO) << "Flushing " << main_map.size() << " Number of pages.";

    for (const auto& item : main_map)
    {
        auto offset = item.second->getMetaData().getPageIdentifier() * item.second->getMetaData().getPageSize();
//        printf("offset : %d , page_id : %d, page_size : %d, page_header_size_raw : %d\n", offset, item.second->getMetaData().getPageIdentifier(), item.second->getMetaData().getPageSize(), item.second->getMetaData().page_header_size_raw);
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
    main_map.clear();
    main_map[head_directory->getMetaData().getPageIdentifier()] = head_directory;
    main_map[head_data->getMetaData().getPageIdentifier()] = head_data;
}
