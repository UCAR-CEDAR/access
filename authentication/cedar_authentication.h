#ifndef cedar_authentication_h_
#define cedar_authentication_h_ 1

#ifdef __cplusplus
extern "C" {
// IDL include.
#include "export.h"
}
#endif

/**
   @name Cedar Authentication Library.
   This are the global status values returned by the library.
   
   \begin{verbatim}
   Return values from the shared object:
   1: Can not connect to MySQL server.
   2: The executed query was not successful.
   3: Bad query exception.
   4: Bad Conversion exception.
   5: Unknown exception.
   6: Incorrect number of parameters.
   7: Incorrect parameter type.
   8: Pointer to parameter was NULL.
   9: user does not exist in the database.
   10: Incorrect numbers of rows returned from the database.
   11: user specified incorrect password
   12: user does not have permission to access cedarweb
    \end{verbatim}
   
   Specific functions return values:
   
   \begin{verbatim}
   cedar_authenticate_user
   0: User has been authenticated.
   11: The given password is incorrect.
   
   cedar_logoff_user
   0: User has been logout.

   cedar_is_login
   0: The user session is still valid.
   12: The user session has expired.
   
   cedar_create_user
   0: User has been created.

   cedar_delete_user
   0: User has been deleted.

   cedar_is_user_authorized
   0: User is authorized 
   13: User is not authorized.

   \end{verbatim}


*/


/**
   Authenticate a user in the database.
   This method takes a username and a password and checks for the validity of the login in the database. If the login is accepted, the session timer is set to the current time on which the authentication was performed.
   @param argv: Array of 2 IDL_VPTR object where the first object is a string with username and the second is the password.
   @return 0: User has been authenticated. 11: The given password is incorrect.
*/
static extern "C" IDL_VPTR cedar_authenticate_user(int argc, IDL_VPTR argv[]);


/**
   Logoff a user from the database.
   Takes a username and if the user exist, removes the user from the
   sessions table.
   @param argv: Array of 1 IDL_VPTR object where the first object is a string with username.
   @return 0: User has been logout.
*/
static extern "C" IDL_VPTR cedar_logoff_user(int argc, IDL_VPTR argv[]);


/**
    Create a session for the specified user
    Takes a username and if the user exists, creates an entry in the session
    table for the user.
    @param argv: Array of 1 IDL_VPTR object where the first object is a
    string with username.
    @return 0: Sessions has been created
*/
static extern "C" IDL_VPTR cedar_create_session(int argc, IDL_VPTR argv[]);


#if 0
/**
  Checks for user's session status.
  Given the username it checks if the session timer has not expired. 
  @param argv: Array of 1 IDL_VPTR object where the first object is a string with username.
  @return  0: The user session is still valid. 12: The user session has expired.
*/
static extern "C" IDL_VPTR cedar_is_login(int argc, IDL_VPTR argv[]);


/**
   Create a new user.
   Adds a new user to the list of users in the database.
   @param argv: Array of 2 IDL_VPTR object where the first object is a string with username and the second is the password.
   @return 0: User has been created.
*/
static extern "C" IDL_VPTR cedar_create_user(int argc, IDL_VPTR argv[]);


/**
   Delete a user.
   Drops a username from the list of users in the database.
   @param argv: Array of 1 IDL_VPTR object where the first object is a string with username.
   @return 0: User has been deleted.
*/
static extern "C" IDL_VPTR cedar_delete_user(int argc, IDL_VPTR argv[]);

/**
   Checks if a user is athorized for an specific action.
   Given a username and a action it checks the group of the user and then it checks the policy for such a group. 
   If the policy contains the action for which the function is invoked it returns 0 otherwise returns 13.
   Please notice that the policy are built based on the level of the group in which the user belongs. 
   For example for level 2, all privileges assigned to level 2,1 and 0 are part of the policy.
   @param argv: Array of 2 IDL_VPTR object where the first object is a string with username and the second is the action.
   @return 0: User is authorized. 13: User is not authorized. 
*/
static extern "C" IDL_VPTR cedar_is_user_authorized(int argc, IDL_VPTR argv[]) ;
#endif


#endif // cedar_authentication_h_
