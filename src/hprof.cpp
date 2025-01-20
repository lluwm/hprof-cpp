// Copyright (c) 2025 Lei Lu. All rights reserved.

#include "hprof.h"

#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

string
Hprof::readNullTerminatedString()
{
    string ret;
    for (unsigned char c = _buffer.readByte(); c != 0; c = _buffer.readByte()) {
        ret += (char) c;
    }
    return ret;
}

void
Hprof::parse()
{
    string version = readNullTerminatedString();
    cout << version << endl;
}