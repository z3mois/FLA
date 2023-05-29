#include "api.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <tuple>
#include <map>
using namespace std;
tuple<set<string>, set<string>> split(DFA& d,set<string> R, set<string> C,char terminate){
  set<string> R1, R2;
  for (auto from: R){
    if (d.has_trans(from, terminate) && C.count(d.get_trans(from, terminate)) != 0){
        R1.insert(from);
      } else {
        R2.insert(from);
      }
  }
  return make_tuple(R1, R2);
}
void delete_ned(DFA& d, string state, set<string>& reach){
  reach.insert(state);
  for (auto terminate:d.get_alphabet()){
    if (d.has_trans(state, terminate) && reach.count(d.get_trans(state, terminate)) == 0){
      delete_ned(d, d.get_trans(state, terminate), reach);
    }
  }
  return;
}
DFA dfa_minim(DFA &d) {
  auto all = d.get_states();
  if (all.empty()){
    return d;
  }
  set<string> reach;
  // 0
  delete_ned(d, d.get_initial_state(), reach);
  // for (auto elem:reach){
  //   cout << elem << " ";
  // }
  for (auto state: d.get_states()){
    cout << state <<  " " << (reach.count(state) == 0) << endl;
    if (reach.count(state) == 0){
      d.delete_state(state);
    }
  }
  string name_add = "finalllllsssssss";
  d.create_state(name_add, false);
  for (auto state: d.get_states()){
    for (auto terminate:d.get_alphabet()){
      if (!d.has_trans(state, terminate)){
        d.set_trans(state, terminate, name_add);
      }
    }
  }
  // 1
  set <set<string>> P;
  queue <set<string>> S;
  set<string> qf;
  set<string> f;
  for (auto state: d.get_states()){
    if (d.is_final(state)){
      f.insert(state);
    }
    else {
      qf.insert(state);
    }
  }
  P.insert(f);
  P.insert(qf);
  S.push(f);
  S.push(qf);
  while (!S.empty()){
    set<string> C = S.front();
    S.pop();
    for (auto terminate:d.get_alphabet()){
      set<set<string>> p_temp = P;
      for (auto R:P){
        set<string> R1, R2;
        tie(R1, R2) = split(d, R, C, terminate); 
        if (!R1.empty() && !R2.empty()){
          p_temp.erase(R);
          p_temp.insert(R1);
          p_temp.insert(R2);
          S.push(R1);
          S.push(R2);
        }
      }
      P = p_temp;
    }
  }
// 2
  DFA answer(d.get_alphabet());
  map<string, set<string>> state2group;
  for (auto settt:P){
    for (auto strr:settt){
      state2group[strr] = settt;
    }
  }
  map<set<string>, string> name2idx;
  map<string, set<string>> idx2name;
  int idx = 1;
  for (auto elem: P){
    name2idx[elem] = to_string(idx);
    idx2name[to_string(idx)] = elem;
    idx += 1;
  }
  for (auto elem: P){
    answer.create_state(name2idx[elem], false);
  }
// 3
  for (auto state:answer.get_states()){
    set<string> tmp = idx2name[state];
    string tmp_state = *(tmp.begin());
    for (auto terminate: answer.get_alphabet()){
      if (d.has_trans(tmp_state, terminate)){
        answer.set_trans(state, terminate, name2idx[state2group[d.get_trans(tmp_state, terminate)]]);
      }
    }
  }
// 4-5
  for (auto tpl:state2group){
    if (d.is_initial(tpl.first)){
      answer.set_initial(name2idx[tpl.second]);
    }
    if (d.is_final(tpl.first)){
      answer.make_final(name2idx[tpl.second]);
    }
  }
//  7
  cout << name2idx[state2group[name_add]] << endl;
  answer.delete_state(name2idx[state2group[name_add]]);
  return answer;
}
