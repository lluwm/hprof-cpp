// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <string>

class MemoryMappedFileBuffer {
public:
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


private:
    MemoryMappedFileBuffer(const std::string& file, int bufferSize, int padding);
    bool isValid() const { return _fd != -1; }
    void init();

private:
    int             _bufferSize;
    long            _length;
    int             _padding;
    unsigned char **_byteBuffers;
    long            _currentPosition;
    int             _fd;
    int             _shards;
};