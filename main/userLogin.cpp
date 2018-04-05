)/**
 * @file userLogin.cpp
 * @author Blaze Sanders ROBO BEV(TM)
 * @date 03/19/2016
 * @link https://www.espressif.com/en/products/hardware/esp32/overview
 * @version 1.0
 *
 * @brief C to HTML interface to implement user login page via mySQL and/or openID
 *
 * @section DESCRIPTION
 *
 * Program to enable user or drone to login into an AirBox Home V1.
 *
 */

#include "userLogin.h"

//Enable program to get user input
#include <string>
#include <iostream>
#include <sstream>

//Enable use of random number generator
#include <cstdlib>
#include <ctime>

//Enable use of mySQL
//TODO How do I tell compiler to look at local code at:
// ~/GitHub/mysql/include?
// https://github.com/twitter/mysql/tree/master/include
//TODO#include <my_global.h>
//TODO#include <mysql.h>

using namespace std;

//Toogle debug print statments on and off
static const bool DEBUG_STATEMENTS_ON = true;

// static index page
static const char http_index[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "<title>AirBox User Login Screen</title>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "<center>\r\n"
    "<img src=\"http:\" alt="" \r\n"
    "<img src=\"http:\/\/customadesign.info/airboxtechnologies-new/wp-content/uploads/2018/03/logo.png\"  alt="">\r\n"
    "<h1 style=\"color: #000000;\"> User Sign In </h1>\r\n"
    "<br>\r\n"
    "<label for=\"name\">Username</label><br> <input style="" name=\"name\" type=\"text\" value=\"FirstLast\" /> <br>\r\n"
    "<br>\r\n"
    "<label for=\"name\">Password</label><br> <input name=\"name\" type=\"text\" value=\"********\" /> </form>\r\n"
    "<br><br><br>\r\n"
    "<button style=\"background-color: #CC322A; color: #FFF; display: inline-block; padding: 20px 30px; font-weight: bold; $  </button>\r\n"
    "</center>\r\n"
    "</body>\r\n"
    "</html>\r\n\r\n";


//See userLogin.h for futher documentation on the following PUBLIC functions:

//Default constructor
UserLogin::UserLogin() : databaseID(-1) {}

//TODO void UserLgoin::InitializeOpenIDDatabase(){ ??? }


void UserLogin::InitializeLocalDatabase(){

  srand(time(NULL));    //Seed random number generator with time
}

bool UserLogin::CheckPassword(string password){
  if(this->currentPassword == password) return true; //TODO Can you do this with string variables???
  else return false;
}

bool UserLogin::SetPassword(string password){
  if(7 < password.length() && password.length() < 17){
    //TODO Other checks if()
    this->currentPassword = password;
    return true;
  }
  else{
    cout << "You enter a PASSWORD less then 8 or more then 16 characters!" << endl;
    cout << "Please try again :)" << endl;
    return false;
  }
}//END SetPassword()


void UserLogin::UnitTest(){
  UserLogin testUser;

  // '!' is the NOT logic operator
  assert(testUser.CheckUsername("AirBoxUser"));
  assert(!testUser.CheckUsername("Bad.Name1"));
  assert(testUser.CheckPassword("AirBoxHome"));
  assert(!testUser.CheckPassword("BAD_PASSWORD"));

  assert(testUser.SetUsername("Blaze.Sanders"));
  assert(testUser.CheckUsername("Blaze.Sanders"));
  assert(!testUser.CheckUsername("Shawn.Miser"));

  assert(testUser.SetPassword("GoodPassword"));
  assert(!testUser.SetPassword("2Short"));
  assert(!testUser.SetPassword("WayWayWayToLongPassword"));
  assert(testUser.SetPassword("!@.#$%^!&*()-_OK"));

  if (DEBUG_STATEMENTS_ON){
    cout << "END PROGRAM" << endl;
  }

}


//See userLogin.h for futher documentation on the following PRIVATE functions:

bool UserLogin::EncryptPassword(){

  return true;
}

bool UserLogin::VerfifyPassword(){

  return true;
}
