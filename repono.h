/*
MIT License

Copyright (c) 2021 Delta17x

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef _REPONO_
#define _REPONO_
#ifndef _INITIALIZER_LIST_
#include <initializer_list>
#endif // !_INITIALIZER_LIST_
namespace as {
// Category: Size Type
#ifdef __EDG_SIZE_TYPE__
	typedef __EDG_SIZE_TYPE__ size_type;
#elif defined __SIZE_TYPE__
	typedef __SIZE_TYPE__ size_type;
#elif defined _WIN64
	typedef unsigned long long size_type;
#else
	typedef unsigned long int size_type;
#endif
	
// Category: Default Allocators
	// The default allocator used for repono data structures.
	template<typename T>
	struct allocator {
		using value_type = T;
		using ptr_type = T*;
		[[nodiscard]] static inline constexpr ptr_type alloc(const size_type amount) noexcept {
			return new value_type[amount];
		} 
		static inline constexpr void dealloc(ptr_type allocated) noexcept {
			delete[] allocated;
		}
	};
// Category: Array

	// Encapsulates an array with a fixed length (does not need to be a compile-time constant). Allocated on the heap.
	template<typename T, class Allocator = ::as::allocator<T>>
	class array {
		using value_type = T;
		using ptr_type = T*;
	public:
		array() : siz(8) {
			ptr = Allocator::alloc(8);
		}
		array(size_type s) : siz(s) {
			ptr = Allocator::alloc(s);
		}
		array(const value_type& first, size_type s) : siz(s) {
			ptr = Allocator::alloc(s);
			ptr[0] = first;
		}
		array(const array<value_type, Allocator>& other) {
			ptr = Allocator::alloc(siz = other.siz);
			for (int i = 0; i < siz; i++) {
				ptr[i] = other.ptr[i];
			}
		}
		array(const std::initializer_list<value_type> a) {
			ptr = Allocator::alloc(siz = a.size());
			int i = 0;
			for (const value_type& iter : a) {
				ptr[i++] = iter;
			}
		}
		inline constexpr array<value_type, Allocator>& operator= (const array<value_type, Allocator>& other) {
			for (int i = 0; i < (siz = other.siz); i++) {
				ptr[i] = other.ptr[i];
			}
			return *this;
		}
		~array() {
			Allocator::dealloc(ptr);
		}
		[[nodiscard]] inline constexpr value_type& operator[] (size_type a) {
			return ptr[a];
		}
		inline const constexpr size_type size() {
			return siz;
		}
		[[nodiscard]] inline constexpr const ptr_type begin() {
			return &ptr[0];
		}
		[[nodiscard]] inline constexpr const ptr_type end() {
			return &ptr[siz];
		}
	private:
		ptr_type ptr;
		size_type siz;
	};

	template<typename T, typename Alloc>
	inline constexpr const bool operator==(const array<T, Alloc>& a, const array<T, Alloc>& other) noexcept {
		if (a.size() != other.size())
			return false;
		for (int i = 0; i < a.size(); i++)
			if (a[i] != other[i])
				return false;
		return true;
	}

// Category: Linked List

	template<typename T>
	class linked_list_node {
		using value_type = T;
	public:
		value_type value;
		linked_list_node* next;
		linked_list_node* back;
		linked_list_node() {
			value = value_type();
			next = back = nullptr;
		}
		linked_list_node(const value_type& val) {
			value = val;
			next = back = nullptr;
		}
		linked_list_node(const value_type& val, linked_list_node* n, linked_list_node* b) {
			value = val;
			n->back = b->next = this;
			next = n;
			back = b;
		}
	};

	template<typename T>
	class linked_list {
		using value_type = T;
		using ptr_type = T*;
		int _size;
	public:
		linked_list_node<value_type>* first;
		linked_list_node<value_type>* last;
		linked_list() {
			first = last = new linked_list_node<value_type>();
			_size = 1;
		}
		linked_list(const value_type& val) {
			first = last = new linked_list_node<value_type>(val);
			_size = 1;
		}
		~linked_list() {
			while (first->next != nullptr) {
				auto temp = first->next;
				delete first;
				first = temp;
			}
		}
		inline value_type& operator[] (size_type index) {
			linked_list_node<value_type>* cur = first;
			for (int i = 0; i < index; i++) {
				cur = cur->next;
			}
			return cur->value;
		}
		inline void add(const value_type& val) {
			last->next = new linked_list_node<value_type>(val);
			last->next->back = last;
			last = last->next;
			_size++;
		}
		inline void remove(const size_type index) {
			linked_list_node<value_type>* cur = first;
			for (int i = 0; i < index; i++) {
				cur = cur->next;
			}
			cur->back->next = cur->next;
			cur->next->back = cur->back;
			_size--;
		}
		// Inserts the element to the index, pushing the value in its place forward.
		inline void insert(size_type index, const value_type& val) {
			if (index < _size++) {
				linked_list_node<value_type>* cur = first;
				for (int i = 0; i < index; i++) {
					cur = cur->next;
				}
				if (cur && cur->back) {
					cur->back->next = new linked_list_node<value_type>(val);
					cur->back->next->next = cur;
					cur->back = cur->back->next;
				}
			}
			else {
				size_type i = 0;
				linked_list_node<value_type>* cur = last;
				while (i++ < index) {
					cur = cur->next = new linked_list_node<value_type>();
				}
				cur->next = new linked_list_node<value_type>(val);
			}
		}
		// Returns the amount of nodes in this linked_list. Note that manual insertion of nodes will cause this to return an incorrect count.
		inline const int size() const noexcept {
			return _size;
		}
	};

// Category: Vector

	// A container that acts as a dynamically sized array.
	template<typename T, class Allocator = ::as::allocator<T>>
	class vector {
		using value_type = T;
		using ptr_type = T*;
		ptr_type ptr;
		size_type occ_elems;
		size_type ptr_size;
	public:
		constexpr vector() : ptr_size(10), occ_elems(1) {
			ptr = Allocator::alloc(10);
			ptr[0] = value_type();
		}
		vector(const size_type size) : occ_elems(1), ptr_size(size) {
			ptr = Allocator::alloc(ptr_size);
			ptr[0] = value_type();
		}
		vector(std::initializer_list<value_type> list) : ptr_size(list.size()), occ_elems(list.size()) {
			ptr = Allocator::alloc(ptr_size);
			int i = 0;
			for (const value_type& iter : list) {
				ptr[i++] = iter;
			}
		}
		vector(const vector<value_type>& other) : occ_elems(other.occ_elems), ptr_size(other.ptr_size) {
			ptr = Allocator::alloc(ptr_size);
			for (int i = 0; i < occ_elems; i++) {
				ptr[i] = other.ptr[i];
			}
		}
		vector(vector<value_type>&& other) noexcept : occ_elems(other.occ_elems), ptr_size(other.ptr_size) {
			ptr = Allocator::alloc(ptr_size);
			for (int i = 0; i < occ_elems; i++) {
				ptr[i] = other.ptr[i];
			}
			Allocator::dealloc(other.ptr);
		}
		inline constexpr vector<value_type>& operator=(const vector<value_type>& other) {
			occ_elems = other.occ_elems;
			Allocator::dealloc(ptr);
			ptr = Allocator::alloc(ptr_size = other.ptr_size);
			for (int i = 0; i < occ_elems; i++) {
				ptr[i] = other.ptr[i];
			}
			return *this;
		}
		~vector() {
			Allocator::dealloc(ptr);
		}
		[[nodiscard]] inline constexpr const ptr_type begin() const noexcept {
			return &ptr[0];
		}
		[[nodiscard]] inline constexpr value_type& operator[] (size_type a) noexcept {
			return ptr[a];
		}
		[[nodiscard]] inline constexpr const ptr_type end() const noexcept {
			return &ptr[occ_elems];
		}
		inline constexpr value_type& at(const int index) {
			if (index < 0 || index >= occ_elems)
				throw "[rn] Index given to vector out of range.";
			return ptr[index];
		}
		inline constexpr const size_type size() const noexcept {
			return occ_elems;
		}
		inline constexpr const size_type capacity() const noexcept {
			return ptr_size;
		}
		inline constexpr const bool empty() const noexcept {
			return occ_elems == 0;
		}
		inline constexpr void clear() noexcept {
			occ_elems = 0;
			*ptr = value_type();
		}
		inline constexpr value_type& pop_back() {
			return ptr[--occ_elems];
		}
		inline constexpr void resize(size_type new_size) {
			size_t old_size = ptr_size;
			auto new_ptr = Allocator::alloc(ptr_size = new_size);
			for (int i = 0; i < old_size; i++) {
				new_ptr[i] = ptr[i];
			}
			Allocator::dealloc(ptr);
			ptr = new_ptr;
		}
		// TODO: optimize a lot
		inline constexpr void insert(const size_type index, const value_type& val) {
			ptr_type to_move_ptr = begin() + index;
			ptr_type to_move = Allocator::alloc(occ_elems - index);
			for (int i = 0; i < occ_elems - index; i++) {
				to_move[i] = to_move_ptr[i];
			}
			ptr[index] = val;
			for (int i = 0; i <= occ_elems; i++) {
				ptr[i + index + 1] = to_move[i];
			}
			occ_elems++;
			Allocator::dealloc(to_move);
		}

		inline constexpr void push_back(const value_type& val) {
			if (occ_elems >= ptr_size) 
				resize(ptr_size * 3);
			ptr[occ_elems++] = val;
		}
	};

	template<typename T, typename Alloc>
	inline constexpr const bool operator==(const vector<T, Alloc>& a, const vector<T, Alloc>& other) noexcept {
		if (a.size() != other.size() || a.capacity() != other.capacity())
			return false;
		for (int i = 0; i < a.size(); i++)
			if (a[i] != other[i])
				return false;
		return true;
	}
}
#endif


