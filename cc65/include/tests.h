#ifndef TESTS_H
#define TESTS_H

#define TEST_START 0xf0
#define TEST_SKIP 0xf1
#define TEST_PASS 0xf2
#define TEST_FAIL 0xf3
#define TEST_ERROR 0xf4
#define TEST_DONEALL 0xff

/** \m65libsummary{unit_test_report}{Reports unit test result to the host machine}
    \m65libsyntax    {void unit_test_report(unsigned short issue, unsigned char sub, unsigned char status);}
    \m65libparam     {issue}{The issue number that identifies the test issue}
    \m65libparam     {sub}{The sub issue number (for multiple tests per issue)}
    \m65libparam     {status}{The test status to be sent}
*/
void unit_test_report(unsigned short issue, unsigned char sub, unsigned char status);

#endif