#include "jtest.hpp"
#include "json_stream.hpp"
#include "json_util.hpp"

int test_error_value() {
	jslite::JsonStream jstm;
	jslite::Json json;

	jstm << "\"str ";
	
	int32_t ret = jstm.Parse(json);
	EXPECT_FALSE((ret == 0));
	LOG(jstm.strerror(ret));
	EXPECT_EQ(jslite::ERR_VALUE, ret);

	return 0;
}

int test_error_esc_char() {
	jslite::JsonStream jstm;
	jslite::Json json;

	jstm << "\"str\\i\"";
	
	int32_t ret = jstm.Parse(json);
	EXPECT_FALSE((ret == 0));
	LOG(jstm.strerror(ret));
	EXPECT_EQ(jslite::ERR_ESC_CHAR, ret);

	return 0;
}

int test_error_unicode() {
	jslite::JsonStream jstm;
	jslite::Json json;

	jstm << "\"\\u10g2\"";
	
	int32_t ret = jstm.Parse(json);
	EXPECT_FALSE((ret == 0));
	LOG(jstm.strerror(ret));
	EXPECT_EQ(jslite::ERR_UNICODE, ret);

	return 0;
}

int test_error_array_end() {
	jslite::JsonStream jstm;
	jslite::Json json;

	jstm << "[1,2,3";
	
	int32_t ret = jstm.Parse(json);
	EXPECT_FALSE((ret == 0));
	LOG(jstm.strerror(ret));
	EXPECT_EQ(jslite::ERR_ARRAY_END, ret);

	return 0;
}

int test_error_object_key() {
	jslite::JsonStream jstm;
	jslite::Json json;

	jstm << "{1:\"val\"}";
	
	int32_t ret = jstm.Parse(json);
	EXPECT_FALSE((ret == 0));
	LOG(jstm.strerror(ret));
	EXPECT_EQ(jslite::ERR_OBJECT_KEY, ret);

	return 0;
}

int test_error_object_sep() {
	jslite::JsonStream jstm;
	jslite::Json json;

	jstm << "{\"key\",\"val\"}";
	
	int32_t ret = jstm.Parse(json);
	EXPECT_FALSE((ret == 0));
	LOG(jstm.strerror(ret));
	EXPECT_EQ(jslite::ERR_OBJECT_SEP, ret);

	return 0;
}

int test_error_object_end() {
	jslite::JsonStream jstm;
	jslite::Json json;

	jstm << "{\"key\":\"val\"]";
	
	int32_t ret = jstm.Parse(json);
	EXPECT_FALSE((ret == 0));
	LOG(jstm.strerror(ret));
	EXPECT_EQ(jslite::ERR_OBJECT_END, ret);

	return 0;
}

int test_json_parse_error(int argc, char* argv[]) {
	EXPECT_EQ(0, test_error_value());
	EXPECT_EQ(0, test_error_esc_char());
	EXPECT_EQ(0, test_error_unicode());
	EXPECT_EQ(0, test_error_array_end());
	EXPECT_EQ(0, test_error_object_key());
	EXPECT_EQ(0, test_error_object_sep());
	EXPECT_EQ(0, test_error_object_end());
    
    LOG("ok");

    return 0;
}