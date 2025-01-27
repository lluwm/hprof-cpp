// Copyright (c) 2025 Lei Lu. All rights reserved.

#include "array_instance.h"
#include "classobj.h"
#include "class_instance.h"
#include "field.h"
#include "hprof.h"
#include "rootobj.h"
#include "stackframe.h"
#include "stacktrace.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using std::cout;
using std::endl;
using std::make_shared;
using std::runtime_error;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

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
    updateObjectTypeSize(_idSizeInBytes);
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
            skipFully(recordLen);
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
    _snapshot.addStackFrame(make_shared<StackFrame>(stackFrameId, methodName, methodSig, sourceFile, classSerial, lineNumber));
}

void
Hprof::loadStackTrace()
{
    unsigned int serialNumber = _buffer.readUInt();
    unsigned int threadSerialNumber = _buffer.readUInt();
    unsigned int numFrames = _buffer.readUInt();
    
    vector<shared_ptr<StackFrame>> frames(numFrames);
    for (int i = 0; i < numFrames; i++) {
        frames.push_back(_snapshot.getStackFrame(readId()));
    }
    _snapshot.addStackTrace(make_shared<StackTrace>(serialNumber, threadSerialNumber, std::move(frames)));
}

void
Hprof::loadHeapDump(long len)
{
    while (len > 0) {
        int tag = _buffer.readByte();
        len--;

        switch(tag) {
        case kRootUnknown:
            len -= loadBasicObj(RootType::kUnknown);
            break;
        case kRootJniGlobal:
            len -= loadBasicObj(RootType::kNativeStatic);
            readId(); // ignored.
            len -= _idSizeInBytes;
            break;
        case kRootJniLocal:
            len -= loadJniLocal();
            break;
        case kRootJavaFrame:
            len -= loadJavaFrame();
            break;
        case kRootNativeStack:
            len -= loadNativeStack();
            break;
        case kRootStickyClass:
            len -= loadBasicObj(RootType::kSystemClass);
            break;
        case kRootThreadBlock:
            len -= loadThreadBlock();
            break;
        case kRootMonitorUsed:
            len -= loadBasicObj(RootType::kBusyMonitor);
            break;
        case kRootThreadObject:
            len -= loadThreadObject();
            break;
        case kClassDump:
            len -= loadClassDump();
            break;
        case kInstanceDump:
            len -= loadInstanceDump();
            break;
        case kObjectArrayDump:
            len -= loadObjectArrayDump();
            break;
        case kPrimitiveArrayDump:
            len -= loadPrimitiveArrayDump();
            break;
        default:
            throw runtime_error("unknown tag: " + to_string(tag));
        }
    }
}

int
Hprof::loadBasicObj(RootType type)
{
    unsigned long id = readId();
    _snapshot.addRoot(make_shared<RootObj>(type, id));
    return _idSizeInBytes;
}


/*
 * ROOT JNI LOCAL:
 *
 * ID - object ID
 * u4 - thread serial number
 * u4 - frame number in stack trace (-1 for empty)
 */

int
Hprof::loadJniLocal()
{
    unsigned long id = readId();
    unsigned int threadSerialNum = _buffer.readUInt();
    unsigned int stackFrameNum = _buffer.readUInt();
    shared_ptr<Thread> thread = _snapshot.getThread(threadSerialNum);
    shared_ptr<StackTrace> trace = _snapshot.getStackTraceAtDepth(threadSerialNum, stackFrameNum);
    _snapshot.addRoot(make_shared<RootObj>(RootType::kNativeLocal, id, threadSerialNum, trace));
    return _idSizeInBytes + 4 * 2;
}


/*
 * ROOT JAVA FRAME:
 *
 * ID - object ID
 * u4 - thread serial number
 * u4 - frame number in stack trace (-1 for empty)
 */

int
Hprof::loadJavaFrame()
{
    unsigned long id = readId();
    unsigned int threadSerialNum = _buffer.readUInt();
    unsigned int stackFrameNum = _buffer.readUInt();
    shared_ptr<Thread> thread = _snapshot.getThread(threadSerialNum);
    shared_ptr<StackTrace> trace = _snapshot.getStackTraceAtDepth(threadSerialNum, stackFrameNum);
    _snapshot.addRoot(make_shared<RootObj>(RootType::kJavaLocal, id, threadSerialNum, trace));
    return _idSizeInBytes + 4 * 2;
}


/*
 * ROOT NATIVE STACK:
 *
 * ID - object ID
 * u4 - thread serial number
 */

int
Hprof::loadNativeStack()
{
    unsigned long id = readId();
    unsigned int threadSerialNum = _buffer.readUInt();
    shared_ptr<Thread> thread = _snapshot.getThread(threadSerialNum);
    shared_ptr<StackTrace> trace = _snapshot.getStackTrace(thread->getStackSerialNum());
    _snapshot.addRoot(make_shared<RootObj>(RootType::kNativeStack, id, threadSerialNum, trace));
    return _idSizeInBytes + 4;
}


/*
 * ROOT THREAD BLOCK:
 *
 * ID - object ID
 * u4 - thread serial number
 */

int
Hprof::loadThreadBlock()
{
    unsigned long id = readId();
    unsigned int threadSerialNum = _buffer.readUInt();
    shared_ptr<Thread> thread = _snapshot.getThread(threadSerialNum);
    shared_ptr<StackTrace> trace = _snapshot.getStackTrace(thread->getStackSerialNum());
    _snapshot.addRoot(make_shared<RootObj>(RootType::kThreadBlock, id, threadSerialNum, trace));
    return _idSizeInBytes + 4;
}


/*
 * ROOT THREAD OBJECT:
 *
 * ID - thread object ID
 * u4 - thread serial number
 * u4 - stack trace serial number
 */

int
Hprof::loadThreadObject()
{
    unsigned long id = readId();
    unsigned int threadSerialNumber = _buffer.readUInt();
    unsigned int stackSerialNumber = _buffer.readUInt();

    shared_ptr<Thread> thread = make_shared<Thread>(id, stackSerialNumber);
    shared_ptr<StackTrace> stack = _snapshot.getStackTrace(stackSerialNumber);

    _snapshot.addRoot(make_shared<RootObj>(RootType::kThreadObject, id, threadSerialNumber, stack));
    _snapshot.addThread(threadSerialNumber, thread);
    return _idSizeInBytes + 4 * 2;
}


/*
 * CLASS DUMP:
 *
 * ID - class object ID
 * u4 - stack trace serial number
 * ID - super class object ID
 * ID - class loader object ID
 * ID - signers object ID
 * ID - protection domain object ID
 * ID - reserved
 * ID - reserved
 * u4 - instance size (in bytes)
 */

int
Hprof::loadClassDump()
{
    unsigned long id = readId();
    unsigned int stackSerialNumber = _buffer.readUInt();
    shared_ptr<StackTrace> stack = _snapshot.getStackTrace(stackSerialNumber);

    unsigned long superClassId = readId();
    unsigned long classLoaderId = readId();

    readId(); // Ignored: Signeres ID.
    readId(); // Ignored: Protection domain ID.
    readId(); // RESERVED.
    readId(); // RESERVED.

    unsigned int instanceSize = _buffer.readUInt();

    int bytesRead = _idSizeInBytes * 7 + 4 * 2;

    //  Skip over the constant pool.
    unsigned short numEntries = _buffer.readUshort();
    bytesRead += 2;
    for (unsigned short i = 0; i < numEntries; i++) {
        _buffer.readUshort();
        bytesRead += 2 + skipValue();
    }

    string className = _classNamesById[id];
    shared_ptr<ClassObj> theClass = make_shared<ClassObj>(id, stack, className, _buffer.getCurrentPosition());
    theClass->setSuperClassId(superClassId);

    //  Skip over static fields.
    numEntries = _buffer.readUshort();
    bytesRead += 2;

    vector<shared_ptr<Field>> staticFields(numEntries);

    for (unsigned short i = 0; i < numEntries; i++) {
        string name = _strings[readId()];
        Type type = static_cast<Type>(_buffer.readByte());
        staticFields.push_back(make_shared<Field>(type, name));
        skipFully(getTypeSize(type));
        bytesRead += _idSizeInBytes + 1 +getTypeSize(type);
    }
    theClass->setStaticFields(std::move(staticFields));

    //  Instance fields
    numEntries = _buffer.readUshort();
    bytesRead += 2;
    vector<shared_ptr<Field>> fields(numEntries);
    for (unsigned short i = 0; i < numEntries; i++) {
        string name = _strings[readId()];
        Type type = static_cast<Type>(_buffer.readByte());
        fields.push_back(make_shared<Field>(type, name));
        bytesRead += _idSizeInBytes + 1;
    }

    theClass->setFields(fields);
    theClass->setInstanceSize(instanceSize);

    _snapshot.addClass(id, theClass);
    return bytesRead;
}


int
Hprof::skipValue() {
    int type = _buffer.readByte();
    int size = getTypeSize(static_cast<Type>(type));
    skipFully(size);
    return size + 1;
}


int
Hprof::getTypeSize(Type type) 
{
    return kTypeSize[type];
}

void
Hprof::updateObjectTypeSize(int size) const
{
    kTypeSize[Type::kObject] = size;
}


void
Hprof::skipFully(long numBytes) {
    _buffer.setCurrentPosition(_buffer.getCurrentPosition() + numBytes);
}


int
Hprof::loadInstanceDump()
{
    unsigned long id = readId();
    unsigned int stackId = _buffer.readUInt();
    shared_ptr<StackTrace> stack = _snapshot.getStackTrace(stackId);

    unsigned long classId = readId();
    unsigned int remaining = _buffer.readUInt();
    long pos = _buffer.getCurrentPosition();

    shared_ptr<Instance> instance = make_shared<ClassInstance>(id, stack, pos);
    instance->setClassId(classId);
    _snapshot.addInstance(id, instance);

    skipFully(remaining);
    return _idSizeInBytes * 2 + 4 + remaining;
}

int
Hprof::loadObjectArrayDump()
{
    unsigned long id = readId();
    unsigned int stackId = _buffer.readUInt();
    shared_ptr<StackTrace> stack = _snapshot.getStackTrace(stackId);

    unsigned int numElements = _buffer.readUInt();
    unsigned long classId = readId();

    shared_ptr<Instance> array = make_shared<ArrayInstance>(id, stack, Type::kObject,
                                                                 numElements, _buffer.getCurrentPosition());
    array->setClassId(classId);
    _snapshot.addInstance(id, array);
    
    int remaining = numElements * _idSizeInBytes;
    skipFully(remaining);
    return _idSizeInBytes + 4 + 4 + _idSizeInBytes + remaining;
}


int
Hprof::loadPrimitiveArrayDump()
{
    unsigned long id = readId();
    unsigned int stackId = _buffer.readUInt();
    shared_ptr<StackTrace> stack = _snapshot.getStackTrace(stackId);

    unsigned int numElements = _buffer.readUInt();
    Type type = static_cast<Type>(_buffer.readByte());
    int size = getTypeSize(type);

    shared_ptr<Instance> array = make_shared<ArrayInstance>(id, stack, type, numElements, _buffer.getCurrentPosition());

    _snapshot.addInstance(id, array);

    int remaining = numElements * size;
    skipFully(remaining);
    return _idSizeInBytes + 4 + 4 + 1 + remaining; 
}