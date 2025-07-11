// Semmle test case for rule SprintfToSqlQuery.ql (Uncontrolled sprintf for SQL query)
// Associated with CWE-089: SQL injection. http://cwe.mitre.org/data/definitions/89.html

///// Library routines /////

typedef unsigned long size_t;
int snprintf(char *s, size_t n, const char *format, ...);
void sanitizeString(char *stringOut, size_t len, const char *strIn);
int mysql_query(int arg1, const char *sqlArg);
int atoi(const char *nptr);
void exit(int i);
///// Test code /////

int main(int argc, char** argv) { // $ Source
  char *userName = argv[2];
  int userNumber = atoi(argv[3]);

  // a string from the user is injected directly into an SQL query.
  char query1[1000] = {0};
  snprintf(query1, 1000, "SELECT UID FROM USERS where name = \"%s\"", userName);
  mysql_query(0, query1); // $ Alert
  
  // the user string is encoded by a library routine.
  char userNameSanitized[1000] = {0};
  sanitizeString(userNameSanitized, 1000, userName); 
  char query2[1000] = {0};
  snprintf(query2, 1000, "SELECT UID FROM USERS where name = \"%s\"", userNameSanitized);
  mysql_query(0, query2); // GOOD

  // an integer from the user is injected into an SQL query.
  char query3[1000] = {0};
  snprintf(query3, 1000, "SELECT UID FROM USERS where number = \"%i\"", userNumber);
  mysql_query(0, query3); // GOOD

  nonReturning(userName);
}

char* globalUsername;

void nonReturning(char* username) {
  globalUsername = username;
  badFunc();
  // This function does not return, so we used to lose the global flow here.
  exit(0);
}

void badFunc() {
  char *userName = globalUsername;
  char query1[1000] = {0};
  snprintf(query1, 1000, "SELECT UID FROM USERS where name = \"%s\"", userName);
  mysql_query(0, query1); // $ Alert
}

//ODBC Library Rountines
typedef unsigned char SQLCHAR;
typedef long int SQLINTEGER;
typedef int SQLRETURN;
typedef void* SQLHSTMT;

char* gets(char *str);


SQLRETURN SQLPrepare(  
     SQLHSTMT      StatementHandle,  
     SQLCHAR *     StatementText,  
     SQLINTEGER    TextLength);  

     SQLRETURN SQLExecDirect(  
     SQLHSTMT     StatementHandle,  
     SQLCHAR *    StatementText,  
     SQLINTEGER   TextLength);  

void ODBCTests(){
  char userInput[100];
  gets(userInput); // $ Source
  SQLPrepare(0, userInput, 100); // $ Alert
  SQLExecDirect(0, userInput, 100); // $ Alert
}

// Oracle Call Interface (OCI) Routines
int OCIStmtPrepare(
      void *arg0,
      void *arg1,
      const unsigned char *sql,
      unsigned int arg3,
      unsigned int arg4,
      unsigned int arg5);
int OCIStmtPrepare2(
      void *arg0,
      void **arg1,
      void *arg2,
      const unsigned char *sql,
      unsigned int arg4,
      const unsigned char *arg5,
      unsigned int arg6,
      unsigned int arg7,
      unsigned int arg8);

void OCITests(){
  char userInput[100];
  gets(userInput); // $ Source

  // a string from the user is injected directly into an SQL query.
  char query1[1000] = {0};
  snprintf(query1, 1000, "SELECT UID FROM USERS where name = \"%s\"", userInput);
  OCIStmtPrepare(0, 0, query1, 0, 0, 0); // $ Alert
  OCIStmtPrepare2(0, 0, 0, query1, 0, 0, 0, 0, 0); // $ Alert

  // an integer from the user is injected into an SQL query.
  int userNumber = atoi(userInput);
  char query2[1000] = {0};
  snprintf(query2, 1000, "SELECT UID FROM USERS where number = \"%i\"", userNumber);
  OCIStmtPrepare(0, 0, query2, 0, 0, 0); // GOOD
  OCIStmtPrepare2(0, 0, 0, query2, 0, 0, 0, 0, 0); // GOOD
}