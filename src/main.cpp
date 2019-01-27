#include <iostream>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <tsl/hopscotch_map.h>

using namespace std;


int main(int argc, char **argv)
{
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Glog";

  tsl::hopscotch_map<int, int> map = {{1, 1}, {2, 1}, {3, 1}};
  for(auto it = map.begin(); it != map.end(); ++it) {
      //it->second = 2; // Illegal
      it.value() = 2; // Ok
  }



  return 0;
}
