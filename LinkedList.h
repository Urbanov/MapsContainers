#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{
	template <typename Type>
	class LinkedList {
	public:
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
		using value_type = Type;
		using pointer = Type*;
		using reference = Type&;
		using const_pointer = const Type*;
		using const_reference = const Type&;

		class ConstIterator;
		class Iterator;
		using iterator = Iterator;
		using const_iterator = ConstIterator;

		LinkedList()
			: root(nullptr)
			, tail(nullptr)
			, size(0)
		{}

		LinkedList(std::initializer_list<Type> l)
			: root(nullptr)
			, tail(nullptr)
			, size(0)
		{
			for (const auto& it : l) {
				append(it);
			}
		}

		LinkedList(const LinkedList& other)
			: root(nullptr)
			, tail(nullptr)
			, size(0)
		{
			for (const auto& it : other) {
				append(it);
			}
		}

		LinkedList(LinkedList&& other)
			: root(other.root)
			, tail(other.tail)
			, size(other.size)
		{
			other.root = nullptr;
			other.tail = nullptr;
			other.size = 0;
		}

		~LinkedList()
		{
			clear();
		}

		LinkedList& operator=(const LinkedList& other)
		{
			if (this != &other) {
				clear();
				for (const auto& it : other) {
					append(it);
				}
			}
			return *this;
		}

		LinkedList& operator=(LinkedList&& other)
		{
			if (this != &other) {
				clear();
				root = other.root;
				tail = other.tail;
				size = other.size;
				other.root = nullptr;
				other.tail = nullptr;
				other.size = 0;
			}
			return *this;
		}

		bool isEmpty() const
		{
			return !size;
		}

		size_type getSize() const
		{
			return size;
		}

		void append(const Type& item)
		{
			insert(end(), item);
		}

		void prepend(const Type& item)
		{
			insert(begin(), item);
		}

		void insert(const const_iterator& insertPosition, const Type& item)
		{
			Node* to_add = new Node(item);

			if (isEmpty()) {
				root = to_add;
				tail = to_add;
			}
			else if (insertPosition.ptr == root) {
				to_add->next = root;
				root->prev = to_add;
				root = to_add;
			}
			else if (insertPosition.ptr == nullptr) {
				to_add->prev = tail;
				tail->next = to_add;
				tail = to_add;
			}
			else {
				to_add->next = insertPosition.ptr;
				to_add->prev = insertPosition.ptr->prev;
				insertPosition.ptr->prev->next = to_add;
				insertPosition.ptr->prev = to_add;
			}
			++size;
		}

		Type popFirst()
		{
			if (isEmpty()) {
				throw std::logic_error("popping first from empty collection");
			}
			Type copy = std::move(root->data);
			erase(begin());
			return copy;
		}

		Type popLast()
		{
			if (isEmpty()) {
				throw std::logic_error("popping last from empty collection");
			}
			Type copy = std::move(tail->data);
			erase(--end());
			return copy;
		}

		void erase(const const_iterator& possition)
		{
			if (isEmpty()) {
				throw std::out_of_range("erasing element from empty collection");
			}
			if (possition.ptr == nullptr) {
				throw std::out_of_range("erasing end() iterator");
			}
			if (root == tail) {
				root = nullptr;
				tail = nullptr;
			}
			else if (possition.ptr == root) {
				root = root->next;
				root->prev = nullptr;
			}
			else if (possition.ptr == tail) {
				tail = tail->prev;
				tail->next = nullptr;
			}
			else {
				possition.ptr->prev->next = possition.ptr->next;
				possition.ptr->next->prev = possition.ptr->prev;
			}
			delete possition.ptr;
			--size;
		}

		void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
		{
			Node* to_delete = firstIncluded.ptr;
			Node* temp = to_delete;
			while (temp != lastExcluded.ptr) {
				to_delete = temp;
				temp = temp->next;
				erase(iterator(*this, to_delete));
			}
		}

		iterator begin()
		{
			return iterator(*this, root);
		}

		iterator end()
		{
			return iterator(*this, nullptr);
		}

		const_iterator cbegin() const
		{
			return const_iterator(*this, root);
		}

		const_iterator cend() const
		{
			return const_iterator(*this, nullptr);
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
		class Node;

		Node* root;
		Node* tail;
		size_type size;

		void clear()
		{
			size = 0;
			Node* temp;
			while (root != nullptr) {
				temp = root->next;
				delete root;
				root = temp;
			}
			tail = nullptr;
		}
	};

	template <typename Type>
	class LinkedList<Type>::Node {
	public:
		Node* next;
		Node* prev;
		Type data;

		Node(Node* next, Node* prev, const Type& data)
			: next(next)
			, prev(prev)
			, data(data)
		{}

		Node(const Type& data)
			: next(nullptr)
			, prev(nullptr)
			, data(data)
		{}
	};

	template <typename Type>
	class LinkedList<Type>::ConstIterator {
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = typename LinkedList::value_type;
		using difference_type = typename LinkedList::difference_type;
		using pointer = typename LinkedList::const_pointer;
		using reference = typename LinkedList::const_reference;

		friend class LinkedList<Type>;

		explicit ConstIterator(const LinkedList& list, Node* node)
			: parent(list)
			, ptr(node)
		{}

		reference operator*() const
		{
			if (ptr == nullptr) {
				throw std::out_of_range("dereferencing end() iterator");
			}
			return ptr->data;
		}

		ConstIterator& operator++()
		{
			if (ptr == nullptr) {
				throw std::out_of_range("incrementing end() iterator");
			}
			ptr = ptr->next;
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
			if (ptr == parent.root) {
				throw std::out_of_range("decrementing begin() iterator");
			}
			if (ptr == nullptr) {
				ptr = parent.tail;
			}
			else {
				ptr = ptr->prev;
			}
			return *this;
		}

		ConstIterator operator--(int)
		{
			ConstIterator copy = *this;
			--(*this);
			return copy;
		}

		ConstIterator operator+(difference_type d) const
		{
			ConstIterator temp = *this;
			while (d) {
				if (temp.ptr == nullptr) {
					throw std::out_of_range("incrementing end() iterator");
				}
				temp.ptr = temp.ptr->next;
				--d;
			}
			return temp;
		}

		ConstIterator operator-(difference_type d) const
		{
			ConstIterator temp = *this;
			while (d) {
				if (temp.ptr == parent.root) {
					throw std::out_of_range("decrementing begin() iterator");
				}
				if (temp.ptr == nullptr) {
					temp.ptr = parent.tail;
				}
				else {
					temp.ptr = temp.ptr->prev;
				}
				--d;
			}
			return temp;
		}

		pointer operator->() const
		{
			return &this->operator*();
		}

		bool operator==(const ConstIterator& other) const
		{
			return ptr == other.ptr;
		}

		bool operator!=(const ConstIterator& other) const
		{
			return ptr != other.ptr;
		}

	protected:
		const LinkedList<Type>& parent;
		Node* ptr;
	};

	template <typename Type>
	class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator {
	public:
		using pointer = typename LinkedList::pointer;
		using reference = typename LinkedList::reference;

		explicit Iterator(const LinkedList& parent, Node* node)
			: ConstIterator(parent, node)
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

		Iterator operator+(difference_type d) const
		{
			return ConstIterator::operator+(d);
		}

		Iterator operator-(difference_type d) const
		{
			return ConstIterator::operator-(d);
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

#endif // AISDI_LINEAR_LINKEDLIST_H