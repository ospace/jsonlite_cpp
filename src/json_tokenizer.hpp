#ifndef __JS_JSON_TOKENIZER_HPP_20130410__
#define __JS_JSON_TOKENIZER_HPP_20130410__

#include <string>

namespace jslite {

class JsonTokenzier {
public:
    typedef enum {
        TK_NONE, TK_WRONG, TK_STRING, TK_INTEGER, TK_REAL, TK_TRUE, TK_FALSE, TK_NULL,
        TK_COLON, TK_COMMA, TK_OBJ_BEGIN, TK_OBJ_END, TK_ARR_BEGIN, TK_ARR_END, 
        TK_COMMENT_C, TK_COMMENT_CPP, TK_EOF
    } TokenType;
    
    struct Token {
        Token() { clear(); }
        void clear() { type = TK_NONE; begin = NULL; end = NULL; }
        TokenType type;
        const char *begin;
        const char *end;
    };

    JsonTokenzier(const char *begin, const char *end);

	char Current();
	char Next();
    char Bump();
    bool IsDone();
    const Token& CurrentToken();
    const Token& SkipCommentAndNextToken();
    const Token& NextToken();
    void SkipSpace();
    std::string str();

protected:
	bool Expact(const char *str, ptrdiff_t len);

private:
    const char *begin_;
    const char *end_;
    const char *it_;
    Token token_;
};

} // namespace jslite

#endif //__JS_JSON_TOKENIZER_HPP_20130410__