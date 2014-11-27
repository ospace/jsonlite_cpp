#include "jtest.hpp"
#include "jsonlite.hpp"
#include "json_stream.hpp"
#include "json_parser.hpp"


using jslite::Json;

int test_json_assign_value(int argc, char* argv[]) {

	LOG("begin");

	try {
		jslite::Json v[10];

		//Object
		v[0]["k1"] = 100;
		EXPECT_EQ(true, v[0].IsObject());
		EXPECT_EQ(true, v[0]["k1"].IsNumber());
		EXPECT_EQ(100, v[0]["k1"].integer());
		EXPECT_EQ((size_t)1, v[0].size());

		v[0]["k2"] = "200";
		EXPECT_EQ(true, v[0]["k2"].IsString());
		EXPECT_EQ("200", v[0]["k2"].string());
		EXPECT_EQ((size_t)2, v[0].size());

		v[1] = 111;
		EXPECT_EQ(true, v[1].IsNumber());
		EXPECT_EQ(111, v[1].integer());

		v[2] = "abc";
		EXPECT_EQ(true, v[2].IsString());
		EXPECT_EQ("abc", v[2].string());

		v[3] = 0.00001;
		EXPECT_EQ(true, v[3].IsReal());
		EXPECT_EQ(0.00001, v[3].integer());

		v[4]["k3"] = "def";
		EXPECT_EQ("def", v[4]["k3"].string());

		jslite::Json json;
		json.put(v[0]);
		json.put(v[1]);
		json.put(v[2]);
		json.put(v[3]);
		json.put(v[4]);

		EXPECT_EQ(true, json.IsArray());
		EXPECT_EQ((size_t)5, json.size());
		EXPECT_EQ(v[0], json[0]);

		LOG(json);

		json[0].remove_by("k1");
		EXPECT_EQ((size_t)5, json.size());

		LOG(json);

		json.remove_at(0);
		EXPECT_EQ((size_t)4, json.size());

		LOG(json);

	} catch (std::range_error &e) {
		LOG("range_error: " << e.what());
		return 1;
	} catch (std::exception &e) {
		LOG("exception: " << e.what());
		return 1;
	}

	LOG("end");

	return 0;
}