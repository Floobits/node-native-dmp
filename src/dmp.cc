#include <node.h>
#include <node_buffer.h>
#include <v8.h>

#include <QtCore>
#include "diff_match_patch.h"

using namespace v8;

#include <stdlib.h>

// #define QT_NO_CAST_FROM_ASCII 1

diff_match_patch dmp;

// patch_make (buf1, buf2) --> patch text
// patch_apply (buf, patch_text) --> new buf

Handle<Value> PatchMake(const Arguments& args) {
  HandleScope scope;
  QByteArray bytes1;
  QByteArray bytes2;
  QString text1;
  QString text2;
  QList<Patch> patches;

  if (args.Length() != 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments. This function takes 2 arguments.")));
    return scope.Close(Undefined());
  }

  if (args[0]->IsString() && args[1]->IsString()) {
    text1 = QString::fromUtf16(*String::Value(args[0]));
    qDebug() << "text1" << text1 << text1.length() << "\n";

    text2 = QString::fromUtf16(*String::Value(args[1]));
    qDebug() << "text2" << text2 << text2.length() << "\n";

    if (text1.isNull() || text2.isNull()) {
      ThrowException(Exception::TypeError(String::New("Arguments must be buffers or strings.")));
      return scope.Close(Undefined());
    }

    patches = dmp.patch_make(text1, text2);
  } else if (node::Buffer::HasInstance(args[0]) && node::Buffer::HasInstance(args[1])) {
    bytes1 = QByteArray::fromRawData(node::Buffer::Data(args[0]), node::Buffer::Length(args[0]));
    qDebug() << "bytes1" << bytes1.length() << "\n";

    bytes2 = QByteArray::fromRawData(node::Buffer::Data(args[1]), node::Buffer::Length(args[1]));
    qDebug() << "bytes2" << bytes2.length() <<"\n";

    if (bytes1.isNull() || bytes2.isNull()) {
      ThrowException(Exception::TypeError(String::New("Arguments must be buffers or strings.")));
      return scope.Close(Undefined());
    }
    patches = dmp.patch_make(bytes1, bytes2);
  } else {
    ThrowException(Exception::TypeError(String::New("Arguments must be 2 buffers or 2 strings.")));
    return scope.Close(Undefined());
  }

  QString patch_text = dmp.patch_toText(patches);

  qDebug() << "patch text:" << patch_text << "\n";

  Handle<String> str = String::New(patch_text.utf16());
  qDebug() << "v8 str:" << QString::fromUtf16(*String::Value(str)) << "\n";

  return scope.Close(str);
}

Handle<Value> PatchApply(const Arguments& args) {
  HandleScope scope;
  QByteArray bytes;
  QString text;

  if (args.Length() != 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments. This function takes 2 arguments.")));
    return scope.Close(Undefined());
  }

  if (args[1]->IsString()) {
    text = QString::fromUtf16(*String::Value(args[1]));
    bytes = text.toUtf8();
    qDebug() << "text" << text << text.length() << bytes.length() << "\n";
  } else if (node::Buffer::HasInstance(args[1])) {
    bytes = QByteArray::fromRawData(node::Buffer::Data(args[1]), node::Buffer::Length(args[1]));
    qDebug() << "bytes" << bytes.length() << "\n";
  } else {
    ThrowException(Exception::TypeError(String::New("Wrong arguments. Arguments should be a string followed by a string or buffer.")));
    return scope.Close(Undefined());
  }

  QString patch_text = QString::fromUtf16(*String::Value(args[0]));
  QList<Patch> patches = dmp.patch_fromText(patch_text);

  QPair<QByteArray, QVector<bool> > result = dmp.patch_apply(patches, bytes);

  Handle<Array> arr = Array::New(2);

  Handle<Array> arr2 = Array::New(result.second.size());
  for(int i = 0; i < result.second.size(); i++) {
    arr2->Set(i, Boolean::New(result.second.at(i)));
  }
  arr->Set(1, arr2);

  // TODO: return results of the form ['That quick brown fox jumped over a lazy dog.', [true, true]]
  if (args[1]->IsString()) {
    Handle<String> text = String::New(result.first);
    arr->Set(0, text);
    return scope.Close(arr);
  }

  node::Buffer *slow_buffer = node::Buffer::New(result.first, result.first.length());

  // Turn slow buffer into fast buffer. Taken from http://luismreis.github.io/node-bindings-guide/docs/returning.html
  Local<Object> globalObj = Context::GetCurrent()->Global();
  Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
  Handle<Value> constructorArgs[3] = { slow_buffer->handle_, Integer::New(node::Buffer::Length(slow_buffer)), Integer::New(0) };
  Local<Object> fast_buffer = bufferConstructor->NewInstance(3, constructorArgs);

  arr->Set(0, fast_buffer);

  return scope.Close(arr);
}

Handle<Value> set_Patch_DeleteThreshold(const Arguments& args) {
  HandleScope scope;
  if (args.Length() != 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments. This function takes 1 argument.")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber()) {
    ThrowException(Exception::TypeError(String::New("Wrong argument. This function takes a number.")));
    return scope.Close(Undefined());
  }

  dmp.Patch_DeleteThreshold = args[0]->ToNumber()->Value();

  return scope.Close(Undefined());
}

Handle<Value> set_Match_Threshold(const Arguments& args) {
  HandleScope scope;
  if (args.Length() != 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments. This function takes 1 argument.")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber()) {
    ThrowException(Exception::TypeError(String::New("Wrong argument. This function takes a number.")));
    return scope.Close(Undefined());
  }

  dmp.Match_Threshold = args[0]->ToNumber()->Value();

  return scope.Close(Undefined());
}

Handle<Value> set_Match_Distance(const Arguments& args) {
  HandleScope scope;
  if (args.Length() != 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments. This function takes 1 argument.")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber()) {
    ThrowException(Exception::TypeError(String::New("Wrong argument. This function takes a number.")));
    return scope.Close(Undefined());
  }

  dmp.Match_Distance = args[0]->ToNumber()->Value();

  return scope.Close(Undefined());
}

void init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("patch_make"), FunctionTemplate::New(PatchMake)->GetFunction());
  exports->Set(String::NewSymbol("patch_apply"), FunctionTemplate::New(PatchApply)->GetFunction());

  exports->Set(String::NewSymbol("set_Patch_DeleteThreshold"), FunctionTemplate::New(set_Patch_DeleteThreshold)->GetFunction());
  exports->Set(String::NewSymbol("set_Match_Threshold"), FunctionTemplate::New(set_Match_Threshold)->GetFunction());
  exports->Set(String::NewSymbol("set_Match_Distance"), FunctionTemplate::New(set_Match_Distance)->GetFunction());
}

NODE_MODULE(dmp, init)
