# CMPT 740 Assignment 1: Storage

This program has been written in **C++17** and has been tested in ubuntu 18.04 with clang 7.0.1. 


## Building

For building the project in source directory you have to do : 
```
> mkdir build
> cd build
> cmake  ..  -DCMAKE_BUILD_TYPE=Release
> make
```
After CMake tested your environment you have to see two file named : "cmpt740-1-storage" and "cmpt740-1-storage_with_O_DIRECT".

The file named "cmpt740-1-storage_with_O_DIRECT" is the one which has the **O_DIRECT** flag for open system call and will write to disk directly.

## Flags

You can run the program with `--help` argumant and you have to see this result :

```
aleph@Ghost:~/development/cmpt740-1-storage/build-Clang_Desktop-Default$ ./cmpt740-1-storage --help
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

The main and only flag which is important for the test required for the assignment as been implemented in `-internal_test` flag. When you give this flag to the program it is going to test serialization/deserialization from record level, to page level and at the end to table level. Most of the outputed text can be ignored. The only text which is required by assignment is the **TableTest**. For the Testing itself I am using GoogleTest library to benchmark and test my program execution time. We have implemented the all combination of page and record size. That being said we have implemented **inserts** at least 10 million records (as assignment required) and the scan too (more than even what assignment required.). 


## Bonus

We have implemented `delete` operation on table and page level. The way we do the delete is with simple optimization. We remove the latest record inserted to database and replace the item which has requested to delete with the latest data. So because the deisnged API for assignment does not expose the RID of a record to outside world, this is perfectly safe to do. 


