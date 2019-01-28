#include <iostream>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <gflags/gflags.h>

using namespace std;

/* Command line flags
 * Page size (--page_size)
 * Valid values:
 * 1024, 4096, 16384
 **/
static bool validate_page_size(const char* flagname, uint32_t page_size) {
   if (page_size == 1024 ||
           page_size == 4096 ||
           page_size == 16384)
     return true;
   LOG(FATAL) << "Invalid value for --" <<  flagname << " : "<<  page_size ;
   return false;
}

DEFINE_uint32(page_size, 1024, "Page size");
DEFINE_validator(page_size, &validate_page_size);


/* Command line flags
 * Record size (--record_size)
 * Valid values:
 * 8, 64, 256
 **/
static bool validate_record_size(const char* flagname, uint32_t record_size) {
   if (record_size == 8 ||
           record_size == 64 ||
           record_size == 256)
     return true;
   LOG(FATAL) << "Invalid value for --" <<  flagname << " : "<<  record_size ;
   return false;
}

DEFINE_uint32(record_size, 8, "Record size");
DEFINE_validator(record_size, &validate_record_size);

/* Command line flags
 * Insert size (--Insert_size)
 * Valid values:
 * NULL, < 1000000
 **/
static bool validate_insert_size(const char* flagname, uint32_t insert_size) {
   if (insert_size == NULL ||
           insert_size >= 10000000)
     return true;
   LOG(FATAL) << "Invalid value for --" <<  flagname << " : "<<  insert_size ;
   return false;
}

DEFINE_uint32(insert_size, NULL, "Insert size");
DEFINE_validator(insert_size, &validate_insert_size);

/* Command line flags
 * Scan length (--scan_length)
 * Valid values:
 * 10, 100, 1000
 **/
static bool validate_scan_length(const char* flagname, uint32_t scan_length) {
   if (scan_length == NULL ||
           scan_length == 10 ||
           scan_length == 100 ||
           scan_length == 1000)
     return true;
   LOG(FATAL) << "Invalid value for --" <<  flagname << " : "<<  scan_length ;
   return false;
}

DEFINE_uint32(scan_length, NULL, "Scan length");
DEFINE_validator(scan_length, &validate_scan_length);

/* Command line flags
 * Record ID (--rid)
 * Valid values:
 * uint32_t
 **/

DEFINE_uint32(rid, NULL, "Record Identifier");


int main(int argc, char **argv)
{
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

#ifndef NDEBUG
    FLAGS_alsologtostderr = 1;
#endif

  LOG(INFO) << "Page Size : " << FLAGS_page_size;
  LOG(INFO) << "Record Size : " << FLAGS_record_size;
  LOG(INFO) << "Insert Size : " << FLAGS_insert_size;
  LOG(INFO) << "Scan Length: " << FLAGS_scan_length;
  LOG(INFO) << "Record Identifier : " << FLAGS_rid;




  return 0;
}
