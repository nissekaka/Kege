#include "KakaException.h"
#include <sstream>

namespace Kaka
{
	KakaException::KakaException(const int aLine, const char* aFile) noexcept
		:
		line(aLine),
		file(aFile) {}

	const char* KakaException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl
			<< GetOriginString();
		whatBuffer = oss.str();
		return whatBuffer.c_str();
	}

	const char* KakaException::GetType() const noexcept
	{
		return "Kaka Exception";
	}

	int KakaException::GetLine() const noexcept
	{
		return line;
	}

	const std::string& KakaException::GetFile() const noexcept
	{
		return file;
	}

	std::string KakaException::GetOriginString() const noexcept
	{
		std::ostringstream oss;
		oss << "[File] " << file << std::endl
			<< "[Line] " << line;
		return oss.str();
	}
}
