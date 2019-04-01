﻿//
// Demo - demonstrates log stream abilities, prints various types of messages.
//

#include <plog/Log.h>
#include "MyClass.h"
#include "Customer.h"

int main()
{
    plog::init(plog::debug, "Demo.csv", 5000, 3); // Initialize the logger.

    // Log macro types.
    LOGD << "Hello log!"; // short macro
    PLOG_DEBUG << "Hello log!"; // long macro
    LOG(plog::debug) << "Hello log!"; // function-style macro

    // Log severity levels.
    LOG_VERBOSE << "This is a VERBOSE message";
    PLOG_DEBUG << "This is a DEBUG message";
    PLOG_INFO << "This is an INFO message";
    LOG_WARNING << "This is a WARNING message";
    PLOG_ERROR << "This is an ERROR message";
    LOG_FATAL << "This is a FATAL message";
    LOG_NONE << "This is a NONE message";

    // Integers demo.
    PLOG_INFO << "This is a bool: " << std::boolalpha << true;
    PLOG_INFO << "This is a char: " << 'x';
    PLOG_INFO << "This is an unsigned char: " << (unsigned char)40;
    PLOG_INFO << "This is a short: " << (short)-1000;
    PLOG_INFO << "This is an unsigned short: " << (unsigned short)1000;
    PLOG_INFO << "This is an int: " << (int)-1000000;
    PLOG_INFO << "This is an unsigned int: " << (unsigned int)1000000;
    PLOG_INFO << "This is a long(hex): " << std::hex << (long)100000000;
    PLOG_INFO << "This is an unsigned long: " << (unsigned long)100000000;
    PLOG_INFO << "This is a float: " << 1.2345f;
    PLOG_INFO << "This is a double: " << std::setprecision(15) << 1.234512345;

    // Managed string.
#ifdef __cplusplus_cli
    System::String^ managedStr = "This is a managed string";
    PLOG_INFO << managedStr;
#endif

    // Null strings are safe.
    PLOG_DEBUG << static_cast<char*>(NULL);
    PLOG_DEBUG << static_cast<const char*>(NULL);

#if PLOG_ENABLE_WCHAR_INPUT
    PLOG_DEBUG << static_cast<wchar_t*>(NULL);
    PLOG_DEBUG << static_cast<const wchar_t*>(NULL);
#endif

    // Plog handles unicode and std::string/wstring.
#ifndef _WIN32 // On Windows the following code produces a warning C4566 if the codepage is not Cyrillic.
    PLOG_DEBUG << "Cat - котэ";
    PLOG_DEBUG << std::string("test - тест");
#endif

#if PLOG_ENABLE_WCHAR_INPUT
    PLOG_DEBUG << L"Cat - котэ";
    PLOG_DEBUG << std::wstring(L"test - тест");
    PLOG_DEBUG << L'ы';
#endif

    // Multiline.
    PLOG_INFO << "This\nis\na" << std::endl << "multiline\nmessage!";

    // Quotes.
    PLOG_INFO << "This is a message with \"quotes\"!";

    // Conditional logging.
    int var = 0;
    LOG_DEBUG_IF(var != 0) << "You shouldn't see this message";
    LOG_DEBUG_IF(var == 0) << "This is a conditional log message";

    // Executed only on log level >= debug.
    IF_LOG(plog::debug) var = 5; // one line
    IF_LOG(plog::debug) // block
    {
        var++;
    }

    // Log macros don't break then-else clause without braces.
    if (var == 0) LOGI << "then clause (condition is false, so it is skipped)"; else LOGI << "else clase (should be visible)";

    // Log in a class (capture this pointer, c++ function names).
    MyClass obj;
    obj.method();
    obj.inlineMethod();

    MyClass::staticMethod();

    // Implicit cast to string.
    PLOG_INFO << obj;

    // ostream operator<< (on Windows wostream operator<< has priority but not required)
    Customer customer = { 10, "John" };
    PLOG_INFO << customer;

    return 0;
}
