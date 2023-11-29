#pragma once
#ifndef __MUZI_MSTRING_H__
#define __MUZI_MSTRING_H__
namespace MUZI
{
	class MString
	{
	public:

	public:
		MString();
		MString(const MString&);
		MString(MString&&);
		~MString();
	public:
		//bool setAllocation(MAllocator*);

	private:
		class MStringData* p_data;
	};




};
#endif // !__MUZI_MSTRING_H__

