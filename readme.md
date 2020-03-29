# Database Storage Engine

This program has been written in **C++17** and has been tested in ubuntu 18.04 with clang 7.0.1.


## Building

For building the project in source directory you have to do :
```
> mkdir build
> cd build
> cmake .. -DCMAKE_BUILD_TYPE=Release
> make
```
After CMake tested your environment you have to see two file named : "storage-engine" and "storage-engine_with_O_DIRECT".

The file named "storage_with_O_DIRECT" is the one which has the **O_DIRECT** flag for open system call and will write to disk directly. Both of the executables do run **the same tests**. But the one with O_DIRECT in its name uses **O_DIRECT** and does write to disk directly.

## Flags

You can run the program with `--help` argumant and you have to see this result :

```
~/development/cmpt740-1-storage/build-Clang_Desktop-Default$ ./cmpt740-1-storage --help
cmpt740-1-storage: Warning: SetUsageMessage() never called

  Flags from ../src/main.cpp:
    -insert_size (Insert size) type: uint32 default: 0
    -internal_test (Run internal tests) type: bool default: false
    -page_size (Page size, valid sizes are : 1024, 4096, 16384) type: uint32
      default: 1024
    -record_size (Record size, valid sizes are : 8, 64, 256) type: uint32
      default: 8
    -rid (Record Identifier) type: uint32 default: 0
    -scan_length (Scan length, valid sizes are : 10, 100, 1000) type: uint32
      default: 0
```

The extra stuff there is not important and can be ignored. They are help flags from gflags library I am using to handle command line flags. The other flags are for reading database.

## Testing

The main and only flag which is important for the test required for the assignment as been implemented in `-internal_test` flag. When you give this flag to the program it is going to test serialization/deserialization from record level, to page level and at the end to table level. Most of the outputed text can be ignored. Please ignore the tests named `DataPageTest`, `DirectoryPageTest` and `PageHeaderTest`and The only text which is required by assignment is the **TableTest**. For the Testing itself I am using GoogleTest library to benchmark and test my program execution time. We have implemented the all combination of page and record size. That being said we have implemented **inserts** at least 10 million records (as assignment required) and the scan too (more than even what assignment required.).

For example the output for the program is going to be something like this :

```
***************
 Number of Page : 4096
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (28 ms)
Scan for all the data in the database took : (17 ms)
[       OK ] Instantiation/TableTest.MainTest/0 (52 ms)
[ RUN      ] Instantiation/TableTest.MainTest/1
***************
 Number of Page : 131072
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (868 ms)
Scan for all the data in the database took : (579 ms)

```
After the line `***************` we are running the test till the next line with `***************`. Each test is doing two combination of the operations required by assignment. First operation is inserting as much as data required by to fill that database (more than stated amount in assignment "at least 10 million records"). In the second part of the test we are doing a scan as total of the number.

In those lines you can see the configuration of the test we are running. For exmaple in the output I've copied here you can see there is two test has been run. one with 4096 number of pages and page size of 1024 and record size of 8. The other one is with 131072 number of pages and page size of 1024 and record size of 8. As you can see after that line the number inside the **parantesis** is total the running time for that specific test.

In each step I am printing the amount of time it takes to do that step in the parantesis in miliseconds. For example :

```
Data has been inserted to the database and written to disk. Insertation and committing to disk took : (868 ms)
Scan for all the data in the database took : (579 ms)
```

It has take `868 ms` for the storage manager to insert and commit data to the disk. And scan for all the data has taken `579 ms`. So the write being slower in SSD devices is clearly visiable here.

Becuase at the end of each test Table manager will try to write touched page into the disk again the value in the *last* (total time for the test) parantesis will be bigger the sum of the two test.

## Bonus

We have implemented `delete` operation on table and page level. The way we do the delete is with simple optimization. We remove the latest record inserted to database and replace the item which has requested to delete with the latest data. So because the deisnged API for assignment does not expose the RID of a record to outside world, this is perfectly safe to do.

### Delete Operation Performance Test
Because of smart way of implementing the delete operation in our storage manager, delete operation is basically `O(log(n) + disk_access)` for our storage manager. In the `DeleteTest` we do create tables with different configuration each time. Then we delete 5 record which randomly has been chose from our database. The time reported is for the time required to report `500` element. We have to mention because we are running the tests on a high-end NVMe storage with fast random access time, `disk_access` is trivial most of the time.

If you want to remove more or less number of records from the database you can change the number `500` here (it will automatically generate random `RID` to remove records and will report the time spent on it.
```
# In the file DeleteTest line 80 :
  for(uint32_t  i  =  0;  i  <  500;  ++i)
```

**IMPORTANT NOTE** The reason why delete operation in `O_DIRECT` is faster is because glibc tries to load more data (and cache them) when you don't have `O_DIRECT` flag.

## Configuration
The test has been done in a system with this configuration:
```
Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz
32GB DDR4 RAM
1TB PCI-Express (NVMe) SSD
```

# Sample Output

## Sample Output for the one Without `O_DIRECT`

```
[==========] Running 47 tests from 7 test cases.
[----------] Global test environment set-up.
[----------] 1 test from DataPageTest
[ RUN      ] DataPageTest.ConcatenateDissociate
[       OK ] DataPageTest.ConcatenateDissociate (0 ms)
[----------] 1 test from DataPageTest (0 ms total)

[----------] 1 test from DirectoryPageTest
[ RUN      ] DirectoryPageTest.ConcatenateDissociate
[       OK ] DirectoryPageTest.ConcatenateDissociate (0 ms)
[----------] 1 test from DirectoryPageTest (0 ms total)

[----------] 9 tests from Instantiation/PageHeaderTest
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/0
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/0 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/1
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/1 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/2
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/2 (1 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/3
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/3 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/4
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/4 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/5
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/5 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/6
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/6 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/7
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/7 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/8
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/8 (0 ms)
[----------] 9 tests from Instantiation/PageHeaderTest (1 ms total)

[----------] 9 tests from Instantiation/DataPageTest
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/0
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/0 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/1
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/1 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/2
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/2 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/3
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/3 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/4
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/4 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/5
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/5 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/6
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/6 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/7
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/7 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/8
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/8 (0 ms)
[----------] 9 tests from Instantiation/DataPageTest (0 ms total)

[----------] 9 tests from Instantiation/DirectoryPageTest
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/0
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/0 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/1
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/1 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/2
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/2 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/3
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/3 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/4
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/4 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/5
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/5 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/6
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/6 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/7
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/7 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/8
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/8 (0 ms)
[----------] 9 tests from Instantiation/DirectoryPageTest (0 ms total)

[----------] 9 tests from Instantiation/TableTest
[ RUN      ] Instantiation/TableTest.MainTest/0
***************
 Number of Page : 4096
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (28 ms)
Scan for all the data in the database took : (17 ms)
[       OK ] Instantiation/TableTest.MainTest/0 (52 ms)
[ RUN      ] Instantiation/TableTest.MainTest/1
***************
 Number of Page : 131072
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (868 ms)
Scan for all the data in the database took : (579 ms)
[       OK ] Instantiation/TableTest.MainTest/1 (1619 ms)
[ RUN      ] Instantiation/TableTest.MainTest/2
***************
 Number of Page : 262144
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (1722 ms)
Scan for all the data in the database took : (1063 ms)
[       OK ] Instantiation/TableTest.MainTest/2 (3125 ms)
[ RUN      ] Instantiation/TableTest.MainTest/3
***************
 Number of Page : 4096
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (22 ms)
Scan for all the data in the database took : (12 ms)
[       OK ] Instantiation/TableTest.MainTest/3 (43 ms)
[ RUN      ] Instantiation/TableTest.MainTest/4
***************
 Number of Page : 131072
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (841 ms)
Scan for all the data in the database took : (396 ms)
[       OK ] Instantiation/TableTest.MainTest/4 (1482 ms)
[ RUN      ] Instantiation/TableTest.MainTest/5
***************
 Number of Page : 262144
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (1693 ms)
Scan for all the data in the database took : (777 ms)
[       OK ] Instantiation/TableTest.MainTest/5 (2954 ms)
[ RUN      ] Instantiation/TableTest.MainTest/6
***************
 Number of Page : 4096
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (48 ms)
Scan for all the data in the database took : (21 ms)
[       OK ] Instantiation/TableTest.MainTest/6 (89 ms)
[ RUN      ] Instantiation/TableTest.MainTest/7
***************
 Number of Page : 131072
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (2039 ms)
Scan for all the data in the database took : (677 ms)
[       OK ] Instantiation/TableTest.MainTest/7 (3371 ms)
[ RUN      ] Instantiation/TableTest.MainTest/8
***************
 Number of Page : 262144
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (7492 ms)
Scan for all the data in the database took : (1319 ms)
[       OK ] Instantiation/TableTest.MainTest/8 (11243 ms)
[----------] 9 tests from Instantiation/TableTest (23978 ms total)

[----------] 9 tests from Instantiation/DeleteTest
[ RUN      ] Instantiation/DeleteTest.MainTest/0
***************
 Number of Page : 4096
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (58 ms)
[       OK ] Instantiation/DeleteTest.MainTest/0 (118 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/1
***************
 Number of Page : 131072
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (81 ms)
[       OK ] Instantiation/DeleteTest.MainTest/1 (926 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/2
***************
 Number of Page : 262144
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (18 ms)
[       OK ] Instantiation/DeleteTest.MainTest/2 (1720 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/3
***************
 Number of Page : 4096
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (19 ms)
[       OK ] Instantiation/DeleteTest.MainTest/3 (42 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/4
***************
 Number of Page : 131072
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (79 ms)
[       OK ] Instantiation/DeleteTest.MainTest/4 (947 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/5
***************
 Number of Page : 262144
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (18 ms)
[       OK ] Instantiation/DeleteTest.MainTest/5 (1754 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/6
***************
 Number of Page : 4096
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (19 ms)
[       OK ] Instantiation/DeleteTest.MainTest/6 (70 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/7
***************
 Number of Page : 131072
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (61 ms)
[       OK ] Instantiation/DeleteTest.MainTest/7 (2206 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/8
***************
 Number of Page : 262144
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (74 ms)
[       OK ] Instantiation/DeleteTest.MainTest/8 (7639 ms)
[----------] 9 tests from Instantiation/DeleteTest (15422 ms total)

[----------] Global test environment tear-down
[==========] 47 tests from 7 test cases ran. (39401 ms total)
[  PASSED  ] 47 tests.
```

## Sample Output for the one with `O_DIRECT` flag

```
[==========] Running 47 tests from 7 test cases.
[----------] Global test environment set-up.
[----------] 1 test from DataPageTest
[ RUN      ] DataPageTest.ConcatenateDissociate
[       OK ] DataPageTest.ConcatenateDissociate (0 ms)
[----------] 1 test from DataPageTest (0 ms total)

[----------] 1 test from DirectoryPageTest
[ RUN      ] DirectoryPageTest.ConcatenateDissociate
[       OK ] DirectoryPageTest.ConcatenateDissociate (0 ms)
[----------] 1 test from DirectoryPageTest (0 ms total)

[----------] 9 tests from Instantiation/PageHeaderTest
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/0
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/0 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/1
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/1 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/2
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/2 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/3
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/3 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/4
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/4 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/5
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/5 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/6
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/6 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/7
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/7 (0 ms)
[ RUN      ] Instantiation/PageHeaderTest.SerializeDeserialize/8
[       OK ] Instantiation/PageHeaderTest.SerializeDeserialize/8 (0 ms)
[----------] 9 tests from Instantiation/PageHeaderTest (0 ms total)

[----------] 9 tests from Instantiation/DataPageTest
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/0
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/0 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/1
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/1 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/2
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/2 (1 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/3
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/3 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/4
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/4 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/5
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/5 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/6
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/6 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/7
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/7 (0 ms)
[ RUN      ] Instantiation/DataPageTest.SerializeDeserialize/8
[       OK ] Instantiation/DataPageTest.SerializeDeserialize/8 (0 ms)
[----------] 9 tests from Instantiation/DataPageTest (1 ms total)

[----------] 9 tests from Instantiation/DirectoryPageTest
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/0
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/0 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/1
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/1 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/2
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/2 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/3
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/3 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/4
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/4 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/5
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/5 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/6
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/6 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/7
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/7 (0 ms)
[ RUN      ] Instantiation/DirectoryPageTest.SerializeDeserialize/8
[       OK ] Instantiation/DirectoryPageTest.SerializeDeserialize/8 (0 ms)
[----------] 9 tests from Instantiation/DirectoryPageTest (0 ms total)

[----------] 9 tests from Instantiation/TableTest
[ RUN      ] Instantiation/TableTest.MainTest/0
***************
 Number of Page : 4096
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (700 ms)
Scan for all the data in the database took : (731 ms)
[       OK ] Instantiation/TableTest.MainTest/0 (1797 ms)
[ RUN      ] Instantiation/TableTest.MainTest/1
***************
 Number of Page : 131072
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (22356 ms)
Scan for all the data in the database took : (22715 ms)
[       OK ] Instantiation/TableTest.MainTest/1 (57267 ms)
[ RUN      ] Instantiation/TableTest.MainTest/2
***************
 Number of Page : 262144
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (44957 ms)
Scan for all the data in the database took : (42887 ms)
[       OK ] Instantiation/TableTest.MainTest/2 (112203 ms)
[ RUN      ] Instantiation/TableTest.MainTest/3
***************
 Number of Page : 4096
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (716 ms)
Scan for all the data in the database took : (214 ms)
[       OK ] Instantiation/TableTest.MainTest/3 (1354 ms)
[ RUN      ] Instantiation/TableTest.MainTest/4
***************
 Number of Page : 131072
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (23805 ms)
Scan for all the data in the database took : (6595 ms)
[       OK ] Instantiation/TableTest.MainTest/4 (43593 ms)
[ RUN      ] Instantiation/TableTest.MainTest/5
***************
 Number of Page : 262144
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (47249 ms)
Scan for all the data in the database took : (13468 ms)
[       OK ] Instantiation/TableTest.MainTest/5 (87485 ms)
[ RUN      ] Instantiation/TableTest.MainTest/6
***************
 Number of Page : 4096
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (880 ms)
Scan for all the data in the database took : (495 ms)
[       OK ] Instantiation/TableTest.MainTest/6 (1866 ms)
[ RUN      ] Instantiation/TableTest.MainTest/7
***************
 Number of Page : 131072
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (28393 ms)
Scan for all the data in the database took : (15777 ms)
[       OK ] Instantiation/TableTest.MainTest/7 (59942 ms)
[ RUN      ] Instantiation/TableTest.MainTest/8
***************
 Number of Page : 262144
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk. Insertation and committing to disk took : (56154 ms)
Scan for all the data in the database took : (31708 ms)
[       OK ] Instantiation/TableTest.MainTest/8 (119090 ms)
[----------] 9 tests from Instantiation/TableTest (484598 ms total)

[----------] 9 tests from Instantiation/DeleteTest
[ RUN      ] Instantiation/DeleteTest.MainTest/0
***************
 Number of Page : 4096
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (18 ms)
[       OK ] Instantiation/DeleteTest.MainTest/0 (817 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/1
***************
 Number of Page : 131072
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (18 ms)
[       OK ] Instantiation/DeleteTest.MainTest/1 (24952 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/2
***************
 Number of Page : 262144
 Page Size : 1024
 Record Size : 8
 Number of Records in each Page : 125

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (18 ms)
[       OK ] Instantiation/DeleteTest.MainTest/2 (49628 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/3
***************
 Number of Page : 4096
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (18 ms)
[       OK ] Instantiation/DeleteTest.MainTest/3 (905 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/4
***************
 Number of Page : 131072
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (18 ms)
[       OK ] Instantiation/DeleteTest.MainTest/4 (27234 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/5
***************
 Number of Page : 262144
 Page Size : 4096
 Record Size : 64
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (19 ms)
[       OK ] Instantiation/DeleteTest.MainTest/5 (54344 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/6
***************
 Number of Page : 4096
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (18 ms)
[       OK ] Instantiation/DeleteTest.MainTest/6 (1014 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/7
***************
 Number of Page : 131072
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (18 ms)
[       OK ] Instantiation/DeleteTest.MainTest/7 (32236 ms)
[ RUN      ] Instantiation/DeleteTest.MainTest/8
***************
 Number of Page : 262144
 Page Size : 16384
 Record Size : 256
 Number of Records in each Page : 63

Data has been inserted to the database and written to disk.
500 randomly choosed record got deleted from database. The delete time for 500 record is : (19 ms)
[       OK ] Instantiation/DeleteTest.MainTest/8 (64333 ms)
[----------] 9 tests from Instantiation/DeleteTest (255463 ms total)

[----------] Global test environment tear-down
[==========] 47 tests from 7 test cases ran. (740062 ms total)
[  PASSED  ] 47 tests.

```
