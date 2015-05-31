/// TestJSON.cpp
/// Shaun Harker
/// 2015-05-26

#include <iostream>
#include <string>
#include <memory>

#include "Tools/json.h"

int main ( int argc, char * argv [] ) {
  using namespace JSON;
  std::shared_ptr<Value> json = parse ( 
    "  [{\"Fred\": 5, \"Sheila\":6 }, [3, \"string with spaces and escaped quote \\\"\"],"
    "    {   \"a\" :  [2, 3 ],\"b\"  :{\"c\":2}}]   " );
  std::string stringified = stringify ( json );
  std::shared_ptr<Value> json2 = parse ( stringified );
  std::string stringified2 = stringify ( json2 );
  std::cout << stringified << "\n";
  std::shared_ptr<Array> array = toArray ( json );
  std::cout << stringify ( array ) << "\n";
  if ( stringified == stringified2 ) std::cout << "JSON test passed.\n";
  return 0;
}
