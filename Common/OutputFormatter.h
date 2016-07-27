#pragma once

#include <vector>
#include <memory>

#include "typedefs.h"
#include "Macros.h"

class FileInfo;

// From the input FileInfo objects list and formatting string, creates serialized into strings (in a given format)
// FileInfos output result.
class OutputFormatter
{
public:
 OutputFormatter(std::vector<const FileInfo*>* const input, std::wstring format_string = std::wstring(L""),
                 int max_files = -1);

 NO_COPY(OutputFormatter)

 std::vector<std::wstring> Format();

private:
 bool DefaultFormat();

 void WriteDefaultFormat();

 bool DateTypeIsSet() const;

 bool DateTimeType() const;

 bool DateTimeDirective() const;

 void ParseDateTime();

 // Returns locale's and timezone specific time.
 std::wstring GetDateTimeStr(uint64 ticks) const;

 bool ParseEscape();

 void WriteCustomUserText();

 bool OtherSupportedDirective() const;

 void WriteDirective();


 static const std::wstring kDefaultTimeFormat;

 static const wchar_t kDefaultDateType;

 static const std::wstring kTimeFields;
 static const std::wstring kDateFields;
 static const std::wstring kDateTypes;
 static const std::wstring kOtherSupportedFields;

 std::wstring format_;
 int output_size_;
 std::wstring::iterator fmt_iter_;

 std::vector<const FileInfo*>* input_;
 std::vector<std::wstring> output_;

 wchar_t date_type_;

 bool intrerpret_symbol_as_directive_;
 bool intrerpret_symbol_as_escape_;
};