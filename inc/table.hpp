#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <glog/logging.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <tsl/robin_map.h>
#include <unistd.h>
#include <sys/stat.h>
#include <algorithm>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <set>

#include "page.hpp"
#include "datapage.hpp"
#include "directorypage.hpp"

using std::string;
using std::pair;
using std::make_pair;
using std::tie;
using tsl::robin_map;
using std::for_each;
using std::vector;
using std::sort;
using std::set;

#ifndef BLKPBSZGET
#define BLKSSZGET  _IO(0x12,104) /* get block device sector size */
#endif

// read(2), write(2), lseek(2), fcntl(2) Operation will be used
class Table
{
public:
    Table(const char *);
    Table(const char *,  // Table name
          uint32_t,      // Page size
          uint32_t);     // Record size
    ~Table();

    /* *IMPORTANT NOTE*
     * Small API change :
     * Instead of having universal functions, just work with Table object.
     * e.g.
     * Just create Table object instead of CreateTable function.
     * (if you really need file descriptor outside of this function. You can call get_file_descriptor function.
     * Just destroy the Table object instead of CloseTable function.
     */
//    int CreateTable(const char *filename);
//    void CloseTable(int fd);

    int get_file_descriptor();
    bool Insert(const char *);
    uint64_t InsertWithReturnRID(const char *);
    bool Read(uint64_t RID, char *buf);
    void flush();
    void Delete(uint32_t);

    static pair<uint64_t, uint64_t> getTotalSystemMemory();
    uint32_t nextIdentifier();
    static void get_block_sector_size(int, int*);
    static Byte* allocatePage(uint32_t);

private:
    string filename;
    int file_descriptor;
    uint32_t page_size;
    uint32_t record_size;
    uint32_t page_header_size_raw;

    DirectoryPage* head_directory;
    DataPage* head_data;

    uint64_t system_avail_mem, system_page_size;
    robin_map<uint32_t, Page*> main_map;

//    uint32_t number_of_datapages;
//    uint32_t number_of_directorypages;

    uint32_t identifiers = 0;
};

#endif // TABLE_H
