#include <cmath>

#include "page.hpp"
#include "datapage.hpp"
#include "directorypage.hpp"
#include "gtest/gtest.h"
#include "glog/logging.h"


static uint32_t page_sizes[] = {1024, 4096, 16384};
static uint32_t record_sizes[] = {8, 64, 256};

class DataPageTest : public ::testing::TestWithParam<uint32_t>
{
    //Random initialisation
public:
    DataPageTest() :
        page(GetParam(),
             page_sizes[GetParam() % 3],
             record_sizes[GetParam() / 3])
    {
#ifndef NDEBUG
        LOG(INFO) << "Page id is : " << page.getMetaData().getPageIdentifier();
        LOG(INFO) << "Page size is : " << page.getMetaData().getPageSize();
        LOG(INFO) << "Record size is : " << page.getMetaData().getRecordSize();
        LOG(INFO) << "Page maximum record count : " << page.getMetaData().getPageMaxRecordCount();
        LOG(INFO) << "Page data size : " << page.getMetaData().getPageDataSize();
        LOG(INFO) << "Page header size : " << page.getMetaData().page_header_size_raw;
#endif
    }

    DataPage page;
};

TEST(DataPageTest, ConcatenateDissociate)
{
    for(uint32_t i = 7351; i < 7369; ++i)
    {
        for(uint32_t j = 124; j < 138; ++j)
        {
            pair<uint32_t, uint32_t> id = Page::dissociate(Page::concatenate(i, j));
            EXPECT_EQ(id.first, i);
            EXPECT_EQ(id.second, j);
        }
    }
}

TEST_P(DataPageTest, SerializeDeserialize)
{
    Byte *ptr = new Byte[page.getMetaData().getPageSize()];
    page.getMetaData().serialize(ptr);
    DataPage result(ptr);
    EXPECT_EQ(page.getMetaData().getPageType(), result.getMetaData().getPageType());
    EXPECT_EQ(page.getMetaData().getPageIdentifier(), result.getMetaData().getPageIdentifier());
    EXPECT_EQ(page.getMetaData().getPageSize(), result.getMetaData().getPageSize());
    EXPECT_EQ(page.getMetaData().getRecordSize(), result.getMetaData().getRecordSize());
    EXPECT_EQ(page.getMetaData().getRecordCount(), result.getMetaData().getRecordCount());
    EXPECT_EQ(page.getMetaData().getNext(), result.getMetaData().getNext());

    uint8_t *temporary_array = new uint8_t[page.getMetaData().getRecordSize()];
    for(uint32_t j = 0; j < page.getMetaData().getRecordSize(); ++j)
        temporary_array[j] = j;

    for (uint32_t i = 0; i < page.getMetaData().getPageMaxRecordCount(); ++i) {
        bool success = page.Insert(reinterpret_cast<char *>(temporary_array));
        if(!success)
            LOG(FATAL) << "Cannot insert to page : " << i;
    }

    for (uint32_t i = 0; i < page.getMetaData().getPageMaxRecordCount(); ++i) {
        const uint8_t* read = reinterpret_cast<const uint8_t*>(page.Read(Page::concatenate(page.getMetaData().getPageIdentifier(), i)));
        EXPECT_EQ(*read, *temporary_array);
    }

    delete[] temporary_array;
}


INSTANTIATE_TEST_CASE_P(Instantiation, DataPageTest, testing::Range(uint32_t(0), uint32_t(9)));


