{
  'targets': [
    {
      'target_name': 'dmp',
      'sources': [
        'src/diff_match_patch.cpp',
        'src/dmp.cc'
      ],
      'cflags': [ '-std=c++11' ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions'],
      'conditions': [
        ['OS=="mac"', {
          'include_dirs': [
            '/usr/local/opt/qt/include',
            '/usr/local/opt/qt/include/QtCore'
          ],
          'libraries': [
            '/usr/local/opt/qt/lib/QtCore.framework/QtCore'
          ],
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'MACOSX_DEPLOYMENT_TARGET': '10.12',
            'OTHER_CPLUSPLUSFLAGS': [ '-std=c++11', '-stdlib=libc++' ],
            'OTHER_LDFLAGS': [ '-stdlib=libc++' ]
          }
        }],
        ['OS=="linux"', {
          'include_dirs': [
            '/usr/local/opt/qt/include',
            '/usr/local/opt/qt/include/QtCore'
          ],
          'cflags': [
            '<!@(pkg-config --cflags QtCore)'
          ],
          'ldflags': [
            '<!@(pkg-config --libs-only-L --libs-only-other QtCore)'
          ],
          'libraries': [
            '<!@(pkg-config --libs-only-l QtCore)'
          ]
        }]
      ]
    },
    {
      'target_name': 'dmp-test',
      'type': 'executable',
      'sources': [
        'src/diff_match_patch_test.cpp',
        'src/diff_match_patch.cpp'
      ],
      'cflags': [ '-std=c++11' ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions'],
      'conditions': [
        ['OS=="mac"', {
          'include_dirs': [
            '/usr/local/opt/qt/include',
            '/usr/local/opt/qt/include/QtCore'
          ],
          'libraries': [
            '/usr/local/opt/qt/lib/QtCore.framework/QtCore'
          ],
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'MACOSX_DEPLOYMENT_TARGET': '10.12',
            'OTHER_CPLUSPLUSFLAGS': [ '-std=c++11', '-stdlib=libc++' ],
            'OTHER_LDFLAGS': [ '-stdlib=libc++' ]
          }
        }],
        ['OS=="linux"', {
          'include_dirs': [
            '/usr/local/opt/qt/include',
            '/usr/local/opt/qt/include/QtCore'
          ],
          'cflags': [
            '<!@(pkg-config --cflags QtCore)'
          ],
          'ldflags': [
            '<!@(pkg-config --libs-only-L --libs-only-other QtCore)'
          ],
          'libraries': [
            '<!@(pkg-config --libs-only-l QtCore)'
          ]
        }]
      ]
    }
  ]
}
