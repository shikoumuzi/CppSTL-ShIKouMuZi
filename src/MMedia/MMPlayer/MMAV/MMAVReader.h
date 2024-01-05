#pragma once
#ifndef __MUZI_MMAVREADER_H__
#define __MUZI_MMAVREADER_H__

#include"MMAVStream.h"
#include"MMAVBase.h"
#include<vector>
#include<iterator>
#include<memory>
#include<compare>
extern "C"
{
}
#include"MMAVPackage.h"

namespace MUZI::ffmpeg
{
	class MMAVReader
	{
		MMAV_FRIEND_CLASS;
		friend class MStreamIterator;
	public:
		using Path = boost::filesystem::path;
	public:
		class MStreamIterator : std::iterator<std::random_access_iterator_tag, MMAVStream, MMAVStream*, MMAVStream&>
		{
		public:
			MMAV_FRIEND_CLASS;
		public:
			MStreamIterator();
			MStreamIterator(const MStreamIterator& iterator);
			MStreamIterator(MStreamIterator&& iterator);
			~MStreamIterator();
		public:
			MStreamIterator(int stream_id, MMAVReader* parent);
		public:
			MStreamIterator operator+(size_t offset);
			void operator+=(size_t offset);
			MStreamIterator operator-(size_t offset);
			void operator-=(size_t offset);
			MMAVStream& operator->();
			MMAVStream& operator*();
			void operator=(const MStreamIterator& iterator);
			bool operator==(const MStreamIterator& iterator);
			std::strong_ordering operator<=>(const MStreamIterator& iterator);
		public:
			const MMAVStream& operator->() const;
			const MMAVStream& operator*() const;
			const MStreamIterator operator-(size_t offset) const;
			const MStreamIterator operator+(size_t offset) const;
			bool operator==(const MStreamIterator& iterator) const;
			std::strong_ordering operator<=>(const MStreamIterator& iterator) const;
		private:
			std::unique_ptr<MMAVStream> m_stream;
			MMAVReader* parent;
		};
		using iterator = MStreamIterator;
	public:
		MMAVReader();
		~MMAVReader();
	public:
		int open(const Path& file_path);
		int close();
		int read(MMAVPackage& package);
		MMAVPackage read(int& error_number);
	public:
		int getStreamSize();
		int getStream(MMAVStream& dest_stream, int stream_id);
		MMAVStream getStream(int stream_id);
	public:
		MMAVStream operator[](size_t index);
		MStreamIterator begin();
		MStreamIterator end();
	private:
		AVFormatContext* m_av_format_context;
	};
}
#endif // !__MUZI_MMAVREADER_H__
