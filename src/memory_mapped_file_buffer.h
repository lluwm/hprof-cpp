// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <string>

class MemoryMappedFileBuffer {

public:
    class Shard {
    public:
        explicit Shard(unsigned char *buffer, long size) {
            _byteBuffers = buffer;
            _size = size;
        }

        unsigned char * getByteBuffers() const {
            return _byteBuffers;
        }

        long getSize() const { return _size; }

        void position(long pos) {
            _pos = pos;
        }

        long remaining() const {
            return _size - _pos;
        }

        long position() const { return _pos; }
    private:
        unsigned char   *_byteBuffers;
        long            _size;
        long            _pos;
    };

    // Default chunk size is 1 << 30, or 1GB
    static const int kDefaultSize = 1 << 30;

    // Eliminate wrapped, multi-byte reads across chunks in most cases.
    static const int kDefaultPadding = 1024;

    explicit MemoryMappedFileBuffer() {
        init();
    }
    explicit MemoryMappedFileBuffer(const std::string& file)
        : MemoryMappedFileBuffer(file, kDefaultSize, kDefaultPadding)
    {}

    ~MemoryMappedFileBuffer();

    unsigned char readByte();
    unsigned short readUshort();
    unsigned int readUInt();
    unsigned long readULong();
    void read(char *dst, int len);

    bool hasRemaining() const {
        return _currentPosition < _length;
    }

    long remaining() const {
        return _length - _currentPosition;
    }

    void setCurrentPosition(long val) {
        _currentPosition = val;
    }

    long getCurrentPosition() const {
        return _currentPosition;
    }

private:
    MemoryMappedFileBuffer(const std::string& file, int bufferSize, int padding);
    void init();

    bool isValid() const { return _fd != -1; }
    int getIndex() const {
        return (int) (_currentPosition / _bufferSize);
    }

    int getOffset() const {
        return (int) (_currentPosition % _bufferSize);
    }

private:
    int     _bufferSize;
    long    _length;
    int     _padding;
    Shard   **_shards;
    long    _currentPosition;
    int     _fd;
    int     _numOfShards;
};