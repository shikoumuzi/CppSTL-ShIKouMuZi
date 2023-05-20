#ifndef __MUZI_MHASHMAP_H__
#define __MUZI_MHASHMAP_H__
#include"MMap.h"
#include"MTreeMap.h"
#include"MBitmapAllocator.h"
namespace MUZI
{
	template<__MMAP_KEY__ K, __MMAP_VALUE__ V>
		requires std::totally_ordered<K>
	class MHashMap
	{
	public:
		template<typename V, typename K = uint32_t>
			requires std::totally_ordered<K>
		struct __MMapPair__
		{
		private:
			static MBitmapAllocator<__MMapPair__<V, K> > alloc;	
		public:
			__MMapPair__(): next(nullptr){}
			__MMapPair__(K& first_key, K& second_key, V& value) :first_key(first_key), second_key(second_key), value(value), next(nullptr){}
			__MMapPair__(K&& first_key, K&& second_key, V& value) :first_key(first_key), second_key(second_key), value(value), next(nullptr) {}
			__MMapPair__(const __MMapPair__<V>&) = delete;
			__MMapPair__(__MMapPair__<V>&& list)
			{
				this->value = value;
				this->next = this->next;
			}
		public:
			std::weak_ordering operator<=>(const __MMapPair__<V, K>& that)
			{
				if (this->first_key > that.first_key) return std::weak_ordering::greater;
				if (this->first_key < that.first_key) return std::weak_ordering::less;
				if (this->second_key > this->second_key) return std::weak_ordering::greater;
				if (this->second_key < this->second_key) return std::weak_ordering::less;
				return std::strong_ordering::equivalent;
			}
			bool operator!=(const __MMapPair__<V ,K>& that)
			{
				return (*this <=> that) != 0;
			}
		public:
			__MMapPair__<V, K>* next()
			{
				return this->node;
			}
			__MMapPair__<V, K>* createNode(K& first_key, K& second_key, V& value)
			{
				this->next = this->alloc.allocate(1);
				this->next = new(this->next) __MMapPair__<V, K>(first_key, second_key, value);
			}
		public:
			K first_key;
			K second_key;
			V value;
			__MMapPair__<V, K>* node;
		};
	public:
		using KEY = uint32_t;
	public:
		MHashMap()
		{

		}
		MHashMap(const MHashMap<K, V>& map)
		{

		}
		MHashMap(MHashMap<K, V>&& map)
		{

		}
		~MHashMap()
		{

		}
	public:
		// …Ë÷√map
		void set(K& key, V& value)
		{
			__MMapPair__<V> tmp_pair(hashmap_1(key), hashmap_2(key), value);
			this->tree->set(tmp_pair, tmp_pair);
			
		}
		const V& get(K& key)
		{
			
		}
		const V get(K& key) const
		{

		}
		void inset(K& key, V& value)
		{
			
		}
		const V& operator[](K& key)
		{
		}
	private:
		static KEY hashmap_1(K& key)
		{
			if (sizeof(K) > 8)
			{
				return 0;
			}
			else
			{
				return 0;
			}
		}
		static KEY hashmap_2(K& key)
		{

		}
	private:
		MRBTree< __MMapPair__<V> > tree;
	};
}



#endif // !__MUZI_MHASHMAP_H__
