{
    'targets': [
        {
            'target_name' : 'dumiverse_swig',
            'sources': [ 'Dumiverse.cpp', 'Dumiverse_wrap.cxx'],
            'include_dirs': [
                M_INCLUDE_PATHS
            ],
            'libraries': [
                M_LIBS
            ],
            'cflags_cc!': [ '-fno-exceptions' ],
        },
    ],
}
