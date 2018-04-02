/**
 * @file userLogin.h
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
#ifndef USER_LOGIN_H
#define USER_LOGIN_H

#include <iostream>      //Standard input/output stream objects, needed to use cout()
#include <string>        //Used for strlen() function
#include <cassert>       //Used in UnitTest for functional testing

using namespace std;

class UserLogin
{

  //Private function prototypes not need by developers or customers
  private:
    int databaseID;


    /**
     * @brief Encrypt password for transimission over the internet
     *
     * @section DESCRIPTION
     *
     * TODO
     *
     * @return  TRUE if password was sucessfully encrypted, and  FALSE otherwise
     */
     bool EncryptPassword();

    /**
     * @brief Compare encrypted password to password store in user database
     *
     * @section DESCRIPTION
     *
     * TODO
     *
     * @return  TRUE if password was sucessfully encrypted, and  FALSE otherwise
     */
    bool VerfifyPassword();


  //Public function prototypes to get and set class variables
  public:
    UserLogin();


    /**
     * @brief Create and configure mySQL database to store encryted user login data
     *
     * @section DESCRIPTION
     *
     * Outputs pins default to LOW and input default to an UNDEFINED state.
     *
     * @param NONE TODO db Pointer to struct holding user login database
     *
     * @see ???
     *
     * @return NOTHING
     */
    void InitializeLocalDatabase();

    /**
     * @brief Verify that username even exists in the database
     *
     * @section DESCRIPTION
     *
     * No special characters such as "@ _ - , . " are allowed in usernames.
     * If username doesn't exist user is promted to create a username
     *
     * @param  userName Username to check database for
     *
     * @see ????
     *
     * @return TRUE if username exists, and FALSE otherwise
     */
    bool GetUsername();


    /**
     * @brief Parse and encrypt user password and then compare to database
     *
     * @section DESCRIPTION
     *
     * No special characters such as "@ _ - , . " are allowed in passwords.
     *
     * @param  password Password compare against username in database
     *
     * @see ????
     *
     * @return  TRUE = 1 if username exists, and  FALSE = 0 otherwise
     */
    bool CheckPassword();


    /**
     * @brief Test userLogin.c pin using assertions, one hardware configurations, and user input.
    *
    * @param NONE
    *
    * @return NOTHING
    */
    void UnitTest();

};  //END CLASS

#endif //USER_LOGIN_H
