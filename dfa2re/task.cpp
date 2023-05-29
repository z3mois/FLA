#include "api.hpp"
#include <string>
#include <set>
#include <map>
#include <iostream>
#include <vector>
using namespace std;
string open = "(";
string close = ")";
string orr = "|";
string multi = "*";
void udalaiu_pridurkov(int state, map<pair<int, int>, string>& m){
  string in_itself = "";
  if (m.count(make_pair(state, state)) != 0){
    in_itself = m[make_pair(state, state)];
    m.erase(make_pair(state, state));
  }
  set<pair<int, int>> for_delete;
  for (auto p_from:m){
    if (p_from.first.second == state && p_from.first.first != state){
      for (auto p_to:m){
        if (p_to.first.second != state && p_to.first.first == state){
          string add = open + p_from.second  + close + open + in_itself + close + multi + open + p_to.second + close;
          if (m.count(make_pair(p_from.first.first, p_to.first.second)) != 0){
            add += orr + m[make_pair(p_from.first.first, p_to.first.second)];
          }
          cout << add << endl;
          m[make_pair(p_from.first.first, p_to.first.second)] = add;
          for_delete.insert(p_from.first);
          for_delete.insert(p_to.first);
        }
      }
    }
  }
  for (auto elem: for_delete){
    m.erase(elem);
  }
  return;
}

std::string dfa2re(DFA &d) {
  cout << "here" << endl;
  auto terms = d.get_alphabet().to_string();
  if (terms == ""){
    return "";
  } 
  auto all = d.get_states();
  if (all.empty()){
    return "";
  }

  map<pair<int, int>, string> m;
  map<int, string> idx2name;
  map<string, int> name_idx;
  int id = 0;
  cout << "here" << endl;
  for (auto state: all){
    name_idx[state] = id;
    idx2name[id] = state;
    id+=1;
  }
  for (auto state: all){
    if (d.is_final(state)){
        m[make_pair(name_idx[state], all.size())] = "";
    }
    for (auto terminate : terms){
        if (d.has_trans(state, terminate)){
            pair<int, int> p = make_pair(name_idx[state], name_idx[d.get_trans(state, terminate)]);
            string s(1, terminate);
            // https://www.techiedelight.com/convert-char-to-string-cpp/
            if (m.count(p) == 0){
                m[p] = s;
            } else {
                m[p] += orr + s;
            }
        } 
    }
  }
  for (auto p:m){
    cout << p.first.first << " " << p.first.second << " : " << p.second << "\n";
  }
  cout << "dadasddddddddddddddddddddddd\n";
  for (int i=0; i<all.size(); i++){
    if (!d.is_initial(idx2name[i])){
      udalaiu_pridurkov(i, m);
    }
  }
  for (auto p:m){
    cout << p.first.first << " " << p.first.second << " : " << p.second << "\n";
  }
  string answer = "";
  if (m.count(make_pair(name_idx[d.get_initial_state()], name_idx[d.get_initial_state()])) != 0){
    answer = open + m[make_pair(name_idx[d.get_initial_state()], name_idx[d.get_initial_state()])] + close + multi;
  }
  answer += open + m[make_pair(name_idx[d.get_initial_state()], all.size())] + close;
  return answer;
}