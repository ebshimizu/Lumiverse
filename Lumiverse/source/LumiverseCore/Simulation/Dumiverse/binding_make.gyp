{
    'variables' : {
        'arnold_include' : 'ARNOLD_PATH_INCLUDE',
        'arnold_lib': 'ARNOLD_PATH_LIB',
    },
    'targets': [
        {
            'target_name' : 'dumiverse_swig',
            'sources': [ 'Dumiverse.cpp', 'Dumiverse_wrap.cxx'],
            'include_dirs': [
                '<(arnold_include)',
            ],
            'libraries': [
                '<(arnold_lib)',
            ],
        },
    ],
}
