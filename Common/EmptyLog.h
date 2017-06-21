#pragma once

#include "Log.h"
#include "Macros.h"

namespace indexer_common {

class EmptyLog : public Log {
  public:
	static EmptyLog& Instance() {
		static EmptyLog instance;
		return instance;
	};

	NO_COPY(EmptyLog)

	virtual void Debug(const std::wstring& message) override {};

	virtual void Info(const std::wstring& message) override {};

	virtual void Warning(const std::wstring& message) override {};

	virtual void Error(const std::wstring& message) override {};

  private:
	EmptyLog() = default;
	~EmptyLog() = default;
};

} // namespace indexer_common

