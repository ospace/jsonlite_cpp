#include "json_tokenizer.hpp"
#include "json_util.hpp"

#include <sstream>

namespace jslite {

static const char* type_name(JsonTokenzier::TokenType type) {
	switch(type) {
	case JsonTokenzier::TK_NONE:        return "NONE";
	case JsonTokenzier::TK_WRONG:       return "WRONG";
	case JsonTokenzier::TK_STRING:      return "STRING";
	case JsonTokenzier::TK_INTEGER:     return "INTEGER";
	case JsonTokenzier::TK_REAL:        return "REAL";
	case JsonTokenzier::TK_TRUE:        return "TRUE";
	case JsonTokenzier::TK_FALSE:       return "FALSE";
	case JsonTokenzier::TK_NULL:        return "NULL";
	case JsonTokenzier::TK_COLON:       return "COLON";
	case JsonTokenzier::TK_COMMA:       return "COMMA";
	case JsonTokenzier::TK_OBJ_BEGIN:   return "OBJECT BEGIN";
	case JsonTokenzier::TK_OBJ_END:     return "OBJECT END";
	case JsonTokenzier::TK_ARR_BEGIN:   return "ARRAY BEGIN";
	case JsonTokenzier::TK_ARR_END:     return "ARRAY END";
	case JsonTokenzier::TK_COMMENT_C:   return "COMMENT C STYLE";
	case JsonTokenzier::TK_COMMENT_CPP: return "COMMENT CPP STYLE";
	case JsonTokenzier::TK_EOF:         return "EOF";
	}

	return "Unknown";
}


JsonTokenzier::JsonTokenzier(const char *begin, const char *end) : begin_(begin), end_(end), it_(begin) {}

char JsonTokenzier::Current() {
	return (IsDone() ? 0 : *it_);
}

char JsonTokenzier::Next() {
	return (IsDone() ? 0 : *++it_);
}

char JsonTokenzier::Bump() {
	return (IsDone() ? 0 : *it_++);
}

bool JsonTokenzier::IsDone() {
	return it_ == end_;
}

const JsonTokenzier::Token& JsonTokenzier::CurrentToken() {
	return token_;
}

const JsonTokenzier::Token& JsonTokenzier::SkipCommentAndNextToken() {
    do {
        NextToken();
    } while(JsonTokenzier::TK_COMMENT_CPP == token_.type || JsonTokenzier::TK_COMMENT_C == token_.type);
    return token_;
}

const JsonTokenzier::Token& JsonTokenzier::NextToken() {
    SkipSpace();
    token_.clear();
    token_.begin = it_;

    char c = 0;
    switch(c = Bump()) {
    case '{': token_.type = TK_OBJ_BEGIN; break;
    case '}': token_.type = TK_OBJ_END; break;
    case '[': token_.type = TK_ARR_BEGIN; break;
    case ']': token_.type = TK_ARR_END; break;
    case ',': token_.type = TK_COMMA; break;
    case ':': token_.type = TK_COLON; break;
    case 't':
        token_.type = (Expact("rue", 3)?TK_TRUE:TK_WRONG);
        break;
    case 'f':
        token_.type = (Expact("alse", 4)?TK_FALSE:TK_WRONG);
        break;
    case 'n':
        token_.type = (Expact("ull", 3)?TK_NULL:TK_WRONG);
        break;
    case '"':
        for (c = Bump(); c && '"' != c; c = Bump()) {
            if ('\\' == c) Bump();
        }
        token_.type = ('"' == c ? TK_STRING : TK_WRONG);
        break;
    case '0': case '1': case '2': case '3': case '4': case '5': 
    case '6': case '7': case '8': case '9': case '-': case '+':
        token_.type = TK_INTEGER;

        for (c = Current(); c; c = Next()) {
            if (isdigit(c)) continue;
            if ('+' == c || '-' == c) continue;
            if ('.' == c || 'e' == c || 'E' == c) {
                token_.type = TK_REAL;
                continue;
            }
            break;
        }
        break;
    case '/':
        c = Bump();
        if ('/' == c) {
            token_.type = TK_COMMENT_CPP;
            while(c = Bump()) {
                if ('\r' == c || '\n' == c) break;
            }
        } else if('*' == c) {
            token_.type = TK_COMMENT_C;
            while(c = Bump()) {
                if ('*' == c && '/' == Current()) {
                    Bump();
                    break;
                }
            }
        } else {
            token_.type = TK_WRONG;
        }
        break;
    case 0: token_.type = TK_EOF; break;
    default: token_.type = TK_WRONG; break;
    }

    token_.end = it_;

    return token_;
}

void JsonTokenzier::SkipSpace() {
    for (char c = Current(); !IsDone() && isspace(c); c = Next());
}

std::string JsonTokenzier::str() {
    size_t line_num = 0, col_num = 0;
    const char *line = begin_;
    for (const char *it = begin_; it != token_.begin; ++it) {
        if ('\n' == *it) {
            ++line_num;
            line = it+1;
        }
    }
    col_num = token_.begin - line;

	std::string token;
	size_t len = token_.end - token_.begin;
	if (0 == len) {
		token = *token_.begin;
	} else if (32 > len) {
		token.assign(token_.begin, len);
	} else {
		token.assign(token_.begin, 29);
		token += "...";
	}

	FormatStream stm;
	stm << "%%: \"%%\" at Line: %%, Column: %%"
		<< (TK_WRONG == token_.type ? "Undefined" : type_name(token_.type))
		<< token << line_num << col_num;

    return stm.str();
}


bool JsonTokenzier::Expact(const char *str, ptrdiff_t len) {
    if (end_ - it_ < len) return false;
    if (0 != strncmp(str, it_, len)) return false;
    it_ += len;
    return true;
}

} // namespace jslite