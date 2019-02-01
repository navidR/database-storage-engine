#include <cmath>
#include <iostream>
#include <random>
#include <chrono>


#include "table.hpp"
#include "pageheader.hpp"
#include "gtest/gtest.h"
#include "glog/logging.h"

using std::to_string;
using std::vector;
using std::tie;
using std::cout;
using std::endl;
using namespace std::chrono;


static uint32_t page_sizes[] = {1024, 4096, 16384};
static uint32_t record_sizes[] = {8, 64, 256};
static uint32_t number_of_pages[] = {4096, 131072, 262144, 2097152, 16777216};


const inline static uint32_t page_header_size_raw = sizeof(uint8_t)   // PageType as uint8_t
            + sizeof(uint32_t)  // page_identifier as uint32_t
            + sizeof(uint32_t)  // page_size as uint32_t
            + sizeof(uint32_t)  // record_size as uint32_t
            + sizeof(uint32_t)  // record_count as uint32_t
            + sizeof(uint32_t); // next as uint32_t


class DeleteTest : public ::testing::TestWithParam<uint32_t>
{
    //Random initialisation
public:
    DeleteTest() :
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

TEST_P(DeleteTest, MainTest)
{
    vector<uint64_t> inserted_items;
    uint32_t number_of_records = (page_size - page_header_size_raw) / record_size;
    cout << "***************" << endl
         << " Number of Page : " << number_of_page << endl
         << " Page Size : " << page_size << endl
         << " Record Size : " << record_size << endl
         << " Number of Records in each Page : " << number_of_records << endl << "" << endl;
    Byte* bytes = new Byte[record_size];
    for (uint32_t i = 0; i < record_size; ++i) {
        bytes[i] = i;
    }

    for (uint32_t j = 0; j < number_of_page; ++j) {
        for(uint32_t i = 0; i < number_of_records; ++i)
        {
            inserted_items.push_back(
                        table.InsertWithReturnRID(reinterpret_cast<const char*>(bytes))
                        );
        }
    }

    Byte* read_bytes = new Byte[record_size];

    table.writeToDisk();
    table.clear();

    std::mt19937 random_number_generator;
    random_number_generator.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> random_page_id(1, number_of_page);
    std::uniform_int_distribution<std::mt19937::result_type> random_record_id(0, number_of_records);

    cout << "Data has been inserted to the database and written to disk." << endl;

    high_resolution_clock::time_point start_delete = high_resolution_clock::now();

    for(uint32_t i = 0; i < 500; ++i)
    {
        uint32_t page_id = random_page_id(random_number_generator);
        uint32_t record_id = random_record_id(random_number_generator);
        table.Delete(Page::concatenate(page_id, record_id));
    }

    high_resolution_clock::time_point end_delete = high_resolution_clock::now();
    auto time_span = duration_cast<milliseconds>(end_delete - start_delete);
    cout << "500 randomly choosed record got deleted from database. The delete time for 500 record is : ("
         << time_span.count() << " ms)" << endl;
}


INSTANTIATE_TEST_CASE_P(Instantiation, DeleteTest, testing::Range(uint32_t(0), uint32_t(9)));
