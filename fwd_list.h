#pragma once
#include <iostream>
#include <string>
#include <initializer_list>
#include <stdexcept>

template <typename Type>
class FwdList final {
private:

	struct Node {
		Type value{};
		Node* next = nullptr;

		Node() = default;
		Node(const Type& lvalue, Node* next) : value(lvalue), next(next) { }
		Node(Type&& rvalue, Node* next) : value(std::move(rvalue)), next(next) { }
	};

	Node head_;
	size_t size_ = 0u;

	template <typename IteratorValueType>
	class BasicIterator {
	private:
		friend class FwdList;

		Node* node_ = nullptr;

		explicit BasicIterator(Node* node) : node_(node) { }

	public:
		using value_type = Type;
		using reference = value_type&;
		using pointer = value_type*;

		BasicIterator() = default;

		BasicIterator(const BasicIterator<value_type>& other) : node_(other.node_) { }

		BasicIterator& operator = (const BasicIterator& rhs) = default;

		bool operator == (const BasicIterator& rhs) const { return node_ == rhs.node_; }
		bool operator != (const BasicIterator& rhs) const { return !(*this == rhs); }

		BasicIterator& operator ++ () {
			using namespace std;

			if (!node_) throw out_of_range("null iterator increment"s);
			node_ = node_->next;
			return *this;
		}

		BasicIterator& operator ++ (int) {
			BasicIterator tmp(*this);
			++(*this);
			return tmp;
		}

		reference operator * () const {
			using namespace std;

			if (!node_) throw out_of_range("null iterator * operation"s);

			return node_->value;
		}

		reference operator -> () const {
			using namespace std;

			if (!node_) throw out_of_range("null iterator -> operation"s);

			return &(node_->value);
		}
	};

public:
	FwdList() = default;

	FwdList(std::initializer_list<Type> values) {
		CopyAndSwapFromIteratorRange(values.begin(), values.end());
	}

	FwdList(const FwdList& lvalue) {
		CopyAndSwapFromIteratorRange(lvalue.begin(), lvalue.end());
	}

	FwdList(FwdList&& rvalue) {
		swap(rvalue);
	}

	~FwdList() {
		clear();
	}

	FwdList& operator = (const FwdList& lvalue) {
		if (this != &lvalue) {
			FwdList tmp_copy(lvalue);
			swap(tmp_copy);
		}
		return *this;
	}

	FwdList& operator = (FwdList&& rvalue) {
		swap(rvalue);
	}

	using Iterator = BasicIterator<Type>;
	using ConstIterator = BasicIterator<const Type>;

	Iterator before_begin() { return Iterator(&head_);     }
	Iterator begin()        { return Iterator(head_.next); }
	Iterator end()          { return Iterator(nullptr);    }

	ConstIterator cbefore_begin() const { return Iterator(const_cast<Node*>(&head_));     }
	ConstIterator cbegin()        const { return Iterator(const_cast<Node*>(head_.next)); }
	ConstIterator cend()          const { return Iterator(nullptr);                       }

	ConstIterator before_begin() const { return cbefore_begin(); }
	ConstIterator begin()        const { return cbegin();        }
	ConstIterator end()          const { return cend();          }

	size_t size() const {
		return size_;
	}

	bool empty() const {
		return size_ == 0u;
	}

	void push_back(const Type& lvalue) {
		Node* back = &head_;
		while (back->next) back = back->next;

		back->next = new Node(lvalue, nullptr);
		++size_;
	}

	void push_back(Type&& rvalue) {
		Node* back = &head_;
		while (back->next) back = back->next;

		back->next = new Node(std::move(rvalue), nullptr);
		++size_;
	}

	Type pop_back() {
		using namespace std;

		if (empty()) throw out_of_range("pop_back from empty fwd_list"s);

		Node* pre_back = &head_;
		Node* back = head_.next;

		while (back->next) {
			pre_back = pre_back->next;
			back = back->next;
		}

		Type tmp(std::move(back->value));

		delete back;
		pre_back->next = nullptr;
		--size_;

		return tmp;
	}

	void push_front(const Type& lvalue) {
		head_.next = new Node(lvalue, head_.next);
		++size_;
	}

	void push_front(Type&& rvalue) {
		head_.next = new Node(std::move(rvalue), head_.next);
		++size_;
	}

	Type pop_front() {
		using namespace std;

		if (empty()) throw out_of_range("pop_front from empty fwd_list"s);

		Node* head_next = head_.next;
		Node* head_next_next = head_next->next;

		Type tmp(std::move(head_next->value));

		delete head_next;
		head_.next = head_next_next;
		--size_;

		return tmp;
	}

	Iterator insert_after(ConstIterator pos, const Type& lvalue) {
		using namespace std;

		if (!pos.node_) throw out_of_range("insert_after null iterator"s);

		pos.node_->next = new Node(lvalue, pos.node_->next);
		++size_;
		return Iterator(pos.node_->next);
	}

	Iterator insert_after(ConstIterator pos, Type&& rvalue) {
		using namespace std;

		if (!pos.node_) throw out_of_range("insert_after null iterator"s);

		pos.node_->next = new Node(std::move(rvalue), pos.node_->next);
		++size_;
		return Iterator(pos.node_->next);
	}

	Iterator erase_after(ConstIterator pos) {
		using namespace std;

		if (!pos.node_ || !pos.node_->next) throw out_of_range("erase_after null iterator"s);

		Node* after_erased_node = pos.node_->next->next;
		delete pos.node_->next;
		pos.node_->next = after_erased_node;
		--size_;

		return Iterator(after_erased_node);
	}

	void clear() {
		while (head_.next) {
			Node* tmp = head_.next->next;
			delete head_.next;
			head_.next = tmp;
		}

		size_ = 0u;
	}

	void swap(FwdList& other) {
		std::swap(head_.next, other.head_.next);
		std::swap(size_, other.size_);
	}

private:

	template <typename ContainerIterator>
	void CopyAndSwapFromIteratorRange(const ContainerIterator from, const ContainerIterator to) {
		FwdList<Type> tmp;

		Node* back = &tmp.head_;

		for (ContainerIterator it = from; it != to; ++it) {
			back->next = new Node(*it, nullptr);
			back = back->next;
			++tmp.size_;
		}

		swap(tmp);
	}
};

template <typename Type>
void swap(FwdList<Type>& lhs, FwdList<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
std::ostream& operator << (std::ostream& os, const FwdList<Type>& fwd_list) {
	using namespace std;

	os << "{"s;
	bool first = true;
	for (const Type& element : fwd_list) {
		if (!first) os << ", "s;
		else        first = false;
		os << element;
	}
	os << "}"s;
	return os;
}