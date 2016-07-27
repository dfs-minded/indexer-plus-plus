#pragma once

#include <string>

#include "Log.h"
#include "Macros.h"

class OneThreadLog : public Log
{
public:
	static Log& Instance();

	NO_COPY(OneThreadLog)

	virtual void Debug(const std::wstring& message) override;

	virtual void Info(const std::wstring& message) override;

	virtual void Warning(const std::wstring& message) override;

	virtual void Error(const std::wstring& message) override;

private:
	OneThreadLog();

	~OneThreadLog();

	inline void WriteToFile(std::wstring& msg);

	FILE* log_file_;
};

