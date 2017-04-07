#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

	template <typename KeyType, typename ValueType>
	class TreeMap {
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

		TreeMap()
			: root(nullptr)
			, size(0)
		{}

		TreeMap(std::initializer_list<value_type> list)
			: TreeMap()
		{
			for (auto&& it : list) {
				insert(it.first, it.second);
			}
		}

		TreeMap(const TreeMap& other)
			: root(nullptr)
			, size(other.size)
		{
			root = copyTreeStructure(nullptr, other.root);
		}

		TreeMap(TreeMap&& other)
			: root(other.root)
			, size(other.size)
		{
			other.root = nullptr;
			other.size = 0;
		}

		~TreeMap()
		{
			clear(root);
		}

		TreeMap& operator=(const TreeMap& other)
		{
			if (this != &other) {
				clear(root);
				root = copyTreeStructure(nullptr, other.root);
				size = other.size;
			}
			return *this;
		}

		TreeMap& operator=(TreeMap&& other)
		{
			if (this != &other) {
				clear(root);
				root = other.root;
				size = other.size;
				other.root = nullptr;
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
			Node* temp = root;

			for (;;) {
				if (temp == nullptr) {
					return cend();
				}

				if (temp->data.first == key) {
					return const_iterator(*this, temp);
				}

				if (key > temp->data.first) {
					temp = temp->right;
				}
				else {
					temp = temp->left;
				}
			}
		}

		iterator find(const key_type& key)
		{
			Node* temp = root;

			for (;;) {
				if (temp == nullptr) {
					return end();
				}

				if (temp->data.first == key) {
					return iterator(*this, temp);
				}

				if (key > temp->data.first) {
					temp = temp->right;
				}
				else {
					temp = temp->left;
				}
			}
		}

		void remove(const key_type& key)
		{
			remove(find(key));
		}

		void remove(const const_iterator& it)
		{
			if (isEmpty()) {
				throw std::out_of_range("cannot remove from empty map");
			}

			if (it == end()) {
				throw std::out_of_range("cannot remove element with non-existent key");
			}

			erase(it.node);
			--size;
		}

		size_type getSize() const
		{
			return size;
		}

		bool operator==(const TreeMap& other) const
		{
			if (size != other.size) {
				return false;
			}

			auto it_this = begin();
			auto it_other = other.begin();
			for (; it_this != end(); ++it_this, ++it_other) {
				if (*it_this != *it_other) {
					return false;
				}
			}
			return true;
		}

		bool operator!=(const TreeMap& other) const
		{
			return !(*this == other);
		}

		iterator begin()
		{
			if (root == nullptr) {
				return end();
			}

			Node* temp = root;
			while (temp->left != nullptr) {
				temp = temp->left;
			}
			return iterator(*this, temp);
		}

		iterator end()
		{
			return iterator(*this, nullptr);
		}

		const_iterator cbegin() const
		{
			if (root == nullptr) {
				return cend();
			}

			Node* temp = root;
			while (temp->left != nullptr) {
				temp = temp->left;
			}
			return const_iterator(*this, temp);
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
		class Node {
		public:
			value_type data;
			Node* parent;
			Node* left;
			Node* right;

			Node(value_type data, Node* parent = nullptr, Node* left = nullptr, Node* right = nullptr)
				: data(data)
				, parent(parent)
				, left(left)
				, right(right)
			{}

		};

		Node* root;
		size_type size;

		void clear(Node* node)
		{
			if (node == nullptr) {
				return;
			}
			clear(node->left);
			clear(node->right);
			delete node;
		}

		Node* copyTreeStructure(Node* parent, Node* other_node)
		{
			if (other_node == nullptr) {
				return nullptr;
			}
			Node* to_add = new Node(other_node->data, parent);
			to_add->left = copyTreeStructure(to_add, other_node->left);
			to_add->right = copyTreeStructure(to_add, other_node->right);
			return to_add;
		}

		iterator insert(const key_type& key, const mapped_type& value)
		{
			iterator search = find(key);
			if (search != end()) {
				return search;
			}

			Node* to_add = new Node(std::make_pair(key, value));
			++size;

			if (root == nullptr) {
				root = to_add;
				return iterator(*this, to_add);
			}

			for (Node* iter = root; ; ) {
				if (key < iter->data.first) {
					if (iter->left == nullptr) {
						iter->left = to_add;
						to_add->parent = iter;
						return iterator(*this, to_add);
					}
					else {
						iter = iter->left;
					}
				}
				else {
					if (iter->right == nullptr) {
						iter->right = to_add;
						to_add->parent = iter;
						return iterator(*this, to_add);
					}
					else {
						iter = iter->right;
					}
				}
			}
		}

		void erase(Node* node)
		{
			//no children
			if (node->left == nullptr && node->right == nullptr) {
				if (node->parent != nullptr) {
					if (node == node->parent->left) {
						node->parent->left = nullptr;
					}
					else {
						node->parent->right = nullptr;
					}
				}
				else {
					root = nullptr;
				}
				delete node;
				return;
			}

			//one child
			if (node->left != nullptr || node->right != nullptr) {
				Node* child = node->left ? node->left : node->right;
				if (node->parent != nullptr) {
					if (node == node->parent->left) {
						node->parent->left = child;
					}
					else {
						node->parent->right = child;
					}
				}
				else {
					root = child;
				}
				child->parent = node->parent;
				delete node;
				return;
			}


			//left and right children
			Node* min = node->right;
			while (min->left != nullptr) {
				min = min->left;
			}
			Node* holder = new Node(std::make_pair(min->data.first, min->data.second), node->parent, node->left, node->right);
			if (node->parent != nullptr) {
				if (node == node->parent->left) {
					node->parent->left = holder;
				}
				else {
					node->parent->right = holder;
				}
			}
			else {
				root = holder;
			}
			delete node;
			erase(min);
		}
	};

	template <typename KeyType, typename ValueType>
	class TreeMap<KeyType, ValueType>::ConstIterator {
	public:
		using reference = typename TreeMap::const_reference;
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = typename TreeMap::value_type;
		using pointer = const typename TreeMap::value_type*;

		friend class TreeMap;

		explicit ConstIterator(const TreeMap& parent, Node* node)
			: parent(parent)
			, node(node)
		{}

		ConstIterator(const ConstIterator& other)
			: parent(other.parent)
			, node(other.node)
		{}

		ConstIterator& operator++()
		{
			if (node == nullptr) {
				throw std::out_of_range("cannot increment end() iterator");
			}

			if (node->right != nullptr) {
				node = node->right;
				while (node->left != nullptr) {
					node = node->left;
				}
				return *this;
			}

			key_type key = node->data.first;
			node = node->parent;
			while (node != nullptr && node->data.first < key) {
				node = node->parent;
			}
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

			if (node == nullptr) {
				node = parent.root;
				while (node->right != nullptr) {
					node = node->right;
				}
				return *this;
			}

			if (node->left != nullptr) {
				node = node->left;
				while (node->right != nullptr) {
					node = node->right;
				}
				return *this;
			}

			key_type key = node->data.first;
			node = node->parent;
			while (node != nullptr && node->data.first > key) {
				node = node->parent;
			}
			return *this;
		}

		ConstIterator operator--(int)
		{
			ConstIterator copy = *this;
			--(*this);
			return copy;
		}

		reference operator*() const
		{
			if (node == nullptr) {
				throw std::out_of_range("cannot dereference end() iterator");
			}

			return node->data;
		}

		pointer operator->() const
		{
			return &this->operator*();
		}

		bool operator==(const ConstIterator& other) const
		{
			return node == other.node;
		}

		bool operator!=(const ConstIterator& other) const
		{
			return !(*this == other);
		}

	protected:
		const TreeMap& parent;
		Node* node;
	};

	template <typename KeyType, typename ValueType>
	class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator {
	public:
		using reference = typename TreeMap::reference;
		using pointer = typename TreeMap::value_type*;

		explicit Iterator(const TreeMap& parent, Node* node)
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

#endif /* AISDI_MAPS_MAP_H */
