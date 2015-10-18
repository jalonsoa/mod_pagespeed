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
// This provides the InsertContentFilter class which adds a Content
// snippet to html pages.

#ifndef NET_INSTAWEB_REWRITER_PUBLIC_INSERT_CONTENT_FILTER_H_
#define NET_INSTAWEB_REWRITER_PUBLIC_INSERT_CONTENT_FILTER_H_

#include "net/instaweb/rewriter/public/common_filter.h"
#include "net/instaweb/rewriter/public/insert_content_rule_map.h"
#include "net/instaweb/util/public/basictypes.h"
#include "net/instaweb/util/public/string.h"
#include "pagespeed/kernel/base/string_hash.h"
#include "pagespeed/kernel/base/string_util.h"
#include "pagespeed/kernel/base/sparse_hash_map.h"


namespace net_instaweb {

class HtmlCharactersNode;
class HtmlCommentNode;
class HtmlElement;
class RewriteDriver;
class Statistics;
class Variable;

// This class is the implementation of insert_content_snippet filter, which adds
// a Google Analytics snippet into the head of html pages.
class InsertContentFilter : public CommonFilter {
 public:
  explicit InsertContentFilter(RewriteDriver* rewrite_driver);
  virtual ~InsertContentFilter();

  // Set up statistics for this filter.
  static void InitStats(Statistics* stats);

  virtual void StartDocumentImpl();
  virtual void StartElementImpl(HtmlElement* element);
  virtual void EndElementImpl(HtmlElement* element);
  // HTML Events we expect to be in <script> elements.
  virtual void Characters(HtmlCharactersNode* characters);
  virtual void Comment(HtmlCommentNode* characters);

  virtual const char* Name() const { return "InsertContent"; }

 private:
  // If appropriate, insert the GA snippet at the end of the body element.
  void HandleEndBody(HtmlElement* body);

  // Adds a text contents immediately before current element.
  void AddTextBeforeNode(HtmlElement* current_element, GoogleString text);
  // Adds a text contents immediately after current element.
  void AddTextAfterNode(HtmlElement* current_element, GoogleString text);
  // Prepend a text contents a child of the current_element
  void AddTextStartNode(HtmlElement* current_element, GoogleString text);
  // Append a text contents a child of the current_element
  void AddTextEndNode(HtmlElement* current_element, GoogleString text);

  // Look to see if the script had a GA snippet, and modify our code
  // appropriately.
  void HandleEndScript(HtmlElement* script);

  // Indicates whether or not buffer_ contains a exclude content snippet
  bool FoundExcludeInBuffer() const;

  // Stats on how many tags we moved.
  Variable* inserted_content_count_;

  // Script element we're currently in, so we can check it to see if
  // it has the Content snippet already.
  HtmlElement* script_element_;
  // Whether we added the analytics js or not.
  bool added_content_;

  // Buffer in which we collect the contents of any script element we're
  // looking for the GA snippet in.
  GoogleString buffer_;

  // Indicates if exclude option is activated and we must search exclude
  bool search_exclude_;

  // string to search to don't insert no fixed content on this page
  GoogleString exclude_;

  // Indicates whether or not we've already found a exclude snippet.
  bool found_exclude_;

  ContentRuleMap contents_;

  DISALLOW_COPY_AND_ASSIGN(InsertContentFilter);
};

}  // namespace net_instaweb

#endif  // NET_INSTAWEB_REWRITER_PUBLIC_INSERT_CONTENT_FILTER_H_
