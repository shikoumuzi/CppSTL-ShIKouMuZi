#include "MSQLiite.h"


namespace MUZI
{
    MSQLite::MSelectResult::MSelectResult(char* stream, size_t size)
    {
        this->bin_data_stream = new char[size];
        this->size = size;
        this->bin_data_stream_index = this->bin_data_stream;
    }

    MSQLite::MSelectResult::~MSelectResult()
    {
        delete this->bin_data_stream;
        this->bin_data_stream = nullptr;
        this->bin_data_stream_index = nullptr;
    }
    int32_t MSQLite::MSelectResult::getINT()
    {
        this->bin_data_stream_index += sizeof(int32_t);
        return 0;
    }
    int64_t MSQLite::MSelectResult::getINT64()
    {
        this->bin_data_stream_index += sizeof(int64_t);
        return 0;
    }
    double MUZI::MSQLite::MSelectResult::getDOUBLE()
    {
        this->bin_data_stream_index += sizeof(double);
        return 0.0;
    }

    char* MUZI::MSQLite::MSelectResult::getTEXT()
    {
        int size = strlen(this->bin_data_stream_index);

        return nullptr;
    }

    char MUZI::MSQLite::MSelectResult::getTEXT16()
    {
        int size = strlen(this->bin_data_stream_index);
        return 0;
    }


}