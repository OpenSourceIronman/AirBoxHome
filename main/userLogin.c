/**
 * @file userLogin.c
 * @author Blaze Sanders ROBO BEV(TM)
 * @date 03/19/2016
 * @link https://www.espressif.com/en/products/hardware/esp32/overview
 * @version 1.0
 *
 * @brief C to HTML interface connect user login page to mySQL database
 *
 * @section DESCRIPTION
 *
 * Program to configure and control the General Purpose Input / Output
 * (GPIO) pins of the ESP32  IoT device, and enable user to login into
 * an AirBox Home V1.
 *
 */


#include "userLogin.h"
#include <string>


// static index page
static const char http_index[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "<title>AirBox User Login Screen</title>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    //Used www.webflow.io to generate HTML
    "<div depth=\"5\" tabindex=\"0\" style=\"position: absolute; top: 0px; left: 0px; width: 240px; opacity: 1; background: rgb(64, 64, 64); transform: translate3d(0px, 225px, 0px); outline: 0px; cursor: default; user-select: none; display: flex; align-items: center; box-sizing: border-box; padding-right: 10px; padding-left: 82px; line-height: 24px;\"><div style=\"box-sizing: border-box; flex-shrink: 0; width: 18px; height: 24px; padding-right: 6px; display: flex; align-items: center; justify-content: center;\"></div><div style=\"flex: 1 1 0%; overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\"><div style=\"display: flex; align-items: center;\"><div type=\"regular\" style=\"color: rgb(217, 217, 217); padding-right: 6px;\"><svg width="12" height="12" viewBox="0 0 12 12" class="bem-Svg " style="display: block; transform: translate(0px, 0px);"><path fill="currentColor" d="M10 2v8H2V2h8m1-1H1v10h10V1z"></path><path fill="currentColor" d="M4 3H3v6h1V3" opacity=".35"></path></svg></div><div style="flex: 1 1 0%; overflow: hidden; text-overflow: ellipsis; white-space: nowrap;"><div style="display: flex; align-items: center; flex: 0 1 auto;"><div style="overflow: hidden; text-overflow: ellipsis; white-space: nowrap;">Form Field</div></div></div></div></div></div>
    "<h1 style="color: #cc322a;">AirBox Technologies <span style="background-color: #FFF; color: #FFFFFF; padding: 0 20px;"></span> </h1> 

<span style="background-color: #cc322a; color: #FFF; display: inline-block; padding: 10px 20px; font-weight: bold; border-radius: 20px;">LOGIN</span>"
    "</body>\r\n"
    "</html>\r\n\r\n";






//See userLogin.h for futher documentation on the following PUBLIC functions:

void InitializeDatabase(??? *db){


}

unsigned int CheckUsername(string userName){

  return TRUE;
}


unsigned int CheckPassword(string password){

  return TRUE;
}


void UnitTest(){

  assert(CheckUsername("GoodName1"))
  assert(CheckUsername("Bad.Name1"))

  assert(CheckUsername("2goodName"))
  assert(CheckUsername("BadName2@"))

  assert(CheckUsername("Goodname3"))
  assert(CheckUsername("Bad_Name3"))
  assert(CheckUsername("-BadName4"))

  IF(DEBUG_STATEMENTS_ON) ???;

}


//See userLogin.h for futher documentation on the following PRIVATE functions:

unsigned int EncryptPassword(){

  return TRUE;
}

unsigned int VerfifyPassword(){

  return TRUE;
}


