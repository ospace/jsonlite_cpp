#include "jtest.hpp"
#include "json_stream.hpp"
#include "json_parser.hpp"
#include "json_util.hpp"
//#include "unicode_conv.hpp"

int test_empty_array() {
	jslite::JsonParser parser;

	jslite::Json *json = parser.Parse("[ ]");
	if (NULL == json) {
		LOG(parser.error_str(0));
		return 1;
	}

	EXPECT_TRUE(json->IsArray());
	EXPECT_EQ(0, json->length());

	jslite::JsonStream jss;

	jss << jslite::default_sep << *json;

	LOG("stream: " << std::endl << jss.str());

	delete json;

	return 0;
}

int test_empty_object() {
	jslite::JsonParser parser;

	jslite::Json *json = parser.Parse("{ }");
	if (NULL == json) {
		LOG(parser.error_str(0));
		return 1;
	}

	EXPECT_TRUE(json->IsObject());
	EXPECT_EQ(0, json->length());

	jslite::JsonStream jss;

	jss << jslite::default_sep << *json;

	LOG("stream: " << std::endl << jss.str());

	delete json;

	return 0;
}

int test_simple_object() {
	jslite::JsonParser parser;

	jslite::Json *json = parser.Parse("{ \"k1\":null, \"k2\":\"v2\" }");
	if (NULL == json) {
		LOG(parser.error_str(0));
		return 1;
	}

	EXPECT_TRUE(json->IsObject());
	EXPECT_EQ(2, json->length());

	jslite::JsonStream jss;

	jss << jslite::default_sep << *json;

	LOG("stream: " << std::endl << jss.str());

	delete json;

	return 0;
}


int test_empty_string() {
	jslite::JsonParser parser;

	jslite::Json *json = parser.Parse(" \"\" ");
	if (NULL == json) {
		LOG(parser.error_str(0));
		return 1;
	}

	EXPECT_TRUE(json->IsString());
	EXPECT_EQ(0, json->length());

	jslite::JsonStream jss;

	jss << jslite::default_sep << *json;

	LOG("stream: " << std::endl << jss.str());

	delete json;

	return 0;
}

int test_unicode_string() {
	jslite::JsonParser parser;
	 
	jslite::Json *json = parser.Parse(" \"\\ud55c\\uAE00\" ");
	if (NULL == json) {
		LOG(parser.error_str(0));
		return 1;
	}

	EXPECT_TRUE(json->IsString());
	EXPECT_EQ(2, jslite::SizeOfUTF8(json->string()));
	//EXPECT_EQ(0, json->length());

	const std::string str("\xED\x95\x9C\xEA\xB8\x80");
	const std::string str2(json->string());

	std::stringstream ss;

	for (size_t i=0; i<str.size(); ++i)
		ss << jslite::hex2asc((uint8_t)str[i]);
	ss << "\", but \"0x";
	for (size_t i=0; i<str2.size(); ++i)
		ss << jslite::hex2asc((uint8_t)str2[i]);
	ss << "\"";

	LOG("compare: " << ss.str());

	EXPECT_EQ(str, json->string());
	EXPECT_EQ(std::string("\xC7\xD1\xB1\xDB"), json->multibyte());

#ifdef WIN32
	LOG("String: " << json->multibyte());
#else
	LOG("String: " << json->string());
#endif

	jslite::JsonStream jss;

	jss << jslite::default_sep << *json;

	LOG("stream: " << std::endl << jss.str());

	delete json;

	return 0;
}

int test_simple_string() {
	jslite::JsonParser parser;

	jslite::Json *json = parser.Parse(" \"\\\"abc\\b\\r\\nde\\t\\/fgh\\ud55c\\uAE00\\f\" ");
	if (NULL == json) {
		LOG(parser.error_str(0));
		return 1;
	}


	EXPECT_TRUE(json->IsString());

#ifdef WIN32
	LOG("String: " << json->multibyte());
#else
	LOG("String: " << json->string());
#endif

	EXPECT_EQ(17, jslite::SizeOfUTF8(json->string()));

	std::string s_hex;
	const char *s = json->string().c_str();
	for (size_t i = 0; i < json->string().size(); ++i) {
		s_hex += jslite::hex2asc((uint8_t)s[i]);
	}

	LOG("HEX: " << s_hex);


	jslite::JsonStream jss;

	jss << jslite::default_sep << *json;

	LOG("stream: " << std::endl << jss.str());

	delete json;

	return 0;
}


int test_simple_integer() {
	jslite::JsonParser parser;

	jslite::Json *json = parser.Parse("12345");
	if (NULL == json) {
		LOG(parser.error_str(0));
		return 1;
	}

	EXPECT_TRUE(json->IsNumber());
	EXPECT_TRUE(json->IsInteger());

	EXPECT_EQ(12345, json->integer());
	jslite::JsonStream jss;

	jss << jslite::default_sep << *json;

	LOG("stream: " << std::endl << jss.str());

	return 0;
}

int test_simple_real() {
	jslite::JsonParser parser;

	jslite::Json *json = parser.Parse("-0.1e10");
	if (NULL == json) {
		LOG(parser.error_str(0));
		return 1;
	}

	EXPECT_TRUE(json->IsNumber());
	EXPECT_TRUE(json->IsReal());

	EXPECT_EQ(-0.1e10, json->real());
	jslite::JsonStream jss;

	jss << jslite::default_sep << *json;

	LOG("stream: " << std::endl << jss.str());

	return 0;
}

int test_json_parser(int argc, char* argv[]) {
	//LOG("½ÃÀÛ");

#if 1
	EXPECT_EQ(0, test_empty_array());
	EXPECT_EQ(0, test_empty_object());
	EXPECT_EQ(0, test_simple_object());
	EXPECT_EQ(0, test_empty_string());
	EXPECT_EQ(0, test_unicode_string());
	EXPECT_EQ(0, test_simple_string());
	EXPECT_EQ(0, test_simple_integer());
	EXPECT_EQ(0, test_simple_real());
	
	LOG("ok");
#else
	const std::string msg1 =
		"{"
		"     \"firstName\": \"John\","
		"     \"lastName\": \"Smith\","
		"     \"age\": 25,"
		"     \"address\":"
		"     {"
		"         \"streetAddress\": \"21 2nd Street\","
		"         \"city\": \"New York\","
		"         \"state\": \"NY\","
		"         \"postalCode\": \"10021\""
		"     },"
		"     \"phoneNumber\":"
		"     ["
		"         {"
		"           \"type\": \"home\","
		"           \"number\": \"212 555-1234\""
		"         },"
		"         {"
		"           \"type\": \"fax\","
		"           \"number\": \"646 555-4567\""
		"         }"
		"     ]"
		" }";

	if (0 != test_json_parser(msg1)) return 1;

	const std::string msg2 = "["
		"{"
		"\"FirstName1\":\"Anoushka\","
		"\"Last_Name\":\"Nycil\","
		"\"Age\":3"
		"},"
		"{"
		"\"FirstName\":\"Len\","
		"\"LastName\":\"bistro\""
		"},"
		"{"
		"\"Length\":55,"
		"\"breath\":25.28,"
		"\"Area\":22e+2"
		"},"
		"{"
		"\"firstname1\":\"Robert\","
		"\"firstname2\":\"Solomon\","
		"\"firstname3\":\"Jane\","
		"\"firstname4\":\"Veronica\","
		"\"firstname5\":\"Christopher\""
		"},"
		"{"
		"\"Name\":\"Isabelle\","
		"\"ID\": 43231,"
		"\"Enrolled\":true,"
		"\"classes\":null"
		"},"
		"{"
		"\"FirstName\":\"Dolly\","
		"\"LastName\":\"Nycil\","
		"\"Class\":\"History\""
		"},"
		"{"
		"\"Student\": { \"Name\":\"isabelle\","
		"                \"ID\":43231,"
		"                \"Enrolled\":{\"History\":true,"
		"                               \"Science\":false"
		"                              }"
		"             }"
		"},"
		"{"
		"\"Students\":[{\"Name\":\"Isabelle\",\"ID\":43231,\"Enrolled\":true},"
		"              {\"Name\":\"Tonya\",\"ID\":43232,\"Enrolled\":true},"
		"              {\"Name\":\"Vivian\",\"ID\":43233,\"Enrolled\":true}]"
		"}]";

	if (0 != test_json_parser(msg2)) return 1;

	const std::string msg3 =
		"{"
		"    \"glossary\": {"
		"        \"title\": \"example glossary\","
		"		\"GlossDiv\": {"
		"            \"title\": \"S\","
		"			\"GlossList\": {"
		"                \"GlossEntry\": {"
		"                    \"ID\": \"SGML\","
		"					\"SortAs\": \"SGML\","
		"					\"GlossTerm\": \"Standard Generalized Markup Language\","
		"					\"Acronym\": \"SGML\","
		"					\"Abbrev\": \"ISO 8879:1986\","
		"					\"GlossDef\": {"
		"                        \"para\": \"A meta-markup language, used to create markup languages such as DocBook.\","
		"						\"GlossSeeAlso\": [\"GML\", \"XML\"]"
		"                    },"
		"					\"GlossSee\": \"markup\""
		"                }"
		"            }"
		"        }"
		"    }"
		"}";

	if (0 != test_json_parser(msg3)) return 1;
	const std::string msg4 =
		"{\"widget\": {"
		"    \"debug\": \"on\","
		"    \"window\": {"
		"        \"title\": \"Sample Konfabulator Widget\","
		"        \"name\": \"main_window\","
		"        \"width\": 500,"
		"        \"height\": 500"
		"    },"
		"    \"image\": { "
		"        \"src\": \"Images/Sun.png\","
		"        \"name\": \"sun1\","
		"        \"hOffset\": 250,"
		"        \"vOffset\": 250,"
		"        \"alignment\": \"center\""
		"    },"
		"    \"text\": {"
		"        \"data\": \"Click Here\","
		"        \"size\": 36,"
		"        \"style\": \"bold\","
		"        \"name\": \"text1\","
		"        \"hOffset\": 250,"
		"        \"vOffset\": 100,"
		"        \"alignment\": \"center\","
		"        \"onMouseUp\": \"sun1.opacity = (sun1.opacity / 100) * 90;\""
		"    }"
		"}}  ";

	if (0 != test_json_parser(msg4)) return 1;

	const std::string msg5 =
		"{\"menu\": {"
		"    \"header\": \"SVG Viewer\","
		"    \"items\": ["
		"        {\"id\": \"Open\"},"
		"        {\"id\": \"OpenNew\", \"label\": \"Open New\"},"
		"        null,"
		"        {\"id\": \"ZoomIn\", \"label\": \"Zoom In\"},"
		"        {\"id\": \"ZoomOut\", \"label\": \"Zoom Out\"},"
		"        {\"id\": \"OriginalView\", \"label\": \"Original View\"},"
		"        null,"
		"        {\"id\": \"Quality\"},"
		"        {\"id\": \"Pause\"},"
		"        {\"id\": \"Mute\"},"
		"        null,"
		"        {\"id\": \"Find\", \"label\": \"Find...\"},"
		"        {\"id\": \"FindAgain\", \"label\": \"Find Again\"},"
		"        {\"id\": \"Copy\"},"
		"        {\"id\": \"CopyAgain\", \"label\": \"Copy Again\"},"
		"        {\"id\": \"CopySVG\", \"label\": \"Copy SVG\"},"
		"        {\"id\": \"ViewSVG\", \"label\": \"View SVG\"},"
		"        {\"id\": \"ViewSource\", \"label\": \"View Source\"},"
		"        {\"id\": \"SaveAs\", \"label\": \"Save As\"},"
		"        null,"
		"        {\"id\": \"Help\"},"
		"        {\"id\": \"About\", \"label\": \"About Adobe CVG Viewer...\"}"
		"    ]"
		"}}";
	if (0 != test_json_parser(msg5)) return 1;

	const std::string msg6 =
		"{"
		"     \"firstName\": \"John\","
		"     \"lastName\": \"Smith\","
		"     \"age\": 25,"
		"     \"address\":"
		"     {"
		"         \"streetAddress\": \"21 2nd Street\","
		"         \"city\": \"New York\","
		"         \"state\": \"NY\","
		"         \"postalCode\": \"10021\""
		"     },"
		"     \"phoneNumber\":"
		"     ["
		"         {"
		"           \"type\": \"home\","
		"           \"number\": \"212 555-1234\""
		"         },"
		"         {"
		"           \"type\": \"fax\","
		"           \"number\": \"646 555-4567\""
		"         }"
		"     ]"
		" }";

	if (0 != test_json_parser(msg6)) return 1;

	const std::string msg7 =
		"{"
		"  \"name=\":\"\\\"ospace\\\"\""
		"}";

	if (0 != test_json_parser(msg7)) return 1;
#endif
	return 0;
}