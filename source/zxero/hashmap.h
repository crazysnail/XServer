/* 
 * HashMap.h 
 */  
  
#ifndef HASHMAP_H_  
#define HASHMAP_H_  
  
#include <iostream>  

namespace zxero
{

	using namespace std;  

	//List all the integer number no less than 57 total number is 28  
	//And each number is about half of its next number  
	static int prime[28] =  
	{  
		57,        97,         193,        389,        769,  
		1543,      3079,       6151,       12289,      24593,  
		49157,     98317,      196613,     393241,     786433,  
		1572869,   3145739,    6291469,    12582917,   25165843,  
		50331653,  100663319,  201326611,  402653189,  805306457,  
		1610612741  
	};  

	class HashMapUtil  
	{  
	public:  
		static int find_NextPrimeNumber(int current)  
		{  
			//Find the next prime number by searching in the prime number list  
			int i = 0;  
			for( ; i < 28 ; i++ )  
				if(current < prime[i])  
					break;  
			return prime[i];     //return the next larger prime.  
		}  
	};  

	template <class Key, class Value, class HashFunc, class EqualKey>  
	class HashMap  
	{  
	private:  
		template <class _Key, class _Value>  
		class KeyNode  
		{  
		public:  
			_Value  value;      //Store the value  
			_Key    key;        //Store the keyword  
			int    used;  
			//if the type of Value/Key is your own class, make sure they can handle copy constructor and operator =  
			KeyNode():used(0){}  
			KeyNode(const KeyNode & kn)  
			{  
				value = kn.value;  
				key = kn.key;  
				used = kn.used;  
			}  
			KeyNode & operator=(const KeyNode & kn)  
			{  
				if(this == &kn) return *this;  
				value = kn.value;  
				key = kn.key;  
				used = kn.used;  
				return *this;  
			}  
		};  

	public:  
		HashMap();  
		~HashMap();  
		bool insert(const Key& hashKey, const Value& value);  
		bool remove(const Key& hashKey);  
		void rehash();  //use it when rehashing  
		Value& find(const Key& hashKey);  
		const Value& operator [](const Key& hashKey) const;  
		Value& operator [](const Key& hashKey);  

	private:  
		HashFunc hash;  
		EqualKey equal;  
		HashMapUtil hml;  
		KeyNode<Key ,Value> *table;  
		int size;    //current number of itmes  
		int capacity;   //capacity of the array  
		static const double loadingFactor;  
		int findKey(const Key& hashKey);  //find the index of a key  
	};  

	template<class Key , class Value , class HashFunc , class EqualKey>  
	const double HashMap<Key, Value, HashFunc, EqualKey>::loadingFactor = 0.9;  

	template<class Key , class Value , class HashFunc , class EqualKey>  
	HashMap<Key, Value, HashFunc, EqualKey>::HashMap()  
	{  
		hash = HashFunc();  
		equal = EqualKey();  
		hml = HashMapUtil();  
		capacity = hml.find_NextPrimeNumber(0); //initialize the capacity with first primer 57  
		//resize the table with capacity because an extra one is used  
		//to return the NULL type of Value in the function find  
		table = new KeyNode<Key,Value>[capacity+1];  
		for(int i = 0 ; i < capacity ; i++)    //initialize the table  
			table[i].used = 0;  
		size = 0;  
	}  

	template<class Key, class Value, class HashFunc, class EqualKey>  
	HashMap<Key, Value, HashFunc, EqualKey>::~HashMap()  
	{  
		delete []table;  
	}  

	template<class Key, class Value, class HashFunc, class EqualKey>  
	bool HashMap<Key, Value, HashFunc, EqualKey>::insert(const Key& hashKey, const Value& value)  
	{  
		int index = hash(hashKey)%capacity;  
		//cout<<"Index is "<<index<<endl;  
		if(table[index].used == 1)  //the key-value's hash is unique  
		{  
			//cout<<"The key-value must be unique!"<<endl;  
			return false;  
		}  
		table[index].used = 1;         //modify the KeyNode  
		table[index].key = hashKey;  
		table[index].value = value;  

		size++;  
		//if the table's size is too large ,then rehash it  
		if (size >= capacity * loadingFactor)  
			rehash();  
		return true;  
	}  

	template<class Key, class Value, class HashFunc, class EqualKey>  
	void HashMap<Key, Value, HashFunc, EqualKey>::rehash()  
	{  
		int pastsize = capacity;  
		//create a new array to copy the information in the old table  
		capacity = hml.find_NextPrimeNumber(capacity);  
		KeyNode<Key,Value>* tmp = new KeyNode<Key,Value>[capacity];  
		for(int i = 0 ; i < pastsize ; i++)  
		{  
			if(table[i].used == 1)       //copy the KeyNode into the tmp array  
			{  
				tmp[i] = table[i];  
			}  
		}  
		delete []table; //release the memory of the old table  

		table = new KeyNode<Key,Value>[capacity+1];   //resize the table  
		for(int i = 0 ; i < capacity ; i++) //initialize the table  
		{  
			table[i].used = 0;  
		}  
		for(int i = 0 ; i < pastsize ; i++) //insert the item into the table one by one  
		{  
			if(tmp[i].used == 1)  
				insert(tmp[i].key, tmp[i].value);  
		}  
		delete []tmp;               //delete the tmp array  
	}  

	template<class Key, class Value, class HashFunc, class EqualKey>  
	bool HashMap<Key, Value, HashFunc, EqualKey>::remove(const Key& hashKey)  
	{  
		int index = findKey(hashKey); //find the index of the key  
		if(index < 0) //if find modify the flag with 0,else print out "no such key!"  
		{  
			cout<<"No such Key!"<<endl;  
			return false;  
		}  
		else  
		{  
			table[index].used = 0;  
			size--;  
			return true;  
		}  
	}  

	template<class Key, class Value, class HashFunc, class EqualKey>  
	Value& HashMap<Key, Value, HashFunc, EqualKey>::find(const Key& hashKey)  
	{  
		int index = findKey(hashKey);  
		if(index < 0) //if index <0 ,not found,else return the index  
		{  
			cout<<"can not find the key!"<<endl;  
			return table[capacity].value; //return NULL  
		}  
		else  
		{  
			return table[index].value;  
		}  
	}  

	template<class Key, class Value, class HashFunc, class EqualKey>  
	const Value& HashMap<Key, Value, HashFunc, EqualKey>::operator[](const Key& hashKey) const  
	{  
		return find(hashKey); //overload the operation to return the value of the element  
	}  

	template<class Key, class Value, class HashFunc, class EqualKey>  
	Value& HashMap<Key, Value, HashFunc, EqualKey>::operator[](const Key& hashKey)  
	{  
		return find(hashKey); //overload the operation to return the value of the element  
	}  

	template<class Key, class Value, class HashFunc, class EqualKey>  
	int HashMap<Key, Value, HashFunc, EqualKey>::findKey(const Key& hashKey)  
	{  
		int index = hash(hashKey)%capacity;  
		if ((table[index].used != 1) || !equal(table[index].key,hashKey))  
			return -1;  
		else  
			return index;  
	}  


	//Hash function you provided must be correspond to the type of the Key  
	class StrHashFunc  
	{  
	public:  
		int operator()(const char* key )  
		{  
			if( key == nullptr )
				return -1;

			int hash = 0;  
			for(int i = 0; i < strlen(key); ++i)  
			{  
				hash = hash << 7 ^ key[i];  
			}  
			return (hash & 0x7FFFFFFF);  
		}  
	};  

	//Equal function you provided to check whether two Keys are equal  
	//must be correspond to the type of the Key  
	class StrEqualKey  
	{  
	public:  
		bool operator()(const char *A ,const char *B)  
		{  
			if( A == nullptr || B == nullptr )
				return false;
			if(strcmp(A,B) == 0)  
				return true;    //if equal return true  
			else  
				return false;   //else false  
		}  
	};  


	////Hash function you provided must be correspond to the type of the Key  
	//class IntHashFunc  
	//{  
	//public:  
	//	int operator()(const char* key )  
	//	{  
	//		if( key == nullptr )
	//			return -1;
	//
	//		int hash = 0;  
	//		for(int i = 0; i < strlen(key); ++i)  
	//		{  
	//			hash = hash << 7 ^ key[i];  
	//		}  
	//		return (hash & 0x7FFFFFFF);  
	//	}  
	//};  
	//
	////Equal function you provided to check whether two Keys are equal  
	////must be correspond to the type of the Key  
	//class IntEqualKey  
	//{  
	//public:  
	//	bool operator()(const char *A ,const char *B)  
	//	{  
	//		if( A == nullptr || B == nullptr )
	//			return false;
	//		if(strcmp(A,B) == 0)  
	//			return true;    //if equal return true  
	//		else  
	//			return false;   //else false  
	//	}  
	//}; 

}
  
#endif /* HASHMAP_H_ */ 