load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "icu",	# Reference as @icu//:target-lib
  build_file = "//:icu.BUILD.bazel",
  sha256 = "547402d50700cd574d4c224af327ef728af75aa27c625b0a864020d3bf86d868",
  urls = ["https://chromium.googlesource.com/chromium/deps/icu.git/+archive/chromium/m70.tar.gz"],
  #strip_prefix = "directory prefix to string from extracted files"
)

new_local_repository(
  name = "v8",
  build_file = "//:v8.BUILD.bazel",
  path = "third_party/v8"
)

http_archive(
  name = "libuv",
  url = "https://github.com/libuv/libuv/archive/v1.23.2.tar.gz",
  sha256 = "30af979c4f4b8d1b895ae6d115f7400c751542ccb9e656350fc89fda08d4eabd",
  strip_prefix = "libuv-1.23.2",
  build_file = "//:libuv.BUILD.bazel",
)
