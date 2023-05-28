#include"MFile.h"

namespace MUZI
{
	struct MFile::__MFile_Data__
	{

	};

	MFile::MFile(){return; }
	MFile::MFile(const MFile&){return; }
	MFile::MFile(MFile&&){return; }
	MFile::~MFile(){return; }

	// 文件打开
	int MFile::open(char* file_name, char* flag, char* encoding){return 0; }
	int MFile::close(){return 0; }

	int MFile::read(){return 0; }
	int MFile::readline(){return 0; }
	int MFile::readlines(){return 0; }

	int MFile::read_no_lock(){return 0; }
	int MFile::readline_no_lock(){return 0; }
	int MFile::readlines_no_lock(){return 0; }

	int MFile::write(){return 0; }
	int MFile::writeline(){return 0; }
	int MFile::writelines(){return 0; }

	int MFile::write_no_lock(){return 0; }
	int MFile::writeline_no_lock(){return 0; }
	int MFile::writelines_no_lock(){return 0; }

	int MFile::seek(){return 0; }

	int MFile::flock(){return 0; }

	int MFile::flush(){return 0; }

	int MFile::is_EOF(){return 0; }

}