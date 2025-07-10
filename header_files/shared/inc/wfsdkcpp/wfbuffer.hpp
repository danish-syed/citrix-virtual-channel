#pragma once

#include <functional>


/// a managed data buffer for the WF SDK. several API functions return allocated memory
/// which needs to be released when no longer needed.
class wfbuffer {
public:

	using ReleaseBufferFn = std::function<void(void*)>;

private:

	LPVOID _data; /// the underlying data
	DWORD _size; /// the size of the data in bytes
	ReleaseBufferFn _deleter;

protected:

	/// constructor.
	wfbuffer(LPVOID data, DWORD size, ReleaseBufferFn deleter = WFFreeMemory)
		: _data(data)
		, _size(size)
		, _deleter(deleter) {
	}

public:

	/// constructor. default constructor.
	wfbuffer(ReleaseBufferFn deleter = WFFreeMemory)
		: _data(nullptr)
		, _size(0)
		, _deleter(deleter) {
	}

	/// constructor. copy construction is now allowed.
	wfbuffer(const wfbuffer&) = delete;

	/// constructor. move constructor.
	wfbuffer(wfbuffer&& other) noexcept
		: _data(std::exchange(other._data, nullptr))
		, _size(std::exchange(other._size, 0))
		, _deleter(std::exchange(other._deleter, WFFreeMemory)) {
	}

	/// factory method for creating new buffer objects by attaching to a data pointer.
	static wfbuffer attach(LPVOID data, DWORD size) {
		return wfbuffer(data, size);
	}

	/// destructor.
	~wfbuffer() {
		if (_data != nullptr) {
			_deleter(_data);
			_data = nullptr;
		}
	}

	/// move assignment operator.
	wfbuffer& operator =(wfbuffer&& other) noexcept {
		std::swap(_data, other._data);
		std::swap(_size, other._size);
		std::swap(_deleter, other._deleter);
		return *this;
	}

	/// LPVOID conversion operator.
	operator LPVOID() { return _data; }
	operator LPBYTE() { return (LPBYTE)_data; }
	operator PCHAR() { return (PCHAR)_data; }

	/// overloaded addressof operator for reading data directly into the buffer.
	LPVOID* operator &() { return &_data; }

	/// obtains access to the size of the buffer. enables reading data directly into the buffer.
	DWORD& mutable_size() { return _size; }

	/// returns the size of the data buffer in bytes.
	DWORD size() const { return _size; }

	/// cast the data pointer to a user-defined type.
	template <typename T>
	T* as() const { return reinterpret_cast<T*>(_data); }
};




/// a fixed capacity user buffer
template <std::size_t N>
class wfuserbuffer : public wfbuffer {
private:

	static constexpr auto user_buf_deleter = [](void* ptr) { delete[] ptr; };

public:

	/// constructor. default constructor.
	wfuserbuffer()
		: wfbuffer(new BYTE[N], 0, user_buf_deleter) {
	}

	/// constructor. move constructor.
	wfuserbuffer(wfuserbuffer&& other) noexcept
		: wfbuffer(std::move(other)) {
	}

	/// move assignment operator.
	wfuserbuffer& operator =(wfuserbuffer&& other) noexcept {
		wfbuffer::operator=(other);
		return *this;
	}

	/// return the fix capacity of the data buffer in bytes.
	DWORD capacity() const { return N; }
};