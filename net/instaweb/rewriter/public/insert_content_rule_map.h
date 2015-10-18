/*
 * Copyright 2012 Google Inc.
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

// Author: jmaessen@google.com (Jan-Willem Maessen)

#ifndef NET_INSTAWEB_REWRITER_PUBLIC_CONTENT_RULE_MAP_H_
#define NET_INSTAWEB_REWRITER_PUBLIC_CONTENT_RULE_MAP_H_

#include <vector>
#include "pagespeed/kernel/base/atomic_int32.h"
#include "pagespeed/kernel/base/basictypes.h"
#include "pagespeed/kernel/base/string.h"
#include "pagespeed/kernel/base/string_util.h"
#include "pagespeed/kernel/base/string_hash.h"
#include "pagespeed/kernel/base/sparse_hash_map.h"
#include "net/instaweb/util/public/inline_slist.h"

namespace net_instaweb {

enum ContentRuleWhere { INSERT_START, INSERT_END, INSERT_AFTER, INSERT_BEFORE };
//vector<ContentRuleWhere> ContentRuleWhereVector;

class ContentRuleMap {
 public:
  ContentRuleMap() {
    contents_.set_deleted_key("");
    scripts_.set_deleted_key("");
  }

  ContentRuleMap(const ContentRuleMap& src) {
    contents_.set_deleted_key("");
    scripts_.set_deleted_key("");
    CopyFrom(src);
  }

  ~ContentRuleMap();

  ContentRuleMap& operator=(const ContentRuleMap& other) {
    CopyFrom(other);
    return *this;
  }
  
  // Add content string to add that complainst requirements
  bool add(const StringPiece& key, const ContentRuleWhere where,
    const bool fixed, const bool global, const bool script, const StringPiece& piece);

  // Determines whether a tag have insert content 
  bool match(const StringPiece& key) const;

  // Delete an entry from the map. Return if we delete an entry
  bool erase(const StringPiece& key);

  // clear all entrys from map
  void clear();

  // clear all scripted entrys from map
  void clearScripts();

  // Get content string that complainst requirements
  // return NULL in other case
  GoogleString content(const StringPiece& key, const ContentRuleWhere where,
    const bool fixed) const;

  // Return the number of configured content rules.
  int size() const { return contents_.size(); }

  // Return if we have rules inside
  bool empty() const { return contents_.empty(); }

  void CopyFrom(const ContentRuleMap& src);
  void AppendFrom(const ContentRuleMap& src);

  // Alias for use of CopyOnWrite
  void Merge(const ContentRuleMap& src) { AppendFrom(src); }

  class ContentRule : public InlineSListElement<ContentRule> {
    public:
      ContentRule():
          where_(INSERT_START),
          global_(false),
          order_(0),
          added_count_(0) {}
      ~ContentRule() {}

      ContentRuleWhere where() const { return where_; }
      bool fixed() const { return fixed_; }
      bool global() const { return global_; }

      int order() const { return order_; }
      int count() const { return added_count_; }

      GoogleString tag() const { return tag_; }
      GoogleString content() const { return content_; }

      void set_where(const ContentRuleWhere value) { where_ = value; }
      void set_fixed(const bool value) { fixed_ = value; }
      void set_global(const bool value) { global_ = value; }

      void set_order(const int value) { order_ = value; }
      void set_tag(const GoogleString value) { tag_ = value; }
      void set_content(const GoogleString value) { content_ = value; }

    private:
      ContentRuleWhere where_;
      bool fixed_;
      bool global_;

      int order_;
      int added_count_;
      GoogleString tag_;
      GoogleString content_;
  };

  typedef InlineSList<ContentRule> ContentRuleList;
  typedef InlineSList<ContentRule>::Iterator ContentRuleIterator;
  typedef InlineSList<ContentRule>::ConstIterator ContentRuleConstIterator;

  typedef sparse_hash_map<
    GoogleString, const ContentRuleList*,
    CaseFoldStringHash,
    CaseFoldStringEqual> StringContentRuleListSparseHashMapInsensitive;

  StringContentRuleListSparseHashMapInsensitive contents_;
  StringContentRuleListSparseHashMapInsensitive scripts_;

 private:
  // This is copyable, since we want to use this with CopyOnWrite<>
};

}  // namespace net_instaweb

#endif  // NET_INSTAWEB_REWRITER_PUBLIC_CONTENT_RULE_MAP_H_
