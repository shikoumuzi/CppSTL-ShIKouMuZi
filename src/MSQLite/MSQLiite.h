#ifndef __MUZI_MSQLITE_H__
#define __MUZI_MSQLITE_H__
#include<sqlite3.h>
namespace MUZI
{
	class MSQLite
	{
	public:
		MSQLite();
		MSQLite(const MSQLite&) = delete;
		MSQLite(MSQLite&&);
	public:
		int setMode();

	public:
		int select();
		int earse();
		int insert();
		int change();
	private:
		
	};
}



#endif // !__MUZI_MSQLITE_H__
