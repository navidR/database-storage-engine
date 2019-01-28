#ifndef TABLE_H
#define TABLE_H

#include <string>

using std::string;

class Table
{
public:
    Table();

    int CreateTable(const char *filename);
    void CloseTable(int fd);
    bool Insert(const char *record);
    bool Read(int RID, char *buf);

private:
    string filename;
    int file_descriptor;
};

#endif // TABLE_H
