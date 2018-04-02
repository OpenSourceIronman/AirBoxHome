/**
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

//TODO http://www.math.uaa.alaska.edu/~afkjm/csce211/handouts/SeparateCompilation.pdf
//TODO https://stackoverflow.com/questions/877523/error-request-for-member-in-which-is-of-non-class-type

#include "userLogin.h"

//Enable program to get user input
#include <string>
#include <iostream>
#include <sstream>

//Enable use of random number generator 
#include <cstdlib>
#include <ctime>

//Enable use of mySQL
#include <my_global.h>
#include <mysql.h>

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
  databaseID = rand();  //Give database a random ID

  //TODO http://zetcode.com/db/mysqlc/
  MYSQL *con = mysql_init(NULL);
  if (DEBUG_STATEMENTS_ON){
    printf("MySQL client version: %s\n", mysql_get_client_info());
  }

  if (con == NULL){
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  //We provide connection handler, host name, user name and password parameters to the function.
  //The other four parameters are the database name, port number, unix socket and finally the client flag.
  //We need superuser priviliges to create a new database.
  if (mysql_real_connect(con, "localhost", "root", "root_pswd", NULL, 0, NULL, 0) == NULL){
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
  }

  if (mysql_query(con, "CREATE DATABASE login_db")){
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
  }

  mysql_close(con);
}

bool UserLogin::GetUsername(){

  return TRUE;
}


bool UserLogin::CheckPassword(){

  return TRUE;
}


void UserLogin::UnitTest(){

  //assert(GetUsername("GoodName1"))
  //assert(GetUsername("Bad.Name1"))

  //assert(GetUsername("2goodName"))
  //assert(GetUsername("BadName2@"))

  //assert(GetUsername("Goodname3"))
  //assert(GetUsername("Bad_Name3"))
  //assert(GetUsername("-BadName4"))

  if (DEBUG_STATEMENTS_ON){
    cout << "END PROGRAM" << endl;
  }

}


//See userLogin.h for futher documentation on the following PRIVATE functions:

bool UserLogin::EncryptPassword(){

  return TRUE;
}

bool UserLogin::VerfifyPassword(){

  return TRUE;
}
