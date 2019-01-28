
#include "pageheader.hpp"
#include "gtest/gtest.h"

TEST(PageHeader, Serialize) {
    auto page_header = PageHeader(PageType::DATA,
               0,
               512,
               32   );


    EXPECT_STREQ(expectVal, actualValTrue);
}

TEST(PageHeader, Deserialize) {
    EXPECT_STREQ(expectVal, actualValFalse);
}




