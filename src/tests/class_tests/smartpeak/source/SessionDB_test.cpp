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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/io/SessionDB.h>

using namespace SmartPeak;
using namespace std;

TEST(SessionDB, dbFilePath)
{
  SessionDB session_db;
  EXPECT_EQ("", session_db.getDBFilePath());
  auto path_db = std::tmpnam(nullptr);
  session_db.setDBFilePath(path_db);
  EXPECT_EQ(path_db, session_db.getDBFilePath());
}

TEST(SessionDB, WriteAndRead)
{
  SessionDB session_db;

  // create db
  auto path_db = std::tmpnam(nullptr);
  session_db.setDBFilePath(path_db);
  
  // write
  auto context = session_db.beginWrite("table1", "col1", "INT", "col2", "REAL", "col3", "STRING");
  ASSERT_TRUE(context);
  session_db.write(*context, 42, 3.14, "test0");
  session_db.write(*context, 142, 13.14, "test1");
  session_db.write(*context, 242, 23.14, "test2");
  session_db.endWrite(*context);

  // read
  context = session_db.beginRead("table1", "col1", "col2", "col3");
  ASSERT_TRUE(context);
  int db_int = 0;
  float db_float = 0.0f;
  std::string db_string;
  bool has_more = session_db.read(*context, db_int, db_float, db_string);
  EXPECT_TRUE(has_more);
  EXPECT_EQ(db_int, 42);
  EXPECT_FLOAT_EQ(db_float, 3.14);
  EXPECT_EQ(db_string, "test0");
  has_more = session_db.read(*context, db_int, db_float, db_string);
  EXPECT_TRUE(has_more);
  EXPECT_EQ(db_int, 142);
  EXPECT_FLOAT_EQ(db_float, 13.14);
  EXPECT_EQ(db_string, "test1");
  has_more = session_db.read(*context, db_int, db_float, db_string);
  EXPECT_TRUE(has_more);
  EXPECT_EQ(db_int, 242);
  EXPECT_FLOAT_EQ(db_float, 23.14);
  EXPECT_EQ(db_string, "test2");
  has_more = session_db.read(*context, db_int, db_float, db_string);
  EXPECT_FALSE(has_more);
}
