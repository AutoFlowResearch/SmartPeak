// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Krzysztof Abram $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------
#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE ConsoleHandler test suite
#include <boost/test/included/unit_test.hpp>

#include <SmartPeak/core/ConsoleHandler.h>
#include <filesystem>


struct ConsoleHandlerFixture 
{
    /* ctor/dtor */
    ConsoleHandlerFixture() 
    {
        m_datapath = SMARTPEAK_GET_TEST_DATA_PATH("");
    }
    std::string m_datapath;
};

/* ---------------------------------------------- */

BOOST_FIXTURE_TEST_SUITE(ConsoleHandler, ConsoleHandlerFixture)

BOOST_AUTO_TEST_CASE(ConsoleHandler_initialize)
{
    namespace fs = std::filesystem;
    auto& ch = SmartPeak::ConsoleHandler::get_instance();
    // Test singleton uniqueness:
    auto& ch1 = SmartPeak::ConsoleHandler::get_instance();
    BOOST_CHECK_EQUAL(&ch, &ch1);
    // Test custom log location:
    {
        ch.set_log_directory(m_datapath);
        ch.initialize("Welcome");
        BOOST_CHECK(fs::exists(ch.get_log_filepath()));
    }
    // Test inability to initialize instance for the second time:
    BOOST_CHECK_THROW(ch.initialize("Welcome"), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
