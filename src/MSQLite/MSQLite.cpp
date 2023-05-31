#include"MSQLiite.h"
#include"MRBTree.h"
#include<string>
namespace MUZI
{
	struct MSQLite::__SQL_MESSAGE__
	{
		MSQLite::sql_id_t sql_id;
		MSQLite::sql_type_t sql_type;
		sqlite3_stmt* data;
	};
	struct MSQLite::__MSQLite_Data__
	{
		MRBTree<MSQLite::__SQL_MESSAGE__>* sql_tree;
		MRBTree<std::string>* table_tree;
	};


	MSQLite::MSQLite(const char* sqlite_dir_path = "./sqlite")
	{

	}
	MSQLite::MSQLite(MSQLite&&)
	{

	}
	MSQLite::~MSQLite()
	{

	}


}

