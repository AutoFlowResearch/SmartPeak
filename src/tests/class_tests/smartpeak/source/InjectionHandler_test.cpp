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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/core/InjectionHandler.h>
#include <SmartPeak/core/MetaDataHandler.h>

using namespace SmartPeak;
using namespace std;

TEST(InjectionHandler, constructor)
{
  InjectionHandler* ptr = nullptr;
  InjectionHandler* nullPointer = nullptr;
  ptr = new InjectionHandler();
  EXPECT_NE(ptr, nullPointer);
  delete ptr;
}

TEST(InjectionHandler, set_get_MetaData)
{
  InjectionHandler injectionHandler;

  MetaDataHandler mdh1;
  mdh1.setSampleName("1");

  injectionHandler.setMetaData(mdh1);

  const MetaDataHandler& mdh2 = injectionHandler.getMetaData();
  EXPECT_STREQ(mdh2.getSampleName().c_str(), "1");
  EXPECT_STREQ(injectionHandler.getMetaDataShared()->getSampleName().c_str(), "1");

  mdh1.setSampleGroupName("2");
  injectionHandler.getMetaData() = mdh1;

  const MetaDataHandler& mdh3 = injectionHandler.getMetaData();
  EXPECT_STREQ(mdh3.getSampleName().c_str(), "1");
  EXPECT_STREQ(mdh3.getSampleGroupName().c_str(), "2");
  EXPECT_STREQ(injectionHandler.getMetaDataShared()->getSampleName().c_str(), "1");
  EXPECT_STREQ(injectionHandler.getMetaDataShared()->getSampleGroupName().c_str(), "2");

  // Quick sanity check of pointer relationships
  auto mdh4 = std::make_shared<MetaDataHandler>(mdh1);

  injectionHandler.setMetaData(mdh4);
  const MetaDataHandler& mdh5 = injectionHandler.getMetaData();
  EXPECT_STREQ(mdh5.getSampleName().c_str(), "1");
  EXPECT_STREQ(mdh5.getSampleGroupName().c_str(), "2");

  mdh4->setSampleName("3");
  const MetaDataHandler& mdh6 = injectionHandler.getMetaData();
  EXPECT_STREQ(mdh6.getSampleName().c_str(), "3");
  EXPECT_STREQ(mdh6.getSampleGroupName().c_str(), "2");
}

TEST(InjectionHandler, set_get_RawData)
{
  InjectionHandler injectionHandler;

  OpenMS::FeatureMap f1;
  f1.setIdentifier("1");
  RawDataHandler rdh1;
  rdh1.setFeatureMap(f1);

  injectionHandler.setRawData(rdh1);

  const RawDataHandler& rdh2 = injectionHandler.getRawData();
  EXPECT_STREQ(rdh2.getFeatureMap().getIdentifier().c_str(), "1");
  EXPECT_STREQ(injectionHandler.getRawDataShared()->getFeatureMap().getIdentifier().c_str(), "1");

  f1.setIdentifier("2");
  rdh1.setFeatureMap(f1);
  injectionHandler.getRawData() = rdh1;

  const RawDataHandler& rdh3 = injectionHandler.getRawData();
  EXPECT_STREQ(rdh3.getFeatureMap().getIdentifier().c_str(), "2");
  EXPECT_STREQ(injectionHandler.getRawDataShared()->getFeatureMap().getIdentifier().c_str(), "2");
}

TEST(InjectionHandler, clear)
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

  EXPECT_STREQ(injectionHandler.getMetaData().getSampleName().c_str(), "1");
  EXPECT_STREQ(injectionHandler.getRawData().getFeatureMap().getIdentifier().c_str(), "1");

  injectionHandler.clear();

  EXPECT_STREQ(injectionHandler.getMetaData().getSampleName().c_str(), "");
  EXPECT_STREQ(injectionHandler.getRawData().getFeatureMap().getIdentifier().c_str(), "");
}
