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

#include "net/instaweb/rewriter/public/insert_content_rule_map.h"

#include <fstream>

namespace net_instaweb {

ContentRuleMap::~ContentRuleMap() {
  clear();
}

bool ContentRuleMap::add(const StringPiece& key, const ContentRuleWhere where,
    const bool fixed, const bool global, const bool script, const StringPiece& piece) {
  GoogleString tag;
  GoogleString content;
  ContentRule* rule = new ContentRule();
  ContentRuleList* rl;

  key.CopyToString(&tag);
  piece.CopyToString(&content);

  rule->set_tag(tag);
  rule->set_where(where);
  rule->set_fixed(fixed);
  rule->set_global(global);
  rule->set_content(content);

  StringContentRuleListSparseHashMapInsensitive::const_iterator p;
  
  if (script) {
    p = scripts_.find(tag);
    if (p != scripts_.end()) {
      rl = (ContentRuleList*) p->second;
      rl->Append(rule);
    } else {
      rl = new ContentRuleList();
      rl->Append(rule);
      scripts_[tag] = rl;
    }
  } else {
    p = contents_.find(tag);
    if (p != contents_.end()) {
      rl = (ContentRuleList*) p->second;
      rl->Append(rule);
    } else {
      rl = new ContentRuleList();
      rl->Append(rule);
      contents_[tag] = rl;
    }
  }

  std::ofstream ofs;
  ofs.open ("/tmp/nginx.debug.txt", std::ofstream::out | std::ofstream::app);
  ofs << "add rulemap tag=[" << tag << "] script=[" << script <<"] content=[" << content << "]\n"; 
  ofs.close();

  return true;
}

// Determines whether a tag have insert content 
bool ContentRuleMap::match(const StringPiece& key) const {
  GoogleString tag;

  key.CopyToString(&tag);

  StringContentRuleListSparseHashMapInsensitive::const_iterator c =
      contents_.find(tag);

  if (c != contents_.end()) {
    return true;
  }

  StringContentRuleListSparseHashMapInsensitive::const_iterator s =
      scripts_.find(tag);

  if (s != scripts_.end()) {
    return true;
  }

  return false;
}

// Delete an entry from the map. Return if we delete an entry
bool ContentRuleMap::erase(const StringPiece& key) {
  GoogleString tag;
  int c;

  key.CopyToString(&tag);

  c = contents_.erase(tag);
  c += scripts_.erase(tag);

  return (0 != c);
}

void ContentRuleMap::clear() {
  contents_.clear();
  scripts_.clear();

  std::ofstream ofs;
  ofs.open ("/tmp/nginx.debug.txt", std::ofstream::out | std::ofstream::app);
  ofs << "clear\n"; 
  ofs.close();
}

void ContentRuleMap::clearScripts() {
  scripts_.clear();

  std::ofstream ofs;
  ofs.open ("/tmp/nginx.debug.txt", std::ofstream::out | std::ofstream::app);
  ofs << "clear scripts\n"; 
  ofs.close();
}

GoogleString ContentRuleMap::content(const StringPiece& key, const ContentRuleWhere where,
      const bool fixed) const {
  GoogleString tag;
  GoogleString content = "";

  key.CopyToString(&tag);
  StringContentRuleListSparseHashMapInsensitive::const_iterator p =
      contents_.find(tag);

  if (p != contents_.end()) {
    const ContentRuleList* rl = p->second;
    for (ContentRuleConstIterator iter(rl->begin()); iter != rl->end(); ++iter) {
       if (iter->where() == where) {
         if (!fixed) {
           content += iter->content();
         } else if (iter->fixed()) {
           content += iter->content();
         }
       }
    }
  }

  StringContentRuleListSparseHashMapInsensitive::const_iterator s =
      scripts_.find(tag);

  if (s != scripts_.end()) {
    const ContentRuleList* rl = s->second;
    for (ContentRuleConstIterator iter(rl->begin()); iter != rl->end(); ++iter) {
       if (iter->where() == where) {
         if (!fixed) {
           content += iter->content();
         } else if (iter->fixed()) {
           content += iter->content();
         }
       }
    }
  }

  std::ofstream ofs;
  ofs.open ("/tmp/nginx.debug.txt", std::ofstream::out | std::ofstream::app);
  ofs << "content tag=[" << tag << "] content=[" << content << "]\n"; 
  ofs.close();

  return content;
}

void ContentRuleMap::CopyFrom(const ContentRuleMap& src) {
  std::ofstream ofs;
  ofs.open ("/tmp/nginx.debug.txt", std::ofstream::out | std::ofstream::app);
  ofs << "copyfrom\n"; 
  ofs.close();

  clear();
  AppendFrom(src);
}

void ContentRuleMap::AppendFrom(const ContentRuleMap& src) {
  StringContentRuleListSparseHashMapInsensitive::const_iterator p;
  
  for (StringContentRuleListSparseHashMapInsensitive::const_iterator p =
      src.contents_.begin(); p != src.contents_.end(); p++) {
      contents_[p->first] = p->second;
  }

  for (StringContentRuleListSparseHashMapInsensitive::const_iterator p =
      src.scripts_.begin(); p != src.scripts_.end(); p++) {
      scripts_[p->first] = p->second;
  }

  std::ofstream ofs;
  ofs.open ("/tmp/nginx.debug.txt", std::ofstream::out | std::ofstream::app);
  ofs << "appendfrom\n"; 
  ofs.close();

  /*
  StringContentRuleListSparseHashMapInsensitive::const_iterator p;
  
  for (StringContentRuleListSparseHashMapInsensitive::const_iterator p =
      src.contents_.begin(); p != src.contents_.end(); p++) {
    if (match(p->first)) {
      const ContentRuleList* rl = p->second;
      for (ContentRuleConstIterator iter(rl->begin()); iter != rl->end(); ++iter) {
        add(iter->tag(),iter->where(),iter->fixed(),iter->global(),iter->content());
      }
    } else {
      contents_[p->first] = p->second;
    }
  }
  */
}

}  // namespace net_instaweb
