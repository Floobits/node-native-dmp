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

  if (args.Length() < 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments. This function takes 2 arguments.")));
    return scope.Close(Undefined());
  }

  if (args[0]->IsString()) {
    text1 = QString::fromUtf16(*String::Value(args[0]));
    qDebug() << "text1" << text1 << text1.length() << "\n";
  }
  if (args[1]->IsString()) {
    text2 = QString::fromUtf16(*String::Value(args[1]));
    qDebug() << "text2" << text2 << text2.length() << "\n";
  }

  if (node::Buffer::HasInstance(args[0])) {
    bytes1 = QByteArray::fromRawData(node::Buffer::Data(args[0]), node::Buffer::Length(args[0]));
    text1 = QString(bytes1);
    qDebug() << "text1" << text1 << text1.length() << bytes1.length() << "\n";
  }

  if (node::Buffer::HasInstance(args[1])) {
    bytes2 = QByteArray::fromRawData(node::Buffer::Data(args[1]), node::Buffer::Length(args[1]));
    text2 = QString(bytes2);
    qDebug() << "text2" << text2 << text2.length() << bytes2.length() <<"\n";
  }

  if (text1.isNull() || text2.isNull()) {
    ThrowException(Exception::TypeError(String::New("Arguments must be buffers or strings.")));
    return scope.Close(Undefined());
  }

  QList<Patch> patches = dmp.patch_make(text1, text2);

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

  if (args.Length() < 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments. This function takes 2 arguments.")));
    return scope.Close(Undefined());
  }

  if (args[1]->IsString()) {
    text = QString::fromUtf16(*String::Value(args[1]));
    qDebug() << "text" << text << text.length() << "\n";
  }
  if (node::Buffer::HasInstance(args[1])) {
    bytes = QByteArray::fromRawData(node::Buffer::Data(args[1]), node::Buffer::Length(args[1]));
    text = QString(bytes);
    qDebug() << "text" << text << text.length() << bytes.length() <<"\n";
  }

  if (!args[0]->IsString() || text.isNull()) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments. Arguments should be a string followed by a string or buffer.")));
    return scope.Close(Undefined());
  }

  QString patch_text = QString::fromUtf16(*v8::String::Value(args[0]));
  QList<Patch> patches = dmp.patch_fromText(patch_text);

  QPair<QString, QVector<bool> > result = dmp.patch_apply(patches, text);

  // TODO: return results of the form ['That quick brown fox jumped over a lazy dog.', [true, true]]
  if (args[1]->IsString()) {
    return scope.Close(String::New(result.first.utf16()));
  }

  node::Buffer *slow_buffer = node::Buffer::New(result.first.toUtf8(), result.first.length());

  // Turn slow buffer into fast buffer. Taken from http://luismreis.github.io/node-bindings-guide/docs/returning.html
  Local<Object> globalObj = Context::GetCurrent()->Global();
  Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
  Handle<Value> constructorArgs[3] = { slow_buffer->handle_, v8::Integer::New(node::Buffer::Length(slow_buffer)), v8::Integer::New(0) };
  Local<Object> fast_buffer = bufferConstructor->NewInstance(3, constructorArgs);

  return scope.Close(fast_buffer);
}

void init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("patch_make"), FunctionTemplate::New(PatchMake)->GetFunction());
  exports->Set(String::NewSymbol("patch_apply"), FunctionTemplate::New(PatchApply)->GetFunction());
}

NODE_MODULE(dmp, init)
