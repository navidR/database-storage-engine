#include <cmath>

#include "pageheader.hpp"
#include "gtest/gtest.h"
#include "glog/logging.h"

uint32_t page_sizes[] = {1024, 4096, 16384};
uint32_t record_sizes[] = {8, 64, 256};

class PageHeaderTest : public ::testing::TestWithParam<uint32_t> {
    //Random initialisation
public:
    PageHeaderTest() :
        page_header(PageType::DATA,
                    GetParam(),
                    page_sizes[GetParam() % 3],
                    record_sizes[GetParam() / 3])
    {
        LOG(INFO) << "Page id is : " << page_header.getPageIdentifier();
        LOG(INFO) << "Page size is : " << page_header.getPageSize();
        LOG(INFO) << "Record size is : " << page_header.getRecordSize();
    }

    PageHeader page_header;
};

TEST_P(PageHeaderTest, SerializeDeserialize) {
    Byte* ptr = (Byte*) malloc(page_header.getPageSize());
    page_header.serialize(ptr);
    PageHeader result(ptr);
    EXPECT_EQ(page_header.getPageType(), result.getPageType());
    EXPECT_EQ(page_header.getPageIdentifier(), result.getPageIdentifier());
    EXPECT_EQ(page_header.getPageSize(), result.getPageSize());
    EXPECT_EQ(page_header.getRecordSize(), result.getRecordSize());
    EXPECT_EQ(page_header.getRecordCount(), result.getRecordCount());
    EXPECT_EQ(page_header.getNext(), result.getNext());
    delete ptr;
}


INSTANTIATE_TEST_CASE_P(Instantiation, PageHeaderTest, testing::Range(uint32_t(0), uint32_t(9)));


