#include "jtest.hpp"

#include "jsonlite.hpp"


int test_json_assign_fail1() {
    LOG("begin");

    int ret = 1;

    jslite::Json json(true);

    try {
        jslite::Json::Real num = json.real();
    } catch(std::logic_error& e) {
        LOG("exception: " << e.what());
        ret = 0;
    }

    LOG("end");
    
    return ret;
}

int test_json_assign_fail2() {
    LOG("begin");

    jslite::Json json;

    int ret = 1;
    try {
        jslite::Json::Real num = json.real();
    } catch(std::logic_error& e) {
        LOG("exception: " << e.what());
        ret  = 0;
    }

    LOG("end");

    return ret;
}

int test_json_assign_fail(int argc, char* argv[]) {
    EXPECT_EQ(0, test_json_assign_fail1());
    EXPECT_EQ(0, test_json_assign_fail2());

    return 0;
}