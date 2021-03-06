{
  'variables': {
    'v8_use_snapshot': 'true',
    # Turn off -Werror in V8
    # See http://codereview.chromium.org/8159015
    'werror': '',
    'target_arch': 'ia32',
    'node_use_dtrace': 'false',
    'node_use_openssl%': 'true',
    'node_use_system_openssl%': 'false',
  },

  'targets': [
    {
      'target_name': 'node',
      'type': 'executable',

      'dependencies': [
        'deps/http_parser/http_parser.gyp:http_parser',
        'deps/uv/uv.gyp:uv',
        'deps/zlib/zlib.gyp:zlib',
      ],

      'include_dirs': [
        'src',
        'deps/uv/src/ares'
      ],

      'sources': [
        'src/main.cc',
        'src/Context.cc',
        'src/NativeModule.cc',
        'src/TryCatch.cc',
        'src/UvCommon.cc',
        
        # Modules
        'src/modules/_debugger.cc',
        'src/modules/_linklist.cc',
        'src/modules/assert.cc',
        'src/modules/buffer.cc',
        'src/modules/buffer_ieee754.cc',
        'src/modules/child_process.cc',
        'src/modules/console.cc',
        'src/modules/constants.cc',
        'src/modules/events.cc',
        'src/modules/fs.cc',
        'src/modules/module.cc',
        'src/modules/net.cc',
        'src/modules/path.cc',
        'src/modules/pipe.cc',
        'src/modules/punycode.cc',
        'src/modules/readline.cc',
        'src/modules/repl.cc',
        'src/modules/stream.cc',
        'src/modules/string_decoder.cc',
        'src/modules/sys.cc',
        'src/modules/timers.cc',
        'src/modules/tty.cc',
        'src/modules/url.cc',
        'src/modules/util.cc',
        'src/modules/vm.cc',
        
        # headers to make for a more pleasant IDE experience
        'src/Context.h',
        'src/gearbox.h',
        'src/NativeModule.h',
        'src/String.h',
        'src/TryCatch.h',
        'src/Value.h',
        
        'deps/http_parser/http_parser.h',
        #'deps/v8/include/v8.h',
        #'deps/v8/include/v8-debug.h',
      ],

      'defines': [
        'ARCH="<(target_arch)"',
        'PLATFORM="<(OS)"',
      ],

      'conditions': [
        [ 'node_use_openssl=="true"', {
          'defines': [ 'HAVE_OPENSSL=1' ],
          #'sources': [ 'src/node_crypto.cc' ],
          'conditions': [
            [ 'node_use_system_openssl=="false"', {
              'dependencies': [ './deps/openssl/openssl.gyp:openssl' ],
            }]]
        }, {
          'defines': [ 'HAVE_OPENSSL=0' ]
        }],

        [ 'node_use_dtrace=="true"', {
          'sources': [
            'src/node_dtrace.cc',
            'src/node_dtrace.h',
            # why does node_provider.h get generated into src and not
            # SHARED_INTERMEDIATE_DIR?
            'src/node_provider.h',
          ],
        }],

        [ 'node_shared_v8=="false"', {
          'dependencies': [ 'deps/v8/tools/gyp/v8.gyp:v8' ],
        }, {
          'libraries': [ '-lv8' ],
        }],

        [ 'OS=="win"', {
          'sources': [
            #'src/platform_win32.cc',
            ## headers to make for a more pleasant IDE experience
            #'src/platform_win32.h',
          ],
          'defines': [
            'FD_SETSIZE=1024',
            # we need to use node's preferred "win32" rather than gyp's preferred "win"
            'PLATFORM="win32"',
          ],
          'libraries': [ '-lpsapi.lib' ]
        },{ # POSIX
          'defines': [ '__POSIX__' ],
          'sources': [
            #'src/node_signal_watcher.cc',
            #'src/node_stat_watcher.cc',
            #'src/node_io_watcher.cc',
          ]
        }],
        [ 'OS=="mac"', {
          #'sources': [ 'src/platform_darwin.cc' ],
          'libraries': [ '-framework Carbon' ],
        }],
        [ 'OS=="linux"', {
          #'sources': [ 'src/platform_linux.cc' ],
          'libraries': [
            '-ldl',
            '-lutil' # needed for openpty
          ],
        }],
        [ 'OS=="freebsd"', {
          #'sources': [ 'src/platform_freebsd.cc' ],
          'libraries': [
            '-lutil',
            '-lkvm',
          ],
        }],
        [ 'OS=="solaris"', {
          #'sources': [ 'src/platform_sunos.cc' ],
          'libraries': [
            '-lkstat',
          ],
        }],
      ],
      'msvs-settings': {
        'VCLinkerTool': {
          'SubSystem': 1, # /subsystem:console
        },
      },
      'cflags_cc': [ '-std=c++0x', '-Wno-unused' ],
    },
  ] # end targets
}

