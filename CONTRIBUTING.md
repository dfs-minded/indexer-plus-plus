# How to become a contributor and submit your own code

## Contributing A Patch

1. Submit an issue describing your proposed change to the
   [issue tracker](https://github.com/dfs-minded/indexer-plus-plus/issues).
1. Please don't mix more than one logical change per submittal,
   because it makes the history hard to follow. If you want to make a
   change that doesn't have a corresponding issue in the issue
   tracker, please create one.
1. Also, coordinate with the repository owner. This ensures that work isn't being duplicated and
   communicating your plan early also generally leads to better
   patches.
1. Fork the desired repo, develop and test your code changes.
1. Ensure that your code adheres to the existing style in the sample to which
   you are contributing.
1. Ensure that your code has an appropriate set of unit tests which all pass.
1. Submit a pull request.


### Please Be Friendly ###

Showing courtesy and respect to others is a vital part of any project
and company, and I strongly encourage everyone participating in Indexer++
development to join me in accepting nothing less. Of course,
being courteous is not the same as failing to constructively disagree
with each other, but it does mean that we should be respectful of each
other when enumerating the 42 technical reasons that a particular
proposal may not be the best choice. There's never a reason to be
antagonistic or dismissive toward anyone who is sincerely trying to
contribute to a discussion.

Don't forget that coding and developing a great project it's also
a lot of fun. Let's keep it that way. Let's strive to be the best we can
and enrich with our contribution the open source community.

## Style

To keep the source consistent, readable and easy to merge, please follow the coding style guides:

All patches having changes in C++ code will be expected
to conform to the style outlined [in Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).
For changes in C#-written part: [C# Coding Conventions](https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/inside-a-program/coding-conventions) and [Design Guidelines](https://docs.microsoft.com/en-us/dotnet/standard/design-guidelines/index) from Microsoft.



## Requirements for Contributors ###

If you plan to contribute a patch, you need to build Indexer++, which has
further requirements:

  * Visual Studio v17 or newer
  * IndexerGUI project has a dependency on a "Hardcodet.NotifyIcon.Wpf" which can be installed 
	using NuGet package manager in Visual Studio (if it is not installed automatically).

### Testing Indexer++ ###

To make sure your changes work as intended and don't break existing
functionality, you'll want to compile and run Indexer++ tests.

First, go to Common project, CompilerSymb.h file. Find #define SINGLE_THREAD and uncomment it.
Test framework works only for single thread mode.

Build IndexerTest project. Set it as a start-up project. Run it.

All tests should pass.

