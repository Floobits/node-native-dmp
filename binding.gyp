{
  'targets': [
    {
      'target_name': 'dmp',
      'sources': [
        'src/diff_match_patch.cpp',
#        'src/diff_match_patch_test.cpp',
        'src/dmp.cc'
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ['OS=="mac"', {
          'include_dirs': [
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
