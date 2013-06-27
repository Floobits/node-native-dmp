#include <node.h>
#include <node_buffer.h>
#include <v8.h>

#include <QtCore>
#include "diff_match_patch.h"

using namespace v8;

diff_match_patch dmp;

Handle<Value> Method(const Arguments& args) {
  HandleScope scope;
  return scope.Close(String::New("world"));
}

// patch_make (buf1, buf2) --> patch text
// patch_apply (buf, patch_text) --> new buf

Handle<Value> PatchMake(const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments. This function takes 2 arguments.")));
    return scope.Close(Undefined());
  }

  if (!node::Buffer::HasInstance(args[0]) || !node::Buffer::HasInstance(args[1])) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments. Both arguments must be buffers.")));
    return scope.Close(Undefined());
  }

  QString text1 = QString(node::Buffer::Data(args[0]));
  QString text2 = QString(node::Buffer::Data(args[1]));

  QList<Patch> patches = dmp.patch_make(text1, text2);

  QString patch_text = dmp.patch_toText(patches);

  return scope.Close(String::New(patch_text.utf16()));
}

Handle<Value> PatchApply(const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments. This function takes 2 arguments.")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsString() || !node::Buffer::HasInstance(args[1])) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments. Arguments should be a string followed by a buffer.")));
    return scope.Close(Undefined());
  }

  QString patch_text = QString::fromUtf16(*v8::String::Value(args[0]));
  QString source_text = QString(node::Buffer::Data(args[1]));

  QList<Patch> patches = dmp.patch_fromText(patch_text);

  QPair<QString, QVector<bool> > result = dmp.patch_apply(patches, source_text);

  // TODO: this is a SlowBuffer
  return scope.Close(node::Buffer::New(result.first.toUtf8(), result.first.length())->handle_);
}

void init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("patch_make"), FunctionTemplate::New(PatchMake)->GetFunction());
  exports->Set(String::NewSymbol("patch_apply"), FunctionTemplate::New(PatchApply)->GetFunction());
}

NODE_MODULE(dmp, init)
