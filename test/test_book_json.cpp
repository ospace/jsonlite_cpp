#include "jtest.hpp"
#include "jsonlite.hpp"

#include <fstream>

int test_book_json(int argc, char* argv[]) {
    std::ifstream openFile;
    openFile.open("book.json");

    EXPECT_EQ(true, openFile.is_open());

    std::ostringstream oss;

    std::istreambuf_iterator<char> begin(openFile);
    std::istreambuf_iterator<char> end;

    std::string buf(begin, end);

    openFile.close();
    
    jslite::Json json;
    json.Decode(buf);

    LOG("channel.item[0].status_des: " << json["channel"]["item"][0]["status_des"]);
    LOG("channel.item[0].status_des: " << json["channel"]["item"][0]["status_des"].multibyte());
    
    return 0;
}