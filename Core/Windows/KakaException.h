#pragma once
#include <exception>
#include <string>

namespace Kaka
{
	class KakaException : public std::exception
	{
	public:
		KakaException(int aLine, const char* aFile) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		int GetLine() const noexcept;
		const std::string& GetFile() const noexcept;
		std::string GetOriginString() const noexcept;
	private:
		int line;
		std::string file;
	protected:
		mutable std::string whatBuffer;
	};
}
