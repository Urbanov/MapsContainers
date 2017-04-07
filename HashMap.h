#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include "LinkedList.h"
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

	template <typename KeyType, typename ValueType>
	class HashMap {
	public:
		using key_type = KeyType;
		using mapped_type = ValueType;
		using value_type = std::pair<const key_type, mapped_type>;
		using size_type = std::size_t;
		using reference = value_type&;
		using const_reference = const value_type&;

		class ConstIterator;
		class Iterator;
		using iterator = Iterator;
		using const_iterator = ConstIterator;

		HashMap()
			: data(new LinkedList<value_type>[BUCKET_COUNT])
			, size(0)
		{}

		HashMap(std::initializer_list<value_type> list)
			: HashMap()
		{
			for (auto&& it : list) {
				insert(it.first, it.second);
			}
		}

		HashMap(const HashMap& other)
			: HashMap()
		{
			for (const auto& it : other) {
				insert(it.first, it.second);
			}
		}

		HashMap(HashMap&& other)
			: data(other.data)
			, size(other.size)
		{
			other.data = nullptr;
			other.size = 0;
		}

		~HashMap()
		{
			delete[] data;
		}

		HashMap& operator=(const HashMap& other)
		{
			if (this != &other) {
				size = 0;
				delete[] data;
				data = new LinkedList<value_type>[BUCKET_COUNT];
				for (const auto& it : other) {
					insert(it.first, it.second);
				}
			}
			return *this;
		}

		HashMap& operator=(HashMap&& other)
		{
			if (this != &other) {
				delete[] data;
				data = other.data;
				size = other.size;
				other.data = nullptr;
				other.size = 0;
			}
			return *this;
		}

		bool isEmpty() const
		{
			return !size;
		}

		mapped_type& operator[](const key_type& key)
		{
			return insert(key, mapped_type())->second;
		}

		const mapped_type& valueOf(const key_type& key) const
		{
			const_iterator search = find(key);
			if (search == cend()) {
				throw std::out_of_range("cannot access non-existent element");
			}
			return search->second;
		}

		mapped_type& valueOf(const key_type& key)
		{
			iterator search = find(key);
			if (search == end()) {
				throw std::out_of_range("cannot access non-existent element");
			}
			return search->second;
		}

		const_iterator find(const key_type& key) const
		{
			size_type bucket = getBucket(key);
			size_type i = 0;
			for (auto it = data[bucket].begin(); it != data[bucket].end(); ++it, ++i) {
				if (it->first == key) {
					return const_iterator(*this, bucket, i);
				}
			}
			return cend();
		}

		iterator find(const key_type& key)
		{
			size_type bucket = getBucket(key);
			size_type i = 0;
			for (auto it = data[bucket].begin(); it != data[bucket].end(); ++it, ++i) {
				if (it->first == key) {
					return iterator(*this, bucket, i);
				}
			}
			return end();
		}

		void remove(const key_type& key)
		{
			if (isEmpty()) {
				throw std::out_of_range("cannot remove from empty map");
			}
			size_type bucket = getBucket(key);
			for (auto it = data[bucket].begin(); it != data[bucket].end(); ++it) {
				if (it->first == key) {
					data[bucket].erase(it);
					--size;
					return;
				}
			}
			throw std::out_of_range("cannot remove element with non-existent key");
		}

		void remove(const const_iterator& it)
		{
			remove(it->first);
		}

		size_type getSize() const
		{
			return size;
		}

		bool operator==(const HashMap& other) const
		{
			if (size != other.size) {
				return false;
			}

			for (size_type i = 0; i < BUCKET_COUNT; ++i) {
				if (data[i].getSize() != other.data[i].getSize()) {
					return false;
				}

				auto it_this = data[i].begin();
				auto it_other = other.data[i].begin();
				for (; it_this != data[i].end(); ++it_this, ++it_other) {
					if (*it_this != *it_other) {
						return false;
					}
				}
			}
			return true;
		}

		bool operator!=(const HashMap& other) const
		{
			return !(*this == other);
		}

		iterator begin()
		{
			for (size_type i = 0; i < BUCKET_COUNT; ++i) {
				if (data[i].getSize() != 0) {
					return iterator(*this, i, 0);
				}
			}
			return iterator(*this, BUCKET_COUNT - 1, 0);
		}

		iterator end()
		{
			return iterator(*this, BUCKET_COUNT - 1, data[BUCKET_COUNT - 1].getSize());
		}

		const_iterator cbegin() const
		{
			for (size_type i = 0; i < BUCKET_COUNT; ++i) {
				if (data[i].getSize() != 0) {
					return const_iterator(*this, i, 0);
				}
			}
			return const_iterator(*this, BUCKET_COUNT - 1, 0);
		}

		const_iterator cend() const
		{
			return const_iterator(*this, BUCKET_COUNT - 1, data[BUCKET_COUNT - 1].getSize());
		}

		const_iterator begin() const
		{
			return cbegin();
		}

		const_iterator end() const
		{
			return cend();
		}

	private:
		static const size_type BUCKET_COUNT = 10000;
		LinkedList<value_type>* data;
		size_type size;

		size_type getBucket(const key_type& key) const
		{
			return std::hash<key_type>{}(key) % BUCKET_COUNT;
		}

		iterator insert(const key_type& key, const mapped_type& value)
		{
			iterator search = find(key);
			if (search != end()) {
				return search;
			}

			size_type bucket = getBucket(key);
			data[bucket].append(std::make_pair(key, value));
			++size;
			return iterator(*this, bucket, data[bucket].getSize() - 1);
		}
	};

	template <typename KeyType, typename ValueType>
	class HashMap<KeyType, ValueType>::ConstIterator {
	public:
		using reference = typename HashMap::const_reference;
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = typename HashMap::value_type;
		using pointer = const typename HashMap::value_type*;

		explicit ConstIterator(const HashMap& parent, size_type bucket, size_type index)
			: parent(parent)
			, bucket(bucket)
			, index(index)
		{}

		ConstIterator(const ConstIterator& other)
			: parent(other.parent)
			, bucket(other.bucket)
			, index(other.index)
		{}

		ConstIterator& operator++()
		{
			if (*this == parent.end()) {
				throw std::out_of_range("cannot increment end() iterator");
			}

			if (++index != parent.data[bucket].getSize()) {
				return *this;
			}

			for (size_type i = bucket + 1; i < BUCKET_COUNT; ++i) {
				if (parent.data[i].getSize() != 0) {
					bucket = i;
					index = 0;
					return *this;
				}
			}

			bucket = BUCKET_COUNT - 1;
			index = parent.data[bucket].getSize();
			return *this;
		}

		ConstIterator operator++(int)
		{
			ConstIterator copy = *this;
			++(*this);
			return copy;
		}

		ConstIterator& operator--()
		{
			if (*this == parent.begin()) {
				throw std::out_of_range("cannot decrement begin() iterator");
			}

			if (index != 0) {
				--index;
				return *this;
			}

			for (size_type i = bucket - 1; /*i >= 0*/; --i) {
				if (parent.data[i].getSize() != 0) {
					bucket = i;
					index = parent.data[bucket].getSize() - 1;
					return *this;
				}
			}
		}

		ConstIterator operator--(int)
		{
			ConstIterator copy = *this;
			--(*this);
			return copy;
		}

		reference operator*() const
		{
			if (index >= parent.data[bucket].getSize()) {
				throw std::out_of_range("cannot dereference end() iterator");
			}
			return *(parent.data[bucket].begin() + index);
		}

		pointer operator->() const
		{
			return &this->operator*();
		}

		bool operator==(const ConstIterator& other) const
		{
			return (&parent == &other.parent && bucket == other.bucket && index == other.index);
		}

		bool operator!=(const ConstIterator& other) const
		{
			return !(*this == other);
		}

	protected:
		const HashMap& parent;
		size_type bucket;
		size_type index;
	};

	template <typename KeyType, typename ValueType>
	class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator {
	public:
		using reference = typename HashMap::reference;
		using pointer = typename HashMap::value_type*;

		explicit Iterator(const HashMap& parent, size_type bucket, size_type index)
			: ConstIterator(parent, bucket, index)
		{}

		Iterator(const ConstIterator& other)
			: ConstIterator(other)
		{}

		Iterator& operator++()
		{
			ConstIterator::operator++();
			return *this;
		}

		Iterator operator++(int)
		{
			auto result = *this;
			ConstIterator::operator++();
			return result;
		}

		Iterator& operator--()
		{
			ConstIterator::operator--();
			return *this;
		}

		Iterator operator--(int)
		{
			auto result = *this;
			ConstIterator::operator--();
			return result;
		}

		pointer operator->() const
		{
			return &this->operator*();
		}

		reference operator*() const
		{
			// ugly cast, yet reduces code duplication.
			return const_cast<reference>(ConstIterator::operator*());
		}
	};

}

#endif /* AISDI_MAPS_HASHMAP_H */
