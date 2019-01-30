#include <cmath>

#include "table.hpp"
#include "gtest/gtest.h"
#include "glog/logging.h"

using std::to_string;

static uint32_t page_sizes[] = {1024, 4096, 16384};
static uint32_t record_sizes[] = {8, 64, 256};

class TableTest : public ::testing::TestWithParam<uint32_t>
{
    //Random initialisation
public:
    TableTest() :
        table((to_string(GetParam()) + ".db").c_str(),
              page_sizes[GetParam() % 3],
              record_sizes[GetParam() / 3]),
        record_size(record_sizes[GetParam() / 3])
    {

    }
    const uint32_t record_size;
    Table table;
};

TEST_P(TableTest, MainTest)
{
    uint32_t num_of_tries = 124;
    Byte* bytes = new Byte[record_size];
    for (uint32_t j = 0; j < record_size; ++j) {
        bytes[j] = j;
    }

    for(uint32_t i = 0; i < num_of_tries; ++i)
    {
        table.Insert(reinterpret_cast<const char*>(bytes));
    }
    Byte* read_bytes = new Byte[record_size];

    for(uint32_t i = 0; i < num_of_tries; ++i)
    {
        memset(read_bytes, 'a', record_size);
        if(!table.Read(Page::concatenate(1, i), reinterpret_cast<char *>(read_bytes)))
        {
            LOG(FATAL) << "Cannot read from table at : " << i;
        }
        for (uint32_t j = 0; j < record_size; ++j) {
            EXPECT_EQ(read_bytes[j], bytes[j]);
        }
    }
}


INSTANTIATE_TEST_CASE_P(Instantiation, TableTest, testing::Range(uint32_t(0), uint32_t(1)));
