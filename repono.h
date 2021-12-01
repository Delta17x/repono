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
#include <initializer_list>
namespace rn {
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


	// Category: Tools
	struct true_type {
		constexpr static bool value = true;
		constexpr operator bool() const noexcept { return value; }
	};

	struct false_type {
		constexpr static bool value = false;
		constexpr operator bool() const noexcept { return value; }
	};

	template<typename T>
	struct is_const : public false_type {};

	template<typename T>
	struct is_const<const T> : public true_type {};

#if __cplusplus >= 199711L
#define ASSERT_IF_CONST(x) static_assert(!is_const< x >::value, "Repono and the C++ Standard do not support containers of const objects.")
#else
#define ASSERT_IF_CONST(x)
#endif
	// Category: Default Allocators
		// The default allocator used for repono data structures.
	template<typename T>
	struct allocator {
		using pointer = T*;
		using const_pointer = const T*;
		allocator() {}
		[[nodiscard]] inline constexpr pointer allocate(const size_type amount) const noexcept {
			return new T[amount];
		}
		inline constexpr void deallocate(pointer allocated) const noexcept {
			delete[] allocated;
		}
	};
	// Category: Array

		// Encapsulates an array with a fixed length (does not need to be a compile-time constant). Allocated on the heap.
	template<typename T, class Allocator = ::rn::allocator<T>>
	class array {
		ASSERT_IF_CONST(T);
		using value_type = T;
		using pointer = typename Allocator::pointer;
		using const_pointer = typename Allocator::const_pointer;
		using reference = T&;
		using const_reference = const T&;
		Allocator alloc;
	public:
		array() : siz(8) {
			ptr = alloc.allocate(8);
		}
		array(size_type s) : siz(s) {
			ptr = alloc.allocate(s);
		}
		array(const_reference first, size_type s) : siz(s) {
			ptr = alloc.allocate(s);
			ptr[0] = first;
		}
		array(const array<value_type, Allocator>& other) {
			ptr = alloc.allocate(siz = other.siz);
			for (int i = 0; i < siz; i++) {
				ptr[i] = other.ptr[i];
			}
		}
		array(const std::initializer_list<value_type> a) {
			ptr = alloc.allocate(siz = a.size());
			int i = 0;
			for (const_reference iter : a) {
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
			alloc.deallocate(ptr);
			alloc.~Allocator();
		}
		[[nodiscard]] inline constexpr reference operator[] (size_type a) {
			return ptr[a];
		}
		inline const constexpr size_type size() {
			return siz;
		}
		[[nodiscard]] inline constexpr const_pointer begin() {
			return &ptr[0];
		}
		[[nodiscard]] inline constexpr const_pointer end() {
			return &ptr[siz];
		}
	private:
		pointer ptr;
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

	template<typename T, typename Allocator = ::rn::allocator<T>>
	class linked_list_node {
		using value_type = T;
		using pointer = typename Allocator::pointer;
		using const_pointer = typename Allocator::const_pointer;
		using reference = T&;
		using const_reference = const T&;
	public:
		value_type value;
		linked_list_node* next;
		linked_list_node* back;
		linked_list_node() {
			value = value_type();
			next = back = nullptr;
		}
		linked_list_node(const_reference val) {
			value = val;
			next = back = nullptr;
		}
		linked_list_node(const_reference val, linked_list_node* n, linked_list_node* b) {
			value = val;
			n->back = b->next = this;
			next = n;
			back = b;
		}
	};

	template<typename T, typename Allocator = ::rn::allocator<T>>
	class linked_list {
		using value_type = T;
		using pointer = typename Allocator::pointer;
		using const_pointer = typename Allocator::const_pointer;
		using reference = T&;
		using const_reference = const T&;
		int _size;
	public:
		linked_list_node<value_type, Allocator>* first;
		linked_list_node<value_type, Allocator>* last;
		linked_list() {
			first = last = new linked_list_node<value_type, Allocator>();
			_size = 1;
		}
		linked_list(const_reference val) {
			first = last = new linked_list_node<value_type, Allocator>(val);
			_size = 1;
		}
		~linked_list() {
			while (first->next != nullptr) {
				auto temp = first->next;
				delete first;
				first = temp;
			}
		}
		inline reference operator[] (size_type index) {
			linked_list_node<value_type, Allocator>* cur = first;
			for (int i = 0; i < index; i++) {
				cur = cur->next;
			}
			return cur->value;
		}
		inline void add(const_reference val) {
			last->next = new linked_list_node<value_type, Allocator>(val);
			last->next->back = last;
			last = last->next;
			_size++;
		}
		inline void remove(const size_type index) {
			linked_list_node<value_type, Allocator>* cur = first;
			for (int i = 0; i < index; i++) {
				cur = cur->next;
			}
			cur->back->next = cur->next;
			cur->next->back = cur->back;
			_size--;
		}
		// Inserts the element to the index, pushing the value in its place forward.
		inline void insert(size_type index, const_reference val) {
			if (index < _size++) {
				linked_list_node<value_type, Allocator>* cur = first;
				for (int i = 0; i < index; i++) {
					cur = cur->next;
				}
				if (cur && cur->back) {
					cur->back->next = new linked_list_node<value_type, Allocator>(val);
					cur->back->next->next = cur;
					cur->back = cur->back->next;
				}
			}
			else {
				size_type i = 0;
				linked_list_node<value_type, Allocator>* cur = last;
				while (i++ < index) {
					cur = cur->next = new linked_list_node<value_type, Allocator>();
				}
				cur->next = new linked_list_node<value_type, Allocator>(val);
			}
		}
		// Returns the amount of nodes in this linked_list. Note that manual insertion of nodes will cause this to return an incorrect count.
		inline const int size() const noexcept {
			return _size;
		}
	};

	// Category: Vector

		// A container that acts as a dynamically sized array.
	template<typename T, class Allocator = ::rn::allocator<T>>
	class vector {
		ASSERT_IF_CONST(T);
		using value_type = T;
		using pointer = typename Allocator::pointer;
		using const_pointer = typename Allocator::const_pointer;
		using reference = T&;
		using const_reference = const T&;
		pointer ptr;
		Allocator alloc;
		size_type occ_elems;
		size_type ptr_size;
	public:
		constexpr vector() : ptr_size(10), occ_elems(1) {
			ptr = alloc.allocate(10);
			ptr[0] = value_type();
		}
		vector(const size_type size) : occ_elems(1), ptr_size(size) {
			ptr = alloc.allocate(ptr_size);
			ptr[0] = value_type();
		}
		vector(std::initializer_list<value_type> list) : ptr_size(list.size()), occ_elems(list.size()) {
			ptr = alloc.allocate(ptr_size);
			int i = 0;
			for (const_reference iter : list) {
				ptr[i++] = iter;
			}
		}
		vector(const vector<value_type>& other) : occ_elems(other.occ_elems), ptr_size(other.ptr_size) {
			ptr = alloc.allocate(ptr_size);
			for (int i = 0; i < occ_elems; i++) {
				ptr[i] = other.ptr[i];
			}
		}
		vector(vector<value_type>&& other) noexcept : occ_elems(other.occ_elems), ptr_size(other.ptr_size) {
			ptr = alloc.allocate(ptr_size);
			for (int i = 0; i < occ_elems; i++) {
				ptr[i] = other.ptr[i];
			}
			alloc.deallocate(other.ptr);
		}
		inline constexpr vector<value_type>& operator=(const vector<value_type>& other) {
			occ_elems = other.occ_elems;
			alloc.deallocate(ptr);
			ptr = nullptr;
			ptr = alloc.allocate(ptr_size = other.ptr_size);
			for (int i = 0; i < occ_elems; i++) {
				ptr[i] = other.ptr[i];
			}
			return *this;
		}

		~vector() {
			// TODO: THIS LINE THROWS AN ERROR IF INSERT() IS CALLED
			alloc.deallocate(ptr);
			ptr = nullptr;
			//alloc.~Allocator();
		}
		[[nodiscard]] inline constexpr pointer begin() noexcept {
			return &ptr[0];
		}
		[[nodiscard]] inline constexpr const_pointer begin() const noexcept {
			return &ptr[0];
		}
		[[nodiscard]] inline constexpr reference operator[] (size_type a) noexcept {
			return ptr[a];
		}
		[[nodiscard]] inline constexpr pointer end() noexcept {
			return &ptr[occ_elems];
		}
		[[nodiscard]] inline constexpr const_pointer end() const noexcept {
			return &ptr[occ_elems];
		}
		inline constexpr reference at(const int index) {
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
		inline constexpr reference pop_back() {
			return ptr[--occ_elems];
		}
		inline constexpr void resize(size_type new_size) {
			size_type old_size = ptr_size;
			auto new_ptr = alloc.allocate(ptr_size = new_size);
			for (int i = 0; i < old_size; i++) {
				new_ptr[i] = ptr[i];	
			}
			alloc.deallocate(ptr);
			ptr = new_ptr;
		}
		// TODO: optimize
		inline constexpr void insert(const size_type index, const_reference val) {
			if (++occ_elems >= ptr_size)
				resize(ptr_size * 3);
			for (int i = occ_elems - 1; i > index; i--) {
				ptr[i] = ptr[i - 1];
			}
			ptr[index] = val;
		}

		inline constexpr void push_back(const_reference val) {
			if (++occ_elems >= ptr_size)
				resize(ptr_size * 3);
			ptr[occ_elems - 1] = val;
		}

		// Returns the index of the first instance of the element given. If the element could not be found, returns -1 as a size_type. O(N) time complexity.
		inline constexpr size_type find(const value_type& val) const {
			for (int i = 0; i < occ_elems; i++) {
				if (val == ptr[i])
					return i;
			}
			return (size_type)-1;
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
	template<typename key_type, typename T, typename key_allocator = ::rn::allocator<key_type>, typename val_allocator = ::rn::allocator<T>>
	class dict {
		using value_type = T;
		using pointer = typename val_allocator::pointer;
		using const_pointer = typename val_allocator::const_pointer;
		using reference = T&;
		using const_reference = const T&;
		val_allocator val_alloc;
		key_allocator key_alloc;
		vector<value_type, val_allocator> ptr;
		vector<key_type, key_allocator> keys;
		size_type elem_count;
	public:
		dict() : val_alloc(), key_alloc(), ptr(), keys(), elem_count(1) {}
		inline const size_type size() const noexcept {
			return elem_count;
		}
		inline void add_entry(key_type key, value_type val) {
			keys.push_back(key);
			ptr.push_back(val);
			elem_count++;
		}
		[[nodiscard]] inline constexpr reference operator[] (key_type a) noexcept {
			return ptr[keys.find(a)];
		}
	};
#undef ASSERT_IF_CONST
}
#endif
