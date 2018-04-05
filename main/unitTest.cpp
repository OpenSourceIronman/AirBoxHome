#include <iostream>
#include <string>

#include "userLogin.h"

// Complied using:
// g++ unitTest.cpp userLogin.cpp -o unitTest -std=c++11 `mysql_config --cflags --libs`

int main(int argc, char **argv){
  UserLogin user1;
  user1.UnitTest();

  return 0;
}


void LocalDatabaseTest(){

  cout << "Create database" << endl;

  UserLogin db;

  bool OK;
  //OK = db.CheckUsername();
}
