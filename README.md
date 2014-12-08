#jsonlite_cpp

Simple lite json stream for c++


##Simple example


```cpp
//input a string
jsonlite::JsonStream jstm;
jstm << "{ \"key\":100 }";

//parser
jsonlite::Json json;
jstm >> json;

std::cout << "key: " << json["key"].integer() << std::endl;

//printer
jstm.str(""); //clear internal buffer
jstm << json;

//output a string
std::cout << jstm.str() << std::endl;
```
