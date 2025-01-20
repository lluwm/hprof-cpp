// Copyright (c) 2025 Lei Lu. All rights reserved.

#include "memory_mapped_file_buffer.h"

#include <sys/mman.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <stdexcept>
#include <string>

using std::min;
using std::string;
using std::runtime_error;

MemoryMappedFileBuffer::MemoryMappedFileBuffer(const string& file, int bufferSize, int padding)
{
    _bufferSize = bufferSize;
    _padding = padding;
    _fd = open(file.c_str(), O_RDONLY);
    if (_fd == -1) {
        throw runtime_error("cannot open file: " + file);
    }
    struct stat fs;
    if (fstat(_fd, &fs) == -1) {
        throw runtime_error("cannot stat file: " + file);
    }
    _length = fs.st_size;
    _shards = (_length + _bufferSize - 1) / _bufferSize;
    _byteBuffers = new unsigned char *[_shards];
    long offset = 0;
    for (int i = 0; i < _shards; i++) {
        long size = min(_length - offset, (long)_bufferSize + _padding);
        _byteBuffers[i] = reinterpret_cast<unsigned char *>(mmap(nullptr, size, PROT_READ, MAP_SHARED, _fd, offset));
        offset += size;
    }
    _currentPosition = 0;
}

MemoryMappedFileBuffer::~MemoryMappedFileBuffer()
{
    if (!isValid()) {
        return;
    }

    long offset = 0;
    for (int i = 0; i < _shards; i++) {
        long size = min(_length - offset, (long)_bufferSize + _padding);
        if (munmap(_byteBuffers[i], size) == -1) {
            throw runtime_error("munmap failed when closing file");
        }
        offset += size;
    }

    delete[] _byteBuffers;
    close(_fd);
    init();
}

void
MemoryMappedFileBuffer::init()
{
    _bufferSize = -1;
    _length = -1;
    _padding = -1;
    _byteBuffers = nullptr;
    _currentPosition = -1;
    _fd = -1;
    _shards = -1;
}