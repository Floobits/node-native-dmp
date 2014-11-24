{
  'targets': [
    {
      'target_name': 'dmp',
      'sources': [
        'src/diff_match_patch.cpp',
        'src/dmp.cc'
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ['OS=="mac"', {
          'include_dirs': [
            '/usr/local/include',
            '/usr/local/include/QtCore'
          ],
          'libraries': [
            '/usr/local/lib/QtCore.framework/QtCore'
          ],
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }],
        ['OS=="linux"', {
          'include_dirs': [
            '/usr/local/include',
            '/usr/local/include/QtCore'
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
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ['OS=="mac"', {
          'include_dirs': [
            '/usr/local/include',
            '/usr/local/include/QtCore'
          ],
          'libraries': [
            '/usr/local/lib/QtCore.framework/QtCore'
          ],
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }],
        ['OS=="linux"', {
          'include_dirs': [
            '/usr/local/include',
            '/usr/local/include/QtCore'
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
