#ifndef __JSON_UTIL_HPP_20141105__
#define __JSON_UTIL_HPP_20141105__

#include <stdint.h>
#include <string>

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#if defined(_MSC_VER)
#define strtoll _strtoi64
#define strtoull _strtoui64
#endif

namespace jslite {

const size_t NPOS = std::string::npos;

static const unsigned char MASK_BODY = 0x3F;  //00111111
static const unsigned char MASK_1BYTE = 0x80; //10000000
static const unsigned char MASK_2BYTES = 0xC0; //11000000
static const unsigned char MASK_3BYTES = 0xE0; //11100000
static const unsigned char MASK_4BYTES = 0xF0; //11110000
static const unsigned char MASK_5BYTES = 0xF8; //11111000
static const unsigned char MASK_6BYTES = 0xFC; //11111100

inline
uint8_t asc2hex(uint8_t c) { return (c>'9') ? (toupper(c)+10-'A') : c-'0'; }

inline
uint8_t asc2hex(uint8_t h, uint8_t l) { return (asc2hex(h)<<4) | asc2hex(l); }

inline
char hex2asc(char c) { 
	static const char *digits = "0123456789ABCDEF";
	return digits[c & 0xf];
}

inline
std::string UnicodeToUTF8(uint16_t unicode) {
	std::string str;
	if (0x80 > unicode) {
		str.resize(1);
		str[0] = static_cast<char>(unicode);
	} else if (0x800 > unicode) {
		str.resize(2);
		str[0] = static_cast<char>(MASK_2BYTES | (unicode>>6));
		str[1] = static_cast<char>(MASK_1BYTE | (unicode & MASK_BODY));
	} else {
		str.resize(3);
		str[0] = static_cast<char>(MASK_3BYTES | (unicode>>12));
		str[1] = static_cast<char>(MASK_1BYTE | (unicode>>6 & MASK_BODY));
		str[2] = static_cast<char>(MASK_1BYTE | (unicode & MASK_BODY));
	}

	return str;
}

inline
std::string UnicodeToUTF8(uint16_t h, uint16_t  l) {
	return UnicodeToUTF8((h<<8) | l);
}


inline
std::wstring ToUCS2(const std::string& src) {
	std::wstring target;
	
	target.reserve(src.size());

	const char *it = src.c_str();
	const char *end = src.c_str()+src.size();

	for(; it != end; ++it) {
		if (!(0x80 & *it)) { //0xxxxxxx
			target.push_back(*it);
		} else if (!(0x40 & *it)) { //10xxxxx tail_data
			throw std::runtime_error("error not utf8 head byte");
		} else if (!(0x20 & *it)) { //110xxxxx
			if (2 > std::distance(it, end)) throw std::runtime_error("error not utf8 body");//error
			target.push_back((((*it&0x1F)<<6) | (*(it+1)&MASK_BODY)));
			it += 1;
		} else { //if (!(0x10 & *begin)) { //1110xxxx
			if (3 > std::distance(it, end)) throw std::runtime_error("error not utf8 body");//error
			target.push_back(((*it&0xF)<<12) | ((*(it+1)&MASK_BODY) << 6) |
				((*(it+2)&MASK_BODY)));
			it += 2;
		}
	}

	return target;
}


inline
int32_t SizeOfUTF8(const char *begin, const char *end) {
	int32_t len = 0;
	while(begin != end) {
		if (!(0x80 & *begin)) {
			begin += 1;
		} else if (!(0x40 & *begin)) {
			return -1;
		} else if (!(0x20 & *begin)) {
			if (2 > std::distance(begin, end)) return -2;
			begin += 2;
		} else if (!(0x10 & *begin)) {
			if (3 > std::distance(begin, end)) return -2;
			begin += 3;
		} else {
			return -3;
		}
		++len;
	}

	return len;
}

inline
int32_t SizeOfUTF8(const std::string& utf8) {
	return SizeOfUTF8(utf8.c_str(), utf8.c_str()+utf8.size());	
}

inline
bool ValidUTF8(const char *begin, const char *end) {
	for (;begin != end; ++begin) {
		if (!(0x80 & *begin)) { //0xxxxxxx
			switch(*begin) {
			case '"': case '\\': case '/': case '\b': case '\f': case '\n': case '\r': case '\t':
				break;
			default:
				if (0x20 > *begin) return false;
			}
		} else if (!(0x40 & *begin)) { //10xxxxx tail_data
			return false;
		} else if (!(0x20 & *begin)) { //110xxxxx
			if (1 > std::distance(begin, end)) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+1))) return false; //10xxxxxx check
			++begin;
		} else if (!(0x10 & *begin)) { //1110xxxx
			if (2 > std::distance(begin, end)) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+1))) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+2))) return false; //10xxxxxx check
			begin += 2;
		} else if (!(0x8 & *begin)) { //11110xxx
			if (3 > std::distance(begin, end)) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+1))) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+2))) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+3))) return false; //10xxxxxx check
			begin += 3;
		} else if (!(0x4 & *begin)) { //111110xx
			if (4 > std::distance(begin, end)) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+1))) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+2))) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+3))) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+4))) return false; //10xxxxxx check
			begin += 4;
		} else { //1111110x
			if (5 > std::distance(begin, end)) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+1))) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+2))) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+3))) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+4))) return false;
			if (MASK_1BYTE != (MASK_2BYTES & *(begin+5))) return false; //10xxxxxx check
			begin += 5;
		}
	}
	return true;
}

#ifdef WIN32
inline
std::string UnicodeToMultibyte(const std::wstring& src) {
	char *tmp = NULL;
	//949, CP_ACP
	int32_t len = ::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, tmp, 0, NULL, NULL);
	tmp = new char[len+1];
	if (0==::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, tmp, len, NULL, NULL)) return std::string();
	std::string target(tmp);
	delete [] tmp;
	return target;
}
#endif

} //namespace jslite

#endif //__JSON_UTIL_HPP_20141105__