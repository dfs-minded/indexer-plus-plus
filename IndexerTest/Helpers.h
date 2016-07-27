#pragma once
// A macros for defining constants of type char16_t*. Todo: remove when Visual Studio will support u"" macros.
#define __L__(str) reinterpret_cast<const char16_t*>(str)