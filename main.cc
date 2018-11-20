#include <stdio.h>
#include <unistd.h>
#include <unordered_map>

#include "libplatform/libplatform.h"
#include "uv.h"
#include "v8.h"

using v8::Boolean;
using v8::Context;
using v8::Global;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Module;
using v8::NewStringType;
using v8::Promise;
using v8::Script;
using v8::String;
using v8::Value;

v8::Platform* g_platform = nullptr;

#ifndef DCHECK
#define DCHECK(condition) assert(condition)
#endif

#ifndef CHECK
#define CHECK(condition) assert(condition)
#endif

bool ProcessMessages(Isolate* isolate) {
  v8::SealHandleScope shs(isolate);
  while (v8::platform::PumpMessageLoop(g_platform, isolate)) {
    isolate->RunMicrotasks();
  }
  if (g_platform->IdleTasksEnabled(isolate)) {
    v8::platform::RunIdleTasks(g_platform, isolate,
                                1.0 / 1000 /* 1 millisecond */);
  }

  return true;
}

// ---------------------

bool RunMain(Isolate* isolate, std::string js) {
  // Create a stack allocated handle scope.
  v8::HandleScope handle_scope(isolate);

  // Create qa new context.
  Local<Context> context = Context::New(isolate);

  // Enter the context for compiling and running the provided script.
  Context::Scope context_scope(context);

  v8::TryCatch try_catch(isolate);
  try_catch.SetVerbose(true);

  Local<String> source = String::NewFromUtf8(
      isolate, js.c_str(), NewStringType::kNormal, js.size())
      .ToLocalChecked();
  Local<Script> v8_script = Script::Compile(context, source).ToLocalChecked();
  MaybeLocal<Value> maybe_result = v8_script->Run(context);

  Local<Value> result;
  if (!maybe_result.ToLocal(&result)) {
    DCHECK(try_catch.HasCaught());
    fprintf(stderr, "-- an exception occurred.\n");
    return false;
  }

  // Convert the string to a UTF8 string and print it.
  String::Utf8Value utf8(isolate, result);
  printf("Evaluation result: %s\n", *utf8);
  return true;
}

void RunLoop(Isolate* isolate, std::string js) {
  uv_loop_t loop;
  uv_loop_init(&loop);

  RunMain(isolate, js);

  while (uv_loop_alive(&loop)) {
    uv_run(&loop, UV_RUN_ONCE);
    ProcessMessages(isolate);
  }
}


// -------------------



int main(int argc, char** argv) {
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();

  g_platform = platform.get();

  // Create a new Isolate and make it the current one.
  // TODO: isolates go in their own processes.
  Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  Isolate* isolate = Isolate::New(create_params);
  {
    Isolate::Scope isolate_scope(isolate);

    // Create a string with the JavaScript source code.
    std::string js("Promise.resolve(21+21);");
    RunLoop(isolate, js);
  }

  g_platform = nullptr;

  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete create_params.array_buffer_allocator;
  return 0;
}