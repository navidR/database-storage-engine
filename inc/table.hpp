#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <glog/logging.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using std::string;

class Table
{
public:
    Table(const char *filename);
    ~Table();

    /* IMPORTANT
     * Small API change :
     * Instead of having universal functions, just work with Table object.
     * e.g.
     * Just create Table object instead of CreateTable function.
     * (if you really need file descriptor outside of this function. You can call get_file_descriptor function.
     * Just destroy the Table object instead of CloseTable function.
     */
//    int CreateTable(const char *filename);
//    void CloseTable(int fd);

    int get_file_descriptor();
    bool Insert(const char *record);
    bool Read(int RID, char *buf);

private:
    string filename;
    int file_descriptor;
};

#endif // TABLE_H
