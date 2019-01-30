#include <cmath>

#include "table.hpp"
#include "pageheader.hpp"
#include "gtest/gtest.h"
#include "glog/logging.h"

using std::to_string;
using std::vector;
using std::tie;

static uint32_t page_sizes[] = {16384, 4096, 1024};
static uint32_t record_sizes[] = {8, 64, 256};
static uint32_t number_of_pages[] = {4096, 131072, 262144, 2097152};


const inline static uint32_t page_header_size_raw = sizeof(uint8_t)   // PageType as uint8_t
            + sizeof(uint32_t)  // page_identifier as uint32_t
            + sizeof(uint32_t)  // page_size as uint32_t
            + sizeof(uint32_t)  // record_size as uint32_t
            + sizeof(uint32_t)  // record_count as uint32_t
            + sizeof(uint32_t); // next as uint32_t


class TableTest : public ::testing::TestWithParam<uint32_t>
{
    //Random initialisation
public:
    TableTest() :
        page_size(page_sizes[GetParam() / 3]),
        record_size(record_sizes[GetParam() / 3]),
        number_of_page(number_of_pages[GetParam() % 3]),
        table((to_string(GetParam()) + ".db").c_str(),
              page_size,
              record_size)
    {

    }
    const uint32_t page_size;
    const uint32_t record_size;
    const uint32_t number_of_page;
    Table table;
};

TEST_P(TableTest, MainTest)
{
    vector<uint64_t> inserted_items;
    uint32_t num_of_records = (page_size - page_header_size_raw) / record_size;
    LOG(INFO) << "Running TableTest for " << number_of_page << " number of pages and page size of " << page_size << " and record size of " << record_size << " and number of records in each page is " << num_of_records;
    Byte* bytes = new Byte[record_size];
    for (uint32_t i = 0; i < record_size; ++i) {
        bytes[i] = i;
    }

    for (uint32_t j = 0; j < number_of_page; ++j) {
        for(uint32_t i = 0; i < num_of_records; ++i)
        {
            inserted_items.push_back(
                        table.InsertWithReturnRID(reinterpret_cast<const char*>(bytes))
                        );
        }
    }

    Byte* read_bytes = new Byte[record_size];

    table.flush();

    for_each(inserted_items.begin(), inserted_items.end(), [&](const uint64_t rid){
        uint32_t page_id, record_id;
        tie(page_id, record_id) = Page::dissociate(rid);
        memset(read_bytes, 'a', record_size);
        if(!table.Read(Page::concatenate(page_id, record_id), reinterpret_cast<char *>(read_bytes)))
        {
            LOG(FATAL) << "Cannot read from table at page_id : " << page_id << ", record_id : " << record_id;
        }
        for (uint32_t j = 0; j < record_size; ++j) {
            EXPECT_EQ(read_bytes[j], bytes[j]);
        }
    });
}


INSTANTIATE_TEST_CASE_P(Instantiation, TableTest, testing::Range(uint32_t(0), uint32_t(9)));
