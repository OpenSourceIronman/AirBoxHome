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


