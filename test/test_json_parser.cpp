#include "jtest.hpp"
#include "json_stream.hpp"
#include "json_util.hpp"


int test_empty_array() {
    jslite::JsonStream parser;
    parser << "[ ]";

    jslite::Json json;
    
    EXPECT_TRUE(0 == parser.Parse(json));
    EXPECT_TRUE(json.IsArray());
    EXPECT_EQ(0, json.length());

    jslite::JsonStream jss;

    jss << jslite::default_sep << json;

    LOG("stream: " << std::endl << jss.str());


    return 0;
}

int test_empty_object() {
    jslite::JsonStream parser;
    parser << "{ }";

    jslite::Json json;
    
    EXPECT_TRUE(0 == parser.Parse(json));
    EXPECT_TRUE(json.IsObject());
    EXPECT_EQ(0, json.length());

    jslite::JsonStream jss;

    jss << jslite::default_sep << json;

    LOG("stream: " << std::endl << jss.str());

    return 0;
}

int test_simple_object() {
    jslite::JsonStream parser;
    parser << "{ \"k1\":null, \"k2\":\"v2\" }";

    jslite::Json json;
    
    EXPECT_TRUE(0 == parser.Parse(json));
    EXPECT_TRUE(json.IsObject());
    EXPECT_EQ(2, json.length());

    jslite::JsonStream jss;

    jss << jslite::default_sep << json;

    LOG("stream: " << std::endl << jss.str());

    return 0;
}


int test_empty_string() {
    jslite::JsonStream parser;

    parser << " \"\" ";

    jslite::Json json;
    
    EXPECT_TRUE(0 == parser.Parse(json));
    EXPECT_TRUE(json.IsString());
    EXPECT_EQ(0, json.length());

    jslite::JsonStream jss;

    jss << jslite::default_sep << json;

    LOG("stream: " << std::endl << jss.str());

    return 0;
}

int test_unicode_string() {
    jslite::JsonStream parser;

    parser << " \"\\ud55c\\uAE00\" ";
     
    jslite::Json json;
    
    EXPECT_TRUE(0 == parser.Parse(json));
    EXPECT_TRUE(json.IsString());
    EXPECT_EQ(2, jslite::SizeOfUTF8(json.string()));
    //EXPECT_EQ(0, json->length());

    const std::string str("\xED\x95\x9C\xEA\xB8\x80");
    const std::string str2(json.string());

    std::stringstream ss;

    for (size_t i=0; i<str.size(); ++i)
        ss << jslite::hex2asc((uint8_t)str[i]);
    ss << "\", but \"0x";
    for (size_t i=0; i<str2.size(); ++i)
        ss << jslite::hex2asc((uint8_t)str2[i]);
    ss << "\"";

    LOG("compare: " << ss.str());

    EXPECT_EQ(str, json.string());
    EXPECT_EQ(std::string("\xC7\xD1\xB1\xDB"), json.multibyte());

#ifdef WIN32
    LOG("String: " << json.multibyte());
#else
    LOG("String: " << json.string());
#endif

    jslite::JsonStream jss;

    jss << jslite::default_sep << json;

    LOG("stream: " << std::endl << jss.str());

    return 0;
}

int test_simple_string() {
    jslite::JsonStream parser;
    parser << " \"\\\"abc\\b\\r\\nde\\t\\/fgh\\ud55c\\uAE00\\f\" ";

    jslite::Json json;
    
    EXPECT_TRUE(0 == parser.Parse(json));
    EXPECT_TRUE(json.IsString());

#ifdef WIN32
    LOG("String: " << json.multibyte());
#else
    LOG("String: " << json.string());
#endif

    EXPECT_EQ(17, jslite::SizeOfUTF8(json.string()));

    std::string s_hex;
    const char *s = json.string().c_str();
    for (size_t i = 0; i < json.string().size(); ++i) {
        s_hex += jslite::hex2asc((uint8_t)s[i]);
    }

    LOG("HEX: " << s_hex);


    jslite::JsonStream jss;

    jss << jslite::default_sep << json;

    LOG("stream: " << std::endl << jss.str());

    return 0;
}


int test_simple_integer() {
    jslite::JsonStream parser;
    parser << "12345";

    jslite::Json json;
    
    EXPECT_TRUE(0 == parser.Parse(json));
    EXPECT_TRUE(json.IsNumber());
    EXPECT_TRUE(json.IsInteger());

    EXPECT_EQ(12345, json.integer());
    jslite::JsonStream jss;

    jss << jslite::default_sep << json;

    LOG("stream: " << std::endl << jss.str());

    return 0;
}

int test_simple_real() {
    jslite::JsonStream parser;

    parser << "-0.1e10";

    jslite::Json json;
    
    EXPECT_TRUE(0 == parser.Parse(json));
    EXPECT_TRUE(json.IsNumber());
    EXPECT_TRUE(json.IsReal());

    EXPECT_EQ(-0.1e10, json.real());
    jslite::JsonStream jss;

    jss << jslite::default_sep << json;

    LOG("stream: " << std::endl << jss.str());

    return 0;
}

int test_simple_comment() {
    jslite::JsonStream parser;

    parser << "//cpp style comment\n";
    parser << "{\"k\":1 /*c style comment*/}\n";

    jslite::Json json;
    
    EXPECT_TRUE(0 == parser.Parse(json));
    EXPECT_TRUE(json.IsObject());
    EXPECT_TRUE(1, json["k"].integer());
    
    jslite::JsonStream jss;
    jss << jslite::default_sep << json;

    LOG("stream: " << std::endl << jss.str());

    return 0;
}

int test_json_parser(int argc, char* argv[]) {
    EXPECT_EQ(0, test_empty_array());
    EXPECT_EQ(0, test_empty_object());
    EXPECT_EQ(0, test_simple_object());
    EXPECT_EQ(0, test_empty_string());
    EXPECT_EQ(0, test_unicode_string());
    EXPECT_EQ(0, test_simple_string());
    EXPECT_EQ(0, test_simple_integer());
    EXPECT_EQ(0, test_simple_real());
    EXPECT_EQ(0, test_simple_comment());
    
    LOG("ok");

    return 0;
}