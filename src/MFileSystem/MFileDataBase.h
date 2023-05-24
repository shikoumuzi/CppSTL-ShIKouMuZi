#ifndef __MUZI__MFILEDATABASE_H__
#define __MUZI__MFILEDATABASE_H__
#include<boost/filesystem.hpp>
#include<boost/filesystem/fstream.hpp>


class MFileDataBase
{
public:
	MFileDataBase();
	MFileDataBase(const MFileDataBase&) = delete;
	MFileDataBase(MFileDataBase&&)noexcept;
};


#endif // !__MUZI__MFILEDATABASE_H__
