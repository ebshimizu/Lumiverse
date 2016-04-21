#!usr/bin/python

import sys, os, shutil
from subprocess import call
import getopt

"""
node_make.py

@brief Helper script for building dumiverse using node-gyp
@param Path to compiled arnold root directory
@warning Assumes you are using node version 5.9.1. It may work with other versions, I don't know.
@author David Vernet
"""
def main():
    includeKey = 'M_INCLUDE_PATHS'
    libKey = 'M_LIBS'
    includePath = ''
    libPath = ''
    includePath = os.environ.get(includeKey)
    libPath = os.environ.get(libKey)
    if (includePath is None) or (libPath is None):
        usage_string = 'usage: ' + sys.argv[0] + ' -i <semicolon_delimited_include_paths> -l <semicolon_delimited_paths_to_libs>'
        try:
          opts, args = getopt.getopt(sys.argv[1:],"hi:l:",["include_dir=","ailib="])
        except getopt.GetoptError:
          print usage_string
          print 'Note: You can also set environment variables ARNOLD_PATH_INCLUDE and ARNOLD_PATH_LIB instead of passing arguments'
          sys.exit(2)
        for opt, arg in opts:
          if opt == '-h':
             print usage_string
             sys.exit(0)
          elif opt in ("-i", "--includes"):
             includePath = arg
          elif opt in ("-l", "--libs"):
             libPath = arg


    if (includePath is None) or (libPath is None):
        print usage_string
        sys.exit(2)

    print 'include dir is: ' + includePath
    print 'lib path is: ' + libPath


    # Get array of paths and libs by semicolon delimiter
    paths = includePath.split(';')
    libs = libPath.split(';');

    # Populate paths and libs value strings
    pathsValue = ""
    for path in paths:
        pathsValue += "'" + path + "',\n"

    libsValue = ""
    for lib in libs:
        libsValue += "'" + lib + "',\n"

    # Make sure binding make file is present
    bindingMake = "./binding_make.gyp"
    if not os.path.isfile(bindingMake):
        print bindingMake + " file not found. We need this file to build the binding.gyp file for node, so exiting because we can't continue without it."
        sys.exit(1)

    # Delete existing wrap file if necessary (not sure it is)
    wrapPath = "./Dumiverse_wrap.cxx"
    if os.path.isfile(wrapPath):
        os.remove(wrapPath)

    # Make tmp_ass directory if it doesn't exist
    tmpDir = "./tmp_ass"
    if not os.path.isdir(tmpDir):
        os.mkdir(tmpDir)

    # Create binding.gyp file and replace ARNOLD_PATH_REPLACE string
    realBindingFile = "./binding.gyp"
    if os.path.isfile("./binding.gyp"):
        os.remove(realBindingFile)
    shutil.copy(bindingMake, realBindingFile)
    with open(realBindingFile, "wt") as fout:
        with open(bindingMake, "rt") as fin:
            for line in fin:
                if line.count(includeKey) > 0:
                    fout.write(line.replace(includeKey, pathsValue))
                else:
                    fout.write(line.replace(libKey, libsValue))
                
    #  Make actual swig and node-gyp calls to build wrapper C++ code and compile into node module
    call(["swig", "-javascript", "-node", "-c++", "-DV8_VERSION=0x051218", "Dumiverse.i"])
    call(["node-gyp", "clean"])
    call(["node-gyp", "configure"])
    call(["node-gyp", "build"])

if __name__ == "__main__":
    main()
