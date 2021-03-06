#pragma once

#include <nms/core.h>
#include <nms/io/path.h>

struct _iobuf;      // msvc
struct __sFILE;     // GNU

namespace nms
{
template<class T, u32 BookSize, u32 PageSize>
class ArrayList;
}

namespace nms::math
{
template<class T, u32 N>
class Array;
}

namespace nms::io
{

class File: public INocopyable
{
public:
    enum OpenMode
    {
        Read    = 0x0,      /// open for read, file must exists
        Write   = 0x1,      /// open for write, file will trunk.
        Append  = 0x2,      /// open for append
    };

    class Exception: public IException
    {};

    class ENotEnough: public Exception
    {};

    NMS_API File(const Path& path, OpenMode mode);
    NMS_API ~File();

    File(File&& rhs) noexcept
        : fid_(rhs.fid_)
    {
        rhs.fid_ = nullptr;
    }

    File& operator=(File&& rhs) noexcept {
        nms::swap(fid_, rhs.fid_);
        return *this;
    }

    NMS_API u64 size() const;

#pragma region read/write

#pragma region raw
    NMS_API void sync() const;


    u64 read(void* data, u64 size) {
        return readRaw(data, 1, size);
    }

    u64 write(const void* buff, u64 size) {
        return writeRaw(buff, 1, size);
    }

#pragma endregion

#pragma region array/vec
    /* read data */
    template<class T>
    u64 read(T data[], u64 count) {
        auto nread = readRaw(static_cast<void*>(data), sizeof(T), count);
        return nread;
    }

    /* write data */
    template<class T>
    u64 write(const T data[], u64 count) {
        auto nwrite = writeRaw(data, sizeof(T), count);
        return nwrite;
    }

    template<class T, u32 N>
    u64 read(T(&data)[N]) {
        return read(data, N);
    }

    template<class T, u32 N>
    u64 write(const T(&data)[N]) {
        return write(data, N);
    }

#pragma endregion

#pragma region view<T,N>
    /* read view data */
    template<class T, u32 N>
    void read(View<T, N>& view) {
        if (!view.isNormal()) {
            throw EBadType{};
        }

        read(view.data(), view.count());
    }

    /* write view data */
    template<class T, u32 N>
    void write(const View<T, N>& view) {
        if (!view.isNormal()) {
            throw EBadType{};
        }
        write(view.data(), view.count());
    }

#pragma region save/load
    template<class T, class = $when_is<$pod, T>>
    File& operator>>(T& val) {
        read(&val, 1);
        return *this;
    }

    template<class T, class = $when_is<$pod, T>>
    File& operator<<(const T& val) {
        write(&val, 1);
        return *this;
    }

    template<class T, u32 N>
    File& operator>>(Vec<T, N>& v) {
        read(v.data(), N);
        return *this;
    }

    template<class T, u32 N>
    File& operator<<(const Vec<T, N>& v) {
        write(v.data(), N);
        return *this;
    }

    template<class T, u32 N>
    File& operator>>(View<T,N>& v) {
        auto& self = *this;
        self >> v.typeinfo();
        self >> v.size();
        self.read(v.data(), v.count());
        return *this;
    }

    template<class T, u32 N>
    File& operator<<(const View<T, N>& v) {
        auto& self = *this;
        self << v.typeinfo();
        self << v.size();
        self.write(v.data(), v.count());
        return *this;
    }

    template<class T>
    File& load(T& val) {
        *this >> val;
        return *this;
    }

    template<class T>
    File& save(T& val) {
        *this << val;
        return *this;
    }

#pragma endregion

protected:
#ifdef _WIN32
    using fid_t = _iobuf;
#else
    using fid_t = __sFILE;
#endif

    fid_t*  fid_;   // the FILE* object

    NMS_API u64 readRaw (void*       buffer, u64 size, u64 count);
    NMS_API u64 writeRaw(const void* buffer, u64 size, u64 count);
};

template<class T, u32 N>
void load(View<T, N>& view, const Path& path) {
    File file(path, File::Read);
    file.load(view);
}

template<class T, u32 N>
void save(const View<T, N>& view, const Path& path) {
    File file(path, File::Write);
    file.save(view);
}

template<class T, u32 N>
void load(View<T, N>& view, const char* path) {
    File file(cstr(path), File::Read);
    file.load(view);
}

template<class T, u32 N>
void save(const View<T, N>& view, const char* path) {
    File file(cstr(path), File::Write);
    file.save(view);
}

class TxtFile
    : public File
{
protected:
    using base = File;

public:
    NMS_API TxtFile(const Path& path, File::OpenMode mode);
    NMS_API ~TxtFile();

    using base::size;
    using base::sync;

    u64 write(StrView str) {
        return base::write(str.data(), str.count());
    }

    template<class ...U>
    u64 write(StrView fmt, const U& ...args) {
        auto s = format(fmt, args...);
        return write(s);
    }
};

NMS_API String loadString(const Path& path);

}

