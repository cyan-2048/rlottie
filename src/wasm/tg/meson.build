source_file =  files(
  'utils/allocator.c',
	'libdeflate/allocator.c',
	'libdeflate/deflate_compress.c',
	'libdeflate/deflate_decompress.c',
	'libdeflate/gzip_decompress.c',
	'libdeflate/zlib_compress.c',
	'libdeflate/adler32.c',
	'crypto/aes256.c',
	'crypto/ige256.c',
	'crypto/ctr256.c',
	'hash/sha256.c',
	'hash/sha1.c'
)


mtcute_wasm_dep = declare_dependency(include_directories
                                          : include_directories('.'), sources
                                          : source_file)