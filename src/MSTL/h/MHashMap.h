#ifndef __MUZI_MHASHMAP_H__
#define __MUZI_MHASHMAP_H__
#include"MMap.h"
#include"MTreeMap.h"
#include"MBitmapAllocator.h"
namespace MUZI
{
	template<typename K, typename V>
		requires std::totally_ordered<K>
	class MHashMap
	{
	public:
		template<typename V>
		struct __MMap_List__
		{
		private:
			static MBitmapAllocator<struct __MMap_List__> alloc;
		public:
			__MMap_List__(V value)
			{
				this->value = value;
				this->next = nullptr;
			}
			__MMap_List__(const __MMap_List__<V>&) = delete;
			__MMap_List__(__MMap_List__<V>&& list)
			{
				this->value = value;
				this->next = this->next;
			}
		public:
			V value;
			__MMap_List__* next;
		};
	public:
		using KEY = uint32_t;
		using VALUE = struct __MMap_List__;
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
		// ÉèÖÃmap
		void set(K& key, V& value)
		{
			KEY k = this->hashmap_1(key);

			this->m_map.set(k, value);
		}
		const V& get(K& key)
		{
			return this->m_map.get()
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
		uint32_t hashmap_1(K key)
		{

		}
		uint32_t hashmap_2(K key)
		{

		}
	private:
		MTreeMap<KEY, VALUE> m_map;
	};
}



#endif // !__MUZI_MHASHMAP_H__
