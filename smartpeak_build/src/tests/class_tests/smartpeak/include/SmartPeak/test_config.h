
#ifndef SMARTPEAK_TEST_CONFIG_H
#define SMARTPEAK_TEST_CONFIG_H

// Macro to construct a c string containing the complete path to the test data
#define SMARTPEAK_GET_TEST_DATA_PATH(filename) (std::string("C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/tests/class_tests/smartpeak/data/") + filename).c_str()
#define SMARTPEAK_GET_TEST_DATA_PATH_MESSAGE(prefix,filename,suffix) (prefix + std::string("C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/tests/class_tests/smartpeak/data/") + filename + suffix).c_str()

#endif // SMARTPEAK_TEST_CONFIG_H
