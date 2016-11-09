import os
from os.path import join, getsize

exclude_dirs_list = ['$RECYCLE.BIN', '.git', 'References', '3dParty', 'packages', '.vs', 'Debug', 'Release', 'obj', 'bin', 'Properties',]

exclude_files_list = ['.cmd', '.md', '.sh', '.exe', '.dll', '.rtf', '.nsi','.sln', '.py', '.suo', '.sdf', '.csproj', '.vcxproj', '.gitignore', '.gitattributes',
    '.ico', '.user', '.opensdf', 'app.manifest', '.xml', '.txt', '.config', '.xaml', '_clang-format', 'LICENSE',
    'ThumbnailProvider.cs', 'VirtualWrapPanel.cs', 'ShortcutResolver.cs', 'VisualTreeSearch.cs', 'ComInterfaces.cs', 'WinApiFunctions.cs', 'WindowsWrapper.h', 'NTFSDataStructures.h',  ]

files_to_licence = []

def printFilenames():
    i = 0
    for f in files_to_licence:
        print "%d %s" % (i, f)
        i += 1;

    #map(lambda pair: print("%d %s" % pair), enumerate(files_to_licence))

def traverseDir(dir):
    for d in exclude_dirs_list:
        if d in dir:
            return False;
    return True;

def includeFileInRes(file):
    for f in exclude_files_list:
        if f in file:
            return False;
    return True;

def getFilesToLicence():
    for root, dirs, files in os.walk('.'):
        dirs[:] = [d for d in dirs if traverseDir(d)]

        for f in filter(includeFileInRes, files):
            files_to_licence.append(join(root, f))

def deleteLicence(lines_num):
    for filename in files_to_licence:
        with open (filename, "r+") as the_file:
            data = the_file.readlines()
            the_file.seek(0)
            data = data[lines_num:]

            for line in data:
                the_file.write(line)

            the_file.truncate()


def insertLicence():
    header_text_filename = "./LicenceHeader.txt"
    header_text = []
    with open (header_text_filename, "r") as license_file:
        header_text = license_file.readlines()

    for filename in files_to_licence:
        with open (filename, "r+") as the_file:
            data = the_file.readlines()
            the_file.seek(0)
            data = header_text + data

            for line in data:
                the_file.write(line)

            the_file.truncate()


getFilesToLicence()
#printFilenames()
insertLicence()
#deleteLicence(4)

