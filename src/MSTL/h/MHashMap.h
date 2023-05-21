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
		static V null;
	public:
		template<typename V, typename K = uint64_t>
			requires std::totally_ordered<K>
		struct __MMapPair__
		{
		private:
			static MBitmapAllocator<__MMapPair__<V, K> > alloc;	
		public:
			__MMapPair__(): next(nullptr){}
			__MMapPair__(K& first_key, K& second_key): first_key(first_key), second_key(second_key), value(V()), next(nullptr) {}
			__MMapPair__(K&& first_key, K&& second_key): first_key(first_key), second_key(second_key), value(V()), next(nullptr) {}
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
				return std::strong_ordering::equivalent;
			}
			bool operator!=(const __MMapPair__<V ,K>& that)
			{
				return (*this <=> that) != 0;
			}
			void operator=(const __MMapPair__<V, K>& that)
			{
				this->first_key = that.first_key;
				this->second_key = that.second_key;
				this->value = that.value;
				__MMapPair__<V, K>* tmp_node = this->node;
				__MMapPair__<V, K>* lase_tmp_node = this->node;
				while (tmp_node != nullptr)
				{
					lase_tmp_node = tmp_node;
					this->alloc.deallocate(lase_tmp_node);
					tmp_node = tmp_node->node;
				}
				this->node = that.node;
			}
		public:
			__MMapPair__* match(K& second_key)
			{
				if (this->second_key == second_key)
				{
					return this;
				}
				__MMapPair__* ret_p = this->node;
				while (ret_p != nullptr)
				{
					if (ret_p->second_key == second_key)
					{
						return ret_p;
					}
					ret_p = ret_p->node;
				}
				
			}
		public:
			__MMapPair__<V, K>* next()
			{
				return this->node;
			}
			__MMapPair__<V, K>* insert(__MMapPair__<V, K>& pair)
			{
				this->next = this->alloc.allocate(1);
				this->next = new(this->next) __MMapPair__<V, K>(pair.first_key, pair.second_key, pair.value);
			}
			__MMapPair__<V, K>* insert(K& first_key, K& second_key, V& value)
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
		using KEY = uint64_t;
	public:
		MHashMap()
		{
			this->tree = new MRBTree<__MMapPair__<V>>();
		}
		MHashMap(const MHashMap<K, V>& map) = delete;
		MHashMap(MHashMap<K, V>&& that)
		{
			this->tree = that.tree;
			that.tree = nullptr; 
		}
		~MHashMap()
		{
			if (this->tree == nullptr)
			{
				delete this->tree;
			}
		}
	public:
		// 设置map
		void set(K& key, V& value)
		{
			__MMapPair__<V> tmp_pair(hashmap_1(key), hashmap_2(key), value);
			this->tree->set(tmp_pair, tmp_pair);
			
		}
		const V& get(K& key)
		{
			KEY key1 = hashmap_1(key);
			KEY key2 = hashmap_2(key);
			__MMapPair__<V> tmp_pair(key1, key2);
			__MMapPair__<V>* tmp_p_pair = nullptr;
			if ((tmp_p_pair = this->tree->find(tmp_pair)) == nullptr)
			{
				return this->null;
			}
			return tmp_p_pair->match(key2)->value;

		}
		const V get(K& key) const
		{
			KEY key1 = hashmap_1(key);
			KEY key2 = hashmap_2(key);
			__MMapPair__<V> tmp_pair(key1, key2);
			__MMapPair__<V>* tmp_p_pair = nullptr;
			if ((tmp_p_pair = this->tree->find(tmp_pair)) == nullptr)
			{
				return this->null;
			}
			return tmp_p_pair->match(key2)->value;
		}
		void insert(K& key, V& value)
		{
			__MMapPair__<V> tmp_pair(hashmap_1(key), hashmap_2(key), value);
			__MMapPair__<V>* tmp_p_pair = nullptr;
			if ((tmp_p_pair = this->tree->find(tmp_pair)) != nullptr)
			{
				tmp_p_pair->insert(tmp_pair);
			}
			else
			{
				this->tree->insert(tmp_pair);
			}
		}
		V& operator[](K& key)
		{
			V& tmp_v = this->null;
			KEY key1 = hashmap_1(key), key2 = hashmap_2(key);
			__MMapPair__<V> tmp_pair(key1, key2);
			__MMapPair__<V>* tmp_p_pair = nullptr;
			if ((tmp_p_pair = this->tree->find(tmp_pair)) != nullptr)
			{
				return tmp_p_pair->match(key2)->value;
			}
			tmp_v = V();
			this->insert(key, tmp_v);
			return tmp_v;
		}
	private:
		void __delete__()
		{
			if (this->tree != nullptr)
			{
				delete this->tree;
			}
		}
	private:
		//MurmurHash
		static KEY hashmap_1(K& key)
		{
			const uint64_t m = 0xc6a4a7935bd1e995;
			const int r = 47;
			unsigned int seed = 0x6589744551114;
			int len = sizeof(K);

			uint64_t h = seed ^ (len * m);

			const uint64_t* data = (const uint64_t*)key;
			const uint64_t* end = data + (len / 8);

			while (data != end)
			{
				uint64_t k = *data++;

				k *= m;
				k ^= k >> r;
				k *= m;

				h ^= k;
				h *= m;
			}

			const unsigned char* data2 = (const unsigned char*)data;

			switch (len & 7)
			{
			case 7: h ^= uint64_t(data2[6]) << 48;
			case 6: h ^= uint64_t(data2[5]) << 40;
			case 5: h ^= uint64_t(data2[4]) << 32;
			case 4: h ^= uint64_t(data2[3]) << 24;
			case 3: h ^= uint64_t(data2[2]) << 16;
			case 2: h ^= uint64_t(data2[1]) << 8;
			case 1: h ^= uint64_t(data2[0]);
				h *= m;
			};

			h ^= h >> r;
			h *= m;
			h ^= h >> r;

			return h;
		}
		//IDEA HASH
		static KEY hashmap_2(K& key)
		{
			int result = 0;
			int key_size = sizeof(K);
			// 如果大于八个字节
			if (key_size >= 2 * sizeof(int))
			{
				int multiple = key_size / sizeof(int);
				for (int i = 0; i < multiple; ++i)
				{
					// 取每一位做运算
					// 这里基于IDEA自动生成的HashCode编写
					result = 31 * result + *(static_cast<int*>(&key) + i);
				}
				return result;
			}
			else
			{
				return reinterpret_cast<int>(key);
			}
		}

	private:
		MRBTree< __MMapPair__<V> >* tree;
	};
}



#endif // !__MUZI_MHASHMAP_H__
