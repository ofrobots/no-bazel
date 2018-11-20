#load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository"

new_http_archive(
  name = "icu",	# Reference as @icu//:target-lib
  build_file = "icu.BUILD.bazel",
  sha256 = "547402d50700cd574d4c224af327ef728af75aa27c625b0a864020d3bf86d868",
  urls = ["https://chromium.googlesource.com/chromium/deps/icu.git/+archive/chromium/m70.tar.gz"],
  #strip_prefix = "directory prefix to string from extracted files"
)

# new_http_archive(
#   name = "v8",	# Reference as @v8//:target-lib
#   build_file = "v8.BUILD.bazel",
#   #sha256 = "sha256 hash to verify download"
#   urls = ["https://github.com/v8/v8/archive/7.1.302.22.tar.gz"],
#   #strip_prefix = "directory prefix to string from extracted files"
# )

new_local_repository(
  name = "v8",
  build_file = "v8.BUILD.bazel",
  path = "third_party/v8"
)

new_git_repository(
  name = "libuv",
  remote = "https://github.com/libuv/libuv.git",
  commit = "v1.23.2",
  build_file = "libuv.BUILD.bazel",
)
