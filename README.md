What is Indexer++ ?
===================

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/hyperium/hyper/master/LICENSE)

Indexer++ is an open-source desktop files and folders search utility. It is a Windows search replacement which is faster, supports entire file system search, filters and more. The Core search engine is written on C++ 11, uses Win API and STL, which ensures high speed and saves memory. Indexer++ directly reads and parses the NTFS  filesystem master files table.


You can search your files using a command-line as well. **ifind** (Indexer++ find) implements a subset of the options of the Linux **find** command, so for Linux users it is easy to start right away. For those, who are not familiar with **find** command <a href="https://github.com/dfs-minded/indexer-plus-plus/blob/master/ifind%20commands%20doc.md" target="_blank">here is options description with some examples</a>.

###Documentation
Brief <i class="icon-provider-gdrive"></i><a href="https://docs.google.com/document/d/17nXQxh4nTiUfIOtnyCv60XTkxgCZciZvFRkawLz5bb8" target="_blank">General Design Doc</a>.


###To build Indexer++ from source
Sources can be built using Visual Studio <b>15</b>, .NET Framework 4.0. Suggested target platform is x86.

IndexerGUI project has a dependency on a "Hardcodet.NotifyIcon.Wpf" which can be installed using NuGet package manager in Visual Studio.

Indexer++ regex feature powered by <a href="https://github.com/google/re2" target="_blank">RE2 regular expression engine</a>.

###Links
See the <a href="http://indexer-plus-plus.com" target="_blank">Indexer++ official site</a> for more information.
For bugs and feature requests relay to <a href="https://github.com/dfs-minded/indexer-plus-plus/issues" target="_blank">https://github.com/dfs-minded/indexer-plus-plus/issues</a>.


