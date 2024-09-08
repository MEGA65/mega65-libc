/**
 * @file tests.h
 * @brief Unit test functions for running on hardware and in Xemu
 * @todo Remove \m65libsummary and \m65libsyntax comments as they replaced by
 * doxygen comments and can be extracted from the XML output
 */
#ifndef __MEGA65_TESTS_H
#define __MEGA65_TESTS_H

#include <stdint.h>

#define TEST_START 0xf0
#define TEST_SKIP 0xf1
#define TEST_PASS 0xf2
#define TEST_FAIL 0xf3
#define TEST_ERROR 0xf4
#define TEST_LOG 0xfd
#define TEST_SETNAME 0xfe
#define TEST_DONEALL 0xff
#define XEMU_CONTROL 0xD6CF
#define XEMU_QUIT 0x42 //!< Command to make Xemu quit

#ifdef __cplusplus
// Being compiled by a C++ compiler, inhibit name mangling
extern "C" {
#endif

/**
 * @brief Quits Xemu with given exit code
 * @param exit_code Exit code passed to Xemu, e.g. EXIT_SUCCESS or EXIT_FAILURE
 *
 * Xemu must be run in `-testing` mode for this to have any effect and
 * also make sure to call `mega65_io_enable()` before calling this function.
 */
void xemu_exit(int exit_code);

/**
 * @brief Assert if two integers are equal; if not, quit Xemu with EXIT_FAILURE
 */
#ifdef __CC65__
void assert_eq(int32_t a, int32_t b);
#else
void assert_eq(int64_t a, int64_t b);
#endif

// convenience methods

/* \m65libsummary{unit_test_setup}{Setup of the unit test reporting to the host
   machine} \m65libsyntax    {void unit_test_setup(char *testName, unsigned
   short issueNum);} \m65libparam     {testName}{Human readable name of the
   test} \m65libparam     {issueNum}{The mega65-core issue number that
   identifies the test issue}
*/
/**
 * @brief Setup of the unit test reporting to the host machine
 * @param testName Human readable name of the test
 * @param issueNum The mega65-core issue number that identifies the test issue
 */
void unit_test_setup(char* testName, unsigned short issueNum);

/* \m65libsummary{unit_test_ok}{Report a successful test with a optional
   message} \m65libsyntax    {void unit_test_ok(char *msg);} \m65libparam
   {msg}{Description of the successful test (NULL uses global testName)}
*/
/**
 * @brief Report a successful test with a optional message
 * @param msg Description of the successful test (NULL uses global testName)
 */
void unit_test_ok(char* msg);

/* \m65libsummary{unit_test_fail}{Report a failed test with a optional message}
    \m65libsyntax    {void unit_test_fail(char *msg);}
    \m65libparam     {msg}{Description of the failed test (NULL uses global
   testName)}
*/
/**
 * @brief Report a failed test with a optional message
 * @param msg Description of the failed test (NULL uses global testName)
 */
void unit_test_fail(char* msg);

/* \m65libsummary{unit_test_done}{Finish test procedure and tell m65 to exit}
    \m65libsyntax    {void unit_test_done(void);}
*/
/**
 * @brief Finish test procedure and tell m65 to exit
 */
void unit_test_done(void);

// low level functions

/* \m65libsummary{unit_test_report}{Reports unit test result to the host
   machine} \m65libsyntax    {void unit_test_report(unsigned short issue,
   unsigned char sub, unsigned char status);} \m65libparam     {issue}{The issue
   number that identifies the test issue} \m65libparam     {sub}{The sub issue
   number (for multiple tests per issue)} \m65libparam     {status}{The test
   status to be sent}
*/
/**
 * @brief Reports unit test result to the host machine
 * @param issue The issue number that identifies the test issue
 * @param sub The sub issue number (for multiple tests per issue)
 * @param status The test status to be sent
 */
void unit_test_report(
    unsigned short issue, unsigned char sub, unsigned char status);

/* \m65libsummary{unit_test_set_current_name}{Reports current test name to the
   host machine} \m65libsyntax    {void unit_test_set_current_name(char *name);}
    \m65libparam     {name}{The human-readable name of the current test}
*/
/**
 * @brief Reports current test name to the host machine
 * @param name The human-readable name of the current test
 */
void unit_test_set_current_name(char* name);

/* \m65libsummary{unit_test_log}{Logs a message on the host machine}
    \m65libsyntax    {void unit_test_log(char *msg);}
    \m65libparam     {msg}{The message to be logged}
*/
/**
 * @brief Logs a message on the host machine
 * @param msg The message to be logged
 */
void unit_test_log(char* msg);

/**
 * @brief VIC init functions
 */
typedef struct {
    uint8_t pal;
    uint8_t crt_emu;
    uint8_t h640;
    uint8_t v400;
    uint16_t side_border_width;
    uint8_t border_col;
    uint8_t background_col;
    uint8_t cols;
    uint8_t rows;
    uint8_t lower_case_charset;
    uint8_t disable_viciii_bug_compatibility;
    uint8_t xscl;
} vic_config;

/*
 * \m65libsummary{unit_test_vic_get_default_pal}{Get the default VIC
 * configuration for PAL} \m65libsyntax{void
 * unit_test_vic_get_default_pal(vic_config* conf);} \m65libparam{conf}{Pointer
 * to a vic_config struct to store the default configuration}
 */
/**
 * @brief Get the default VIC configuration for PAL
 * @param conf Pointer to a vic_config struct to store the default configuration
 */
void unit_test_vic_get_default_pal(vic_config* conf);

/*
 * \m65libsummary{unit_test_vic_get_default_ntsc}{Get the default VIC
 * configuration for NTSC} \m65libsyntax{void
 * unit_test_vic_get_default_ntsc(vic_config* conf);} \m65libparam{conf}{Pointer
 * to a vic_config struct to store the default configuration}
 */
/**
 * @brief Get the default VIC configuration for NTSC
 * @param conf Pointer to a vic_config struct to store the default configuration
 */
void unit_test_vic_get_default_ntsc(vic_config* conf);

/*
 * \m65libsummary{unit_test_init_vic}{Initialize the VIC with the given
 * configuration} \m65libsyntax{void unit_test_init_vic(vic_config* conf);}
 * \m65libparam{conf}{Pointer to a vic_config struct containing the
 * configuration to use}
 */
/**
 * @brief Initialize the VIC with the given configuration
 * @param conf Pointer to a vic_config struct containing the configuration to
 * use
 */
void unit_test_init_vic(vic_config* conf);

/*
 * \m65libsummary{unit_test_print}{Print a message on the screen at the given
 * position and color} \m65libsyntax{void unit_test_print(uint8_t x, uint8_t y,
 * uint8_t colour, char *msg);} \m65libparam{x}{The x-coordinate of the position
 * to print the message, zero-based} \m65libparam{y}{The y-coordinate of the
 * position to print the message, zero-based} \m65libparam{colour}{The color to
 * use for the message} \m65libparam{msg}{The null-terminated message to print,
 * ascii encoded}
 */
/**
 * @brief Print a message on the screen at the given position and color
 * @param x The x-coordinate of the position to print the message, zero-based
 * @param y The y-coordinate of the position to print the message, zero-based
 * @param colour The color to use for the message
 * @param msg The null-terminated message to print, ascii encoded
 */
void unit_test_print(uint8_t x, uint8_t y, uint8_t colour, char* msg);

/*
 * \m65libsummary{unit_test_read_pixel}{Read the color of the pixel at the given
 * position} \m65libsyntax{void unit_test_read_pixel(short x, short y, uint8_t
 * *red, uint8_t *green, uint8_t *blue);} \m65libparam{x}{The x-coordinate of
 * the pixel to read} \m65libparam{y}{The y-coordinate of the pixel to read}
 * \m65libparam{red}{Pointer to a variable to store the red component of the
 * pixel color} \m65libparam{green}{Pointer to a variable to store the green
 * component of the pixel color} \m65libparam{blue}{Pointer to a variable to
 * store the blue component of the pixel color}
 */
/**
 * @brief Read the color of the pixel at the given position
 * @param x The x-coordinate of the pixel to read
 * @param y The y-coordinate of the pixel to read
 * @param red Pointer to a variable to store the red component of the pixel
 * color
 * @param green Pointer to a variable to store the green component of the pixel
 * color
 * @param blue Pointer to a variable to store the blue component of the pixel
 * color
 */
void unit_test_read_pixel(
    short x, short y, uint8_t* red, uint8_t* green, uint8_t* blue);

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif // __MEGA65_TESTS_H
