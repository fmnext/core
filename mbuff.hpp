#pragma once
#include <ostream>
#include <vector>

struct mbuff : std::streambuf
{
	mbuff(char* begin, char* end) : begin(begin), end(end + 1) {
		this->setg(begin, begin, end);
		this->setp(begin, begin, end);
	}

	mbuff(std::vector<uint8_t>& buffer) : begin(reinterpret_cast<char*>(&buffer.front())), end(reinterpret_cast<char*>(&buffer.back() + 1)) {
		this->setg(begin, begin, end);
		this->setp(begin, begin, end);
	}

	mbuff(std::vector<char>& buffer) : begin(reinterpret_cast<char*>(&buffer.front())), end(reinterpret_cast<char*>(&buffer.back() + 1)) {
		this->setg(begin, begin, end);
		this->setp(begin, begin, end);
	}
	
	virtual pos_type seekoff(std::streambuf::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in) override {
		switch (dir)
		{
		case std::ios_base::cur:
			std::streambuf::gbump(static_cast<int>(off));
			break;

		case std::ios_base::end:
			std::streambuf::setg(begin, end + static_cast<int>(off), end);
			break;

		case std::ios_base::beg:
			std::streambuf::setg(begin, begin + static_cast<int>(off), end);
			break;
		}

		return std::streambuf::gptr() - std::streambuf::eback();
	}

	virtual pos_type seekpos(std::streampos pos, std::ios_base::openmode mode) override {
		return seekoff(pos - pos_type(std::streambuf::off_type(0)), std::ios_base::beg, mode);
	}

	char* begin;
	char* end;
};