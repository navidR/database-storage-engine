#include "table.hpp"

Table::Table(const char *filename) :
    filename(filename)
{
    file_descriptor = open(filename, O_DIRECT);
    if(file_descriptor == -1)
    {
        int save_errno = errno;
        LOG(FATAL) << "Cannot open file : " << filename << ", error : " << strerror(save_errno);
    }
}

Table::~Table()
{
    close(file_descriptor);
    LOG(INFO) << "Closing the table " << filename;
}

int Table::get_file_descriptor()
{
    return file_descriptor;
}

bool Table::Insert(const char *record)
{

}

bool Table::Read(int RID, char *buf)
{

}
