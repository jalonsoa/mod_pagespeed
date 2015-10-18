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
// Implements the insert_content filter, which inserts the
// content defined by user into html pages.

#include "net/instaweb/rewriter/public/insert_content_filter.h"

#include "base/logging.h"
#include "net/instaweb/htmlparse/public/html_element.h"
#include "net/instaweb/htmlparse/public/html_name.h"
#include "net/instaweb/htmlparse/public/html_node.h"
#include "net/instaweb/rewriter/public/experiment_util.h"
#include "net/instaweb/rewriter/public/rewrite_driver.h"
#include "net/instaweb/rewriter/public/rewrite_options.h"
#include "net/instaweb/util/public/google_url.h"
#include "net/instaweb/util/public/message_handler.h"
#include "net/instaweb/util/public/statistics.h"
#include "net/instaweb/util/public/string.h"
#include "net/instaweb/util/public/string_util.h"
#include "net/instaweb/htmlparse/public/html_keywords.h"

namespace {

// Name for statistics variable.
const char kInsertedContent[] = "inserted_content";

}  // namespace

namespace net_instaweb {

InsertContentFilter::InsertContentFilter(RewriteDriver* rewrite_driver)
    : CommonFilter(rewrite_driver),
      added_content_(false),
      exclude_(rewrite_driver->options()->content_exclude()),
      found_exclude_(false),
      contents_(rewrite_driver->options()->insert_contents()) {
  Statistics* stats = driver()->statistics();
  inserted_content_count_ = stats->GetVariable(kInsertedContent);
  search_exclude_ = !exclude_.empty();
}

void InsertContentFilter::InitStats(Statistics* stats) {
  stats->AddVariable(kInsertedContent);
}

InsertContentFilter::~InsertContentFilter() {}

void InsertContentFilter::StartDocumentImpl() {
  added_content_ = false;
  found_exclude_ = false;
  buffer_.clear();
}

void InsertContentFilter::AddTextBeforeNode(HtmlElement* current_element,
                                   GoogleString text ) {
  if (text.empty()) {
    return;
  }
  // InsertNodeBeforeCurrent
  HtmlNode* snippet = driver()->NewCharactersNode(NULL, text);

  driver()->InsertNodeBeforeNode(current_element, snippet);
}

void InsertContentFilter::AddTextAfterNode(HtmlElement* current_element,
                                   GoogleString text ) {
  if (text.empty()) {
    return;
  }
  // InsertNodeAfterCurrent
  HtmlNode* snippet = driver()->NewCharactersNode(NULL, text);

  driver()->InsertNodeAfterNode(current_element, snippet);
}

void InsertContentFilter::AddTextStartNode(HtmlElement* current_element,
                                   GoogleString text ) {
  if (text.empty()) {
    return;
  }
  // PrependChild
  HtmlNode* snippet =
      driver()->NewCharactersNode(current_element, text);

  driver()->PrependChild(current_element, snippet);
}

void InsertContentFilter::AddTextEndNode(HtmlElement* current_element,
                                   GoogleString text ) {
  if (text.empty()) {
    return;
  }

  // AppendChild
  HtmlNode* snippet =
      driver()->NewCharactersNode(current_element, text);

  driver()->AppendChild(current_element, snippet);
}

// This may not be exact, but should be a pretty good guess.
bool InsertContentFilter::FoundExcludeInBuffer() const {
  return (buffer_.find(exclude_) != GoogleString::npos);
}

// Add the experiment js snippet at the beginning of <head> and then
// start looking for content snippet.
void InsertContentFilter::StartElementImpl(HtmlElement* element) {
  if (search_exclude_ && !found_exclude_ && !buffer_.empty()) {
    found_exclude_ = FoundExcludeInBuffer();
    buffer_.clear();
  }

  const StringPiece* piece =
      HtmlKeywords::KeywordToString(element->keyword());

  if (piece != NULL) {
    if (contents_.match(*piece)) {
        AddTextBeforeNode(element, contents_.content(*piece,INSERT_BEFORE,found_exclude_));
        AddTextStartNode(element, contents_.content(*piece,INSERT_START,found_exclude_));
    }
  }
}

void InsertContentFilter::EndElementImpl(HtmlElement* element) {
  // Search for exclude inside element if search_exclude is true
  // and not found it
  if (search_exclude_ && !found_exclude_ && !buffer_.empty()) {
    found_exclude_ = FoundExcludeInBuffer();
    buffer_.clear();
  }

  // Search in contents hash, if we find key = element->keyword, then we
  // need process this hash element to insert content into the page
  const StringPiece* piece =
      HtmlKeywords::KeywordToString(element->keyword());

  if (piece != NULL) {
    if (contents_.match(*piece)) {
        AddTextAfterNode(element, contents_.content(*piece,INSERT_AFTER,found_exclude_));
        AddTextEndNode(element, contents_.content(*piece,INSERT_END,found_exclude_));
    }
  }
}

void InsertContentFilter::Characters(HtmlCharactersNode* characters) {
  if (search_exclude_ && !found_exclude_) {
    buffer_ += characters->contents();
  }
}

void InsertContentFilter::Comment(HtmlCommentNode* comment) {
  if (search_exclude_ && !found_exclude_) {
    buffer_ += comment->contents();
  }
}

}  // namespace net_instaweb
