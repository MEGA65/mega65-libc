#ifndef TESTS_H
#define TESTS_H

#define TEST_START 0xf0
#define TEST_SKIP 0xf1
#define TEST_PASS 0xf2
#define TEST_FAIL 0xf3
#define TEST_ERROR 0xf4
#define TEST_LOG 0xfd
#define TEST_SETNAME 0xfe
#define TEST_DONEALL 0xff


/** \m65libsummary{unit_test_report}{Reports unit test result to the host machine}
    \m65libsyntax    {void unit_test_report(unsigned short issue, unsigned char sub, unsigned char status);}
    \m65libparam     {issue}{The issue number that identifies the test issue}
    \m65libparam     {sub}{The sub issue number (for multiple tests per issue)}
    \m65libparam     {status}{The test status to be sent}
*/
void unit_test_report(unsigned short issue, unsigned char sub, unsigned char status);

/** \m65libsummary{unit_test_set_current_name}{Reports current test name to the host machine}
    \m65libsyntax    {void unit_test_set_current_name(char *name);}
    \m65libparam     {name}{The human-readable name of the current test}
*/
void unit_test_set_current_name(char *name);

/** \m65libsummary{unit_test_log}{Logs a message on the host machine}
    \m65libsyntax    {void unit_test_log(char *msg);}
    \m65libparam     {msg}{The message to be logged}
*/
void unit_test_log(char *msg);

// convenience methods
void unit_test_setup(char *testName, unsigned short issueNum);
void unit_test_ok(void);
void unit_test_fail(char *msg);
void unit_test_done(void);


#endif