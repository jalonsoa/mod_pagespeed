/*
 * Copyright 2011 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Author: justo.alonso@gmail.com (Justo Alonso)
// Based on works from nforman@google.com (Naomi Forman)
//
// This file contains unit tests for the InsertContentFilter.

#include "net/instaweb/rewriter/public/insert_content_filter.h"

#include "net/instaweb/rewriter/public/rewrite_test_base.h"
#include "net/instaweb/rewriter/public/rewrite_driver.h"
#include "net/instaweb/rewriter/public/rewrite_options.h"
#include "net/instaweb/util/public/gtest.h"
#include "net/instaweb/util/public/null_message_handler.h"
#include "net/instaweb/util/public/string.h"
#include "net/instaweb/util/public/string_util.h"

namespace net_instaweb {

namespace {

// Test fixture for InsertContentFilter unit tests.
class InsertContentFilterTest : public RewriteTestBase {
 protected:
  virtual void SetUp() {
    options()->EnableFilter(RewriteOptions::kInsertContent);
    RewriteTestBase::SetUp();
  }
};

const char kHtmlInput[] =
    "<head>\n"
    "<title>Something</title>\n"
    "</head>"
    "<body> Hello World!</body>";

const char kHtmlOutputFormat[] =
    "<head>\n"
    "<title>Something</title>\n"
    "</head>"
    "<body> Hello World!</body>";

GoogleString GenerateExpectedHtml(GoogleString url_prefix) {
//  GoogleString output = StringPrintf(kHtmlOutputFormat,
//                                     analytics_js.c_str());
//  return output;
  GoogleString output = StringPrintf(kHtmlOutputFormat);
  return output;
}

TEST_F(InsertContentFilterTest, SimpleInsert) {
  rewrite_driver()->AddFilters();
  GoogleString output = GenerateExpectedHtml("http");
  ValidateExpected("simple_addition", kHtmlInput, output);

  output = GenerateExpectedHtml("https");
  ValidateExpectedUrl("https://", kHtmlInput, output);
}

}  // namespace

}  // namespace net_instaweb
