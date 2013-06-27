#include <node.h>
#include <node_buffer.h>
#include <v8.h>

#include <QtCore>
#include "diff_match_patch.h"

using namespace v8;

Handle<Value> Method(const Arguments& args) {
  HandleScope scope;
  return scope.Close(String::New("world"));
}

// patch_make (buf1, buf2) --> patch text
// patch_apply (buf, patch_text) --> new buf

Handle<Value> PatchMake(const Arguments& args) {
  HandleScope scope;
  diff_match_patch dmp;

  if (args.Length() < 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!node::Buffer::HasInstance(args[0]) || !node::Buffer::HasInstance(args[1])) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments- need buffers")));
    return scope.Close(Undefined());
  }

  QString text1 = QString(node::Buffer::Data(args[0]));
  QString text2 = QString(node::Buffer::Data(args[1]));

  QList<Patch> patches = dmp.patch_make(text1, text2);

  QString patch_text = dmp.patch_toText(patches);

  // TODO: this is a SlowBuffer
  return scope.Close(node::Buffer::New(patch_text.toUtf8(), patch_text.length())->handle_);
}


void init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("patch_make"), FunctionTemplate::New(PatchMake)->GetFunction());
}

NODE_MODULE(dmp, init)
