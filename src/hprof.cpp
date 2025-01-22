// Copyright (c) 2025 Lei Lu. All rights reserved.

#include "hprof.h"
#include "stackframe.h"
#include "stacktrace.h"

#include <iostream>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::runtime_error;
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
    _idSizeInBytes = _buffer.readUInt();
    _buffer.readULong(); // Timestamp, ignored.

    while (_buffer.hasRemaining()) {
        int tag = _buffer.readByte();
        _buffer.readUInt(); // TIME, ignored.
        unsigned int recordLen = _buffer.readUInt();
        switch (tag) {
        case kStringInUtf8:
            loadString(recordLen - _idSizeInBytes);
            /* code */
            break;
        case kLoadClass:
            loadClass();
            break;
        case kStackFrame:
            loadStackFrame();
            break;
        case kStackTrace:
            loadStackTrace();
            break;
        case kHeapDump:
        case kHeapDumpSegment:
            loadHeapDump(recordLen);
            break;
        default:
            cout << "unkown: " << std::hex<< tag << endl;
            break;
        }
    }
}

unsigned long
Hprof::readId()
{
    // As long as we don't interpret IDs, reading signed values here is fine.
    switch (_idSizeInBytes) {
    case 1:
        return _buffer.readByte();
    case 2:
        return _buffer.readUshort();
    case 4:
        return _buffer.readUInt();
    case 8:
        return _buffer.readULong();
    }
    throw new runtime_error("ID Length must be 1, 2, 4, or 8");
}

void
Hprof::loadString(int length)
{
    unsigned long id = readId();
    string str = readUTF8(length);
    _strings[id] = str;
}

string
Hprof::readUTF8(int length)
{
    char *temp = new char [length];
    _buffer.read(temp, length);
    string str (temp, length);
    delete []temp;
    return str;
}

void
Hprof::loadClass() {
    unsigned int serial = _buffer.readUInt();
    unsigned long classId = readId();
    _buffer.readUInt(); // Ignored: Stack trace serial number.

    string className = _strings[readId()];
    _classNamesById[classId] = className;
    _classNamesBySerial[serial] =  className;
}

void
Hprof::loadStackFrame()
{
    unsigned long stackFrameId = readId();
    string methodName = _strings[readId()];
    string methodSig = _strings[readId()];
    string sourceFile = _strings[readId()];

    unsigned int classSerial = _buffer.readUInt();
    int lineNumber = _buffer.readUInt();

    string className = _classNamesBySerial[classSerial];
    StackFrame *stackFrame = new StackFrame(stackFrameId, methodName, methodSig, sourceFile, classSerial, lineNumber);
    _stackFrameById[stackFrameId] = stackFrame;
}

void
Hprof::loadStackTrace()
{
    unsigned int serialNumber = _buffer.readUInt();
    unsigned int threadSerialNumber = _buffer.readUInt();
    unsigned int numFrames = _buffer.readUInt();
    
    StackFrame **frames = new StackFrame*[numFrames];
    for (int i = 0; i < numFrames; i++) {
        frames[i] = _stackFrameById[readId()];
    }
    StackTrace *trace = new StackTrace(serialNumber, threadSerialNumber, frames);
}

void
Hprof::loadHeapDump(long len)
{
    while (len > 0) {
        int tag = _buffer.readByte();
        len--;

        switch(tag) {
        case kRootUnknown:
            break;
        default:

        }
    }
}