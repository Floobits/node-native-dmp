#include <node.h>
#include <node_buffer.h>
#include <v8.h>

#include <QtCore>
#define QT_NO_DEBUG_OUTPUT

#include "diff_match_patch.h"
diff_match_patch dmp;

using namespace v8;


void PatchMake(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  QByteArray bytes1;
  QByteArray bytes2;
  QString text1;
  QString text2;
  QList<Patch> patches;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments. This function takes 2 arguments.")));
    args.GetReturnValue().Set(Undefined(isolate));
    return;
  }

  if (args[0]->IsString() && args[1]->IsString()) {
    text1 = QString::fromUtf16(*String::Value(args[0]));
    // qDebug() << "text1" << text1 << text1.length() << "\n";

    text2 = QString::fromUtf16(*String::Value(args[1]));
    // qDebug() << "text2" << text2 << text2.length() << "\n";

    if (text1.isNull() || text2.isNull()) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Arguments must be buffers or strings.")));
      args.GetReturnValue().Set(Undefined(isolate));
      return;
    }

    patches = dmp.patch_make(text1, text2);
  } else if (node::Buffer::HasInstance(args[0]) && node::Buffer::HasInstance(args[1])) {
    bytes1 = QByteArray::fromRawData(node::Buffer::Data(args[0]), node::Buffer::Length(args[0]));
    // qDebug() << "bytes1" << bytes1.length() << "\n";

    bytes2 = QByteArray::fromRawData(node::Buffer::Data(args[1]), node::Buffer::Length(args[1]));
    // qDebug() << "bytes2" << bytes2.length() <<"\n";

    if (bytes1.isNull() || bytes2.isNull()) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Arguments must be buffers or strings.")));
      args.GetReturnValue().Set(Undefined(isolate));
      return;
    }
    patches = dmp.patch_make(bytes1, bytes2);
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Arguments must be 2 buffers or 2 strings.")));
    args.GetReturnValue().Set(Undefined(isolate));
    return;
  }

  QString patch_text = dmp.patch_toText(patches);

  // qDebug() << "patch text:" << patch_text << "\n";

  Handle<String> str = String::NewFromUtf8(isolate, patch_text.toUtf8());
  // qDebug() << "v8 str:" << QString::fromUtf16(*String::Value(str)) << "\n";

  args.GetReturnValue().Set(str);
}

void PatchApply(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  QByteArray bytes;
  QString text;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments. This function takes 2 arguments.")));
    args.GetReturnValue().Set(Undefined(isolate));
    return;
  }

  if (args[1]->IsString()) {
    text = QString::fromUtf16(*String::Value(args[1]));
    bytes = text.toUtf8();
    // qDebug() << "text" << text << text.length() << bytes.length() << "\n";
  } else if (node::Buffer::HasInstance(args[1])) {
    bytes = QByteArray::fromRawData(node::Buffer::Data(args[1]), node::Buffer::Length(args[1]));
    // qDebug() << "bytes" << bytes.length() << "\n";
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments. Arguments should be a string followed by a string or buffer.")));
    args.GetReturnValue().Set(Undefined(isolate));
    return;
  }

  QString patch_text = QString::fromUtf16(*String::Value(args[0]));
  QList<Patch> patches = dmp.patch_fromText(patch_text);

  QPair<QByteArray, QVector<bool> > result = dmp.patch_apply(patches, bytes);

  Handle<Array> arr = Array::New(isolate, 2);

  Handle<Array> arr2 = Array::New(isolate, result.second.size());
  for(int i = 0; i < result.second.size(); i++) {
    arr2->Set(i, Boolean::New(isolate, result.second.at(i)));
  }
  arr->Set(1, arr2);

  // TODO: return results of the form ['That quick brown fox jumped over a lazy dog.', [true, true]]
  if (args[1]->IsString()) {
    Handle<String> text = String::NewFromUtf8(isolate, result.first);
    arr->Set(0, text);
    args.GetReturnValue().Set(arr);
    return;
  }

  MaybeLocal<Object> mb = node::Buffer::Copy(isolate, result.first, result.first.length());
  Local<Object> buf;
  if (!mb.ToLocal(&buf)) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Unknown error in MaybeLocal::ToLocal()")));
  }
  arr->Set(0, buf);
  args.GetReturnValue().Set(arr);
  return;
}

void set_Patch_DeleteThreshold(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments. This function takes 1 argument.")));
    args.GetReturnValue().Set(Undefined(isolate));
    return;
  }

  if (!args[0]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong argument. This function takes a number.")));
    args.GetReturnValue().Set(Undefined(isolate));
    return;
  }

  dmp.Patch_DeleteThreshold = args[0]->ToNumber(isolate)->Value();
  args.GetReturnValue().Set(Undefined(isolate));
}

void set_Match_Threshold(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments. This function takes 1 argument.")));
    args.GetReturnValue().Set(Undefined(isolate));
    return;
  }

  if (!args[0]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong argument. This function takes a number.")));
    args.GetReturnValue().Set(Undefined(isolate));
    return;
  }

  dmp.Match_Threshold = args[0]->ToNumber(isolate)->Value();
  args.GetReturnValue().Set(Undefined(isolate));
  return;
}

void set_Match_Distance(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments. This function takes 1 argument.")));
    args.GetReturnValue().Set(Undefined(isolate));
    return;
  }

  if (!args[0]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong argument. This function takes a number.")));
    args.GetReturnValue().Set(Undefined(isolate));
    return;
  }

  dmp.Match_Distance = args[0]->ToNumber(isolate)->Value();
  args.GetReturnValue().Set(Undefined(isolate));
}

void init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "patch_make", PatchMake);
  NODE_SET_METHOD(exports, "patch_apply", PatchApply);

  NODE_SET_METHOD(exports, "set_Patch_DeleteThreshold", set_Patch_DeleteThreshold);
  NODE_SET_METHOD(exports, "set_Match_Threshold", set_Match_Threshold);
  NODE_SET_METHOD(exports, "set_Match_Distance", set_Match_Distance);
}

NODE_MODULE(dmp, init)
