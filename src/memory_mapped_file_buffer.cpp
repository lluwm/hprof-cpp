// Copyright (c) 2025 Lei Lu. All rights reserved.

#include "memory_mapped_file_buffer.h"

#include <sys/mman.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <string>

using std::memcpy;
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
    _numOfShards = (_length + _bufferSize - 1) / _bufferSize;
    _shards = new Shard*[_numOfShards];
    long offset = 0;
    for (int i = 0; i < _numOfShards; i++) {
        long size = min(_length - offset, (long)_bufferSize + _padding);
        _shards[i] = new Shard(reinterpret_cast<unsigned char *>(mmap(nullptr, size, PROT_READ, MAP_SHARED, _fd, offset)), size);
        offset += size;
    }
    _currentPosition = 0;
}

MemoryMappedFileBuffer::~MemoryMappedFileBuffer()
{
    if (!isValid()) {
        return;
    }

    for (int i = 0; i < _numOfShards; i++) {
        if (munmap(_shards[i]->getByteBuffers(), _shards[i]->getSize()) == -1) {
            throw runtime_error("munmap failed when closing file");
        }
        delete _shards[i];
    }

    delete[] _shards;
    close(_fd);
    init();
}

void
MemoryMappedFileBuffer::init()
{
    _bufferSize = -1;
    _length = -1;
    _padding = -1;
    _shards = nullptr;
    _currentPosition = -1;
    _fd = -1;
    _numOfShards = -1;
}

unsigned char
MemoryMappedFileBuffer::readByte()
{
    unsigned char res = _shards[getIndex()]->getByteBuffers()[getOffset()];
    _currentPosition++;
    return res;
}

unsigned short
MemoryMappedFileBuffer::readUshort()
{
    unsigned short res =
        *(reinterpret_cast<unsigned short *>(&(_shards[getIndex()]->getByteBuffers()[getOffset()])));
    _currentPosition += 2;
    return __builtin_bswap16(res);
}

unsigned int
MemoryMappedFileBuffer::readUInt()
{
    unsigned int res =
        *(reinterpret_cast<unsigned int *>(&(_shards[getIndex()]->getByteBuffers()[getOffset()])));
    _currentPosition += 4;
    // u4 is in big endian format.
    return __builtin_bswap32(res);
}

unsigned long
MemoryMappedFileBuffer::readULong()
{
    unsigned long res =
        *(reinterpret_cast<unsigned long *>(&(_shards[getIndex()]->getByteBuffers()[getOffset()])));
    _currentPosition += 8;
    return __builtin_bswap64(res);
}

void
MemoryMappedFileBuffer::read(char *dst, int len)
{
    int index = getIndex();
    _shards[index]->position(getOffset());
    if (len <= _shards[index]->remaining()) {
        memcpy(dst, &(_shards[index]->getByteBuffers()[getOffset()]), len);
    } else {
        // Wrapped read
        int split = _bufferSize - _shards[index]->position();
        memcpy(dst, &(_shards[index]->getByteBuffers()[getOffset()]), split);
        _shards[index + 1]->position(0);
        memcpy(dst + split, &(_shards[index+1]->getByteBuffers()[getOffset()]), len - split);
    }
    _currentPosition += len;
}