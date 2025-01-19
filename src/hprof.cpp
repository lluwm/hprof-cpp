// Copyright (c) 2025 Lei Lu. All rights reserved.

#include "hprof.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

void
Hprof::mmapOpen(const string& path)
{
    _fd = open(path.c_str(), O_RDONLY);
    struct stat fs;
    if (fstat(_fd, &fs) == -1) {
        throw runtime_error("cannot stat file");
    }
    _fSize = fs.st_size;
    _data = reinterpret_cast<char *>(mmap(nullptr, _fSize, PROT_READ, MAP_SHARED, _fd, 0));
}

void
Hprof::mmapClose()
{
    if (!isMmapOpen()) {
        return;
    }

    if (munmap(_data, _fSize) == -1) {
        throw runtime_error("munmap failed when closing file");
    }
    close(_fd);
    initMmapMembers();
}