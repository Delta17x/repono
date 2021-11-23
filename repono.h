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
	template<typename _T, std::size_t s>
	class array {
		using value_type = _T;
		using ptr_type = _T*;
	public:
		array() {
			ptr = new value_type[s];
		}
		array(const value_type& first) {
			ptr = new value_type[s];
			ptr[0] = first;
		}
		array(const array<value_type, s>& other) {
			for (int i = 0; i < s; i++) {
				ptr[i] = other.ptr[i];
			}
		}
		array(const std::initializer_list<value_type> a) {
			ptr = new value_type[s];
			int i = 0;
			for (const value_type& iter : a) {
				ptr[i++] = iter;
			}
		}
		inline constexpr array<value_type, s>& operator= (const array<value_type, s>& other) {
			for (int i = 0; i < s; i++) {
				ptr[i] = other.ptr[i];
			}
			return *this;
		}
		~array() {
			delete[] ptr;
		}
		[[nodiscard]] inline constexpr value_type& operator[] (const int a) {
			return ptr[a];
		}
		inline const constexpr std::size_t size() {
			return s;
		}
		[[nodiscard]] inline constexpr const ptr_type begin() {
			return &ptr[0];
		}
		[[nodiscard]] inline constexpr const ptr_type end() {
			return &ptr[s];
		}
	private:
		ptr_type ptr;
		const std::size_t sizet = sizeof(value_type);
	};

	template<typename T>
	class vector {
		using value_type = T;
		using ptr_type = T*;
		ptr_type ptr;
		std::size_t occ_elems;
		std::size_t ptr_size;
	public:
		constexpr vector() : ptr_size(2), occ_elems(1) {
			ptr = new value_type[2];
			ptr[0] = value_type();
		}
		vector(const std::size_t size) : occ_elems(1), ptr_size(size) {
			ptr = new value_type[ptr_size];
			ptr[0] = value_type();
		}
		vector(std::initializer_list<value_type> list) : ptr_size(list.size() + 1), occ_elems(list.size()) {
			ptr = new value_type[ptr_size];
			int i = 0;
			for (const value_type& iter : list) {
				ptr[i++] = iter;
			}
		}
		vector(const vector<value_type>& other) : occ_elems(other.occ_elems), ptr_size(other.ptr_size) {
			ptr = new value_type[ptr_size];			
			for (int i = 0; i < occ_elems; i++) {
				ptr[i] = other.ptr[i];
			}
		}
		vector(vector<value_type>&& other) noexcept : occ_elems(other.occ_elems), ptr_size(other.ptr_size) {
			ptr = new value_type[ptr_size];
			for (int i = 0; i < occ_elems; i++) {
				ptr[i] = other.ptr[i];
			}
			delete[] other.ptr;
			other.ptr = nullptr;
		}
		inline constexpr vector<value_type>& operator=(const vector<value_type>& other) {
			occ_elems = other.occ_elems;
			ptr_size = other.ptr_size;
			delete[] ptr;
			ptr = new value_type[ptr_size];
			for (int i = 0; i < occ_elems; i++) {
				ptr[i] = other.ptr[i];
			}
			return *this;
		}
		~vector() {
			delete[] ptr;
		}
		[[nodiscard]] inline constexpr const ptr_type begin() const noexcept {
			return &ptr[0];
		}
		[[nodiscard]] inline constexpr value_type& operator[] (const int a) noexcept {
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
		inline constexpr const std::size_t size() const noexcept {
			return occ_elems;
		}
		inline constexpr const std::size_t capacity() const noexcept {
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
		inline constexpr void resize(std::size_t new_size) {
			size_t old_size = ptr_size;
			auto new_ptr = new value_type[new_size];
			for (int i = 0; i < old_size; i++) {
				new_ptr[i] = ptr[i];
			}
			delete[] ptr;
			ptr = new_ptr;
		}
		inline constexpr void push_back(const value_type& val) {
			// need to make ptr bigger and then copy ptr_last to ptr
			if (occ_elems >= ptr_size) {
				size_t old_size = ptr_size;
				auto new_ptr = new value_type[ptr_size *= 2];
				for (int i = 0; i < old_size; i++) {
					new_ptr[i] = ptr[i];
				}
				delete[] ptr;
				ptr = new_ptr;
			}
			ptr[occ_elems++] = val;
		}
		inline constexpr const bool operator==(const vector<value_type>& other) const noexcept {
			if (occ_elems != other.occ_elems || ptr_size != other.ptr_size)
				return false;
			for (int i = 0; i < occ_elems; i++)
				if (ptr[i] != other.ptr[i])
					return false;
			return true;
		}
	};
}
#endif
