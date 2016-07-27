What is Indexer++ ?
===================

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/hyperium/hyper/master/LICENSE)

Indexer++ is an open-source desktop files and folders search utility. It is a Windows search replacement which is faster, supports entire file system search, filters and more. The Core search engine is written on C++ 11, uses Win API and STL, which ensures high speed and saves memory. Indexer++ directly reads and parses the NTFS  filesystem master files table.

###Documentation
Brief <i class="icon-provider-gdrive"></i>[General Design Doc](https://docs.google.com/document/d/17nXQxh4nTiUfIOtnyCv60XTkxgCZciZvFRkawLz5bb8/edit).


###To build Indexer++ from source
Sources can be built using Visual Studio 13 or later, .NET Framework 4.0. Suggested target platform is x86.

IndexerGUI project has a dependency on a "Hardcodet.NotifyIcon.Wpf" which can be installed using NuGet package manager in Visual Studio.

###To create an installer

Indexer++ installer is written using [NSIS](http://nsis.sourceforge.net/Download). 

 - Build the solution in release configuration
 - Install NSIS software if you do not have it on your machine
 - Run installerPickingUpScript.cmd located in the Project root directory
 
 Generated installer path: ./Installer/Indexer++Installer.exe.
 

###Links
See the [Indexer++ official site](http://indexer-plus-plus.com/) for more information.
For bugs and feature requests relay to https://github.com/dfs-minded/indexer-plus-plus/issues.


