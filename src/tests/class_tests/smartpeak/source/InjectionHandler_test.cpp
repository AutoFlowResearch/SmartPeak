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

#define BOOST_TEST_MODULE InjectionHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/InjectionHandler.h>
#include <SmartPeak/core/MetaDataHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(samplehandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  InjectionHandler* ptr = nullptr;
  InjectionHandler* nullPointer = nullptr;
  ptr = new InjectionHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
  delete ptr;
}

BOOST_AUTO_TEST_CASE(set_get_MetaData)
{
  InjectionHandler injectionHandler;

  MetaDataHandler mdh1;
  mdh1.setSampleName("1");

  injectionHandler.setMetaData(mdh1);

  const MetaDataHandler& mdh2 = injectionHandler.getMetaData();
  BOOST_CHECK_EQUAL(mdh2.getSampleName(), "1");
  BOOST_CHECK_EQUAL(injectionHandler.getMetaDataShared()->getSampleName(), "1");

  mdh1.setSampleGroupName("2");
  injectionHandler.getMetaData() = mdh1;

  const MetaDataHandler& mdh3 = injectionHandler.getMetaData();
  BOOST_CHECK_EQUAL(mdh3.getSampleName(), "1");
  BOOST_CHECK_EQUAL(mdh3.getSampleGroupName(), "2");
  BOOST_CHECK_EQUAL(injectionHandler.getMetaDataShared()->getSampleName(), "1");
  BOOST_CHECK_EQUAL(injectionHandler.getMetaDataShared()->getSampleGroupName(), "2");

  // Quick sanity check of pointer relationships
  std::shared_ptr<MetaDataHandler> mdh4(new MetaDataHandler(mdh1));

  injectionHandler.setMetaData(mdh4);
  const MetaDataHandler& mdh5 = injectionHandler.getMetaData();
  BOOST_CHECK_EQUAL(mdh5.getSampleName(), "1");
  BOOST_CHECK_EQUAL(mdh5.getSampleGroupName(), "2");

  mdh4->setSampleName("3");
  const MetaDataHandler& mdh6 = injectionHandler.getMetaData();
  BOOST_CHECK_EQUAL(mdh6.getSampleName(), "3");
  BOOST_CHECK_EQUAL(mdh6.getSampleGroupName(), "2");
}

BOOST_AUTO_TEST_CASE(set_get_RawData)
{
  InjectionHandler injectionHandler;

  OpenMS::FeatureMap f1;
  f1.setIdentifier("1");
  RawDataHandler rdh1;
  rdh1.setFeatureMap(f1);

  injectionHandler.setRawData(rdh1);

  const RawDataHandler& rdh2 = injectionHandler.getRawData();
  BOOST_CHECK_EQUAL(rdh2.getFeatureMap().getIdentifier(), "1");
  BOOST_CHECK_EQUAL(injectionHandler.getRawDataShared()->getFeatureMap().getIdentifier(), "1");

  f1.setIdentifier("2");
  rdh1.setFeatureMap(f1);
  injectionHandler.getRawData() = rdh1;

  const RawDataHandler& rdh3 = injectionHandler.getRawData();
  BOOST_CHECK_EQUAL(rdh3.getFeatureMap().getIdentifier(), "2");
  BOOST_CHECK_EQUAL(injectionHandler.getRawDataShared()->getFeatureMap().getIdentifier(), "2");
}

BOOST_AUTO_TEST_CASE(clear)
{
  InjectionHandler injectionHandler;

  MetaDataHandler mdh1;
  OpenMS::FeatureMap f1;
  RawDataHandler rdh1;
  mdh1.setSampleName("1");
  f1.setIdentifier("1");
  rdh1.setFeatureMap(f1);
  injectionHandler.setMetaData(mdh1);
  injectionHandler.setRawData(rdh1);

  BOOST_CHECK_EQUAL(injectionHandler.getMetaData().getSampleName(), "1");
  BOOST_CHECK_EQUAL(injectionHandler.getRawData().getFeatureMap().getIdentifier(), "1");

  injectionHandler.clear();

  BOOST_CHECK_EQUAL(injectionHandler.getMetaData().getSampleName(), "");
  BOOST_CHECK_EQUAL(injectionHandler.getRawData().getFeatureMap().getIdentifier(), "");
}

BOOST_AUTO_TEST_SUITE_END()
