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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#define BOOST_TEST_MODULE CastValue test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/CastValue.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(castvalue)

BOOST_AUTO_TEST_CASE(castValue_constructor_copyConstructor)
{
  CastValue c;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::UNINITIALIZED, true);
  BOOST_CHECK_EQUAL(c.b_, false);

  c = 7;

  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c.i_, 7);

  CastValue c2 = c;
  BOOST_CHECK_EQUAL(c2.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c2.i_, 7);

  CastValue c3(c);
  BOOST_CHECK_EQUAL(c3.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c3.i_, 7);
}

BOOST_AUTO_TEST_SUITE_END()
