#include <iostream>
#include <string>

#include "userLogin.h"

int main(int argc, char **argv){
  cout << "Create database" << endl;

  UserLogin db;

  bool OK;
  OK = db.GetUsername();

  return 0;
}
