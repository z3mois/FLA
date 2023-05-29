#include "api.hpp"
#include <string>
#include <iostream>
#include <stack>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <queue>


using namespace std;

class Node{
public:
    set<int> firstpos;
    set<int> lastpos;
    bool nullable = NULL;
    Node* left = NULL;
    Node* right = NULL;
    char symbol;
    Node(){};
    Node( char symb, set<int> fp = {}, set<int> lp = {}, bool nul = false, 
    Node* l = NULL, Node* r = NULL): 
         firstpos(fp), lastpos(lp), nullable(nul), left(l), right(r), symbol(symb) {};
};
int idx_all = 0;
int idx_symb = 0;
Node* A (string st);
Node* D (string st){
    Node* temp;
    if (st[idx_all] == '(') {
        idx_all++;
        temp = A(st);
        idx_all++;
    } else {
        cout << st[idx_all];
        temp = new Node(st[idx_all], {idx_symb}, {idx_symb}, false, NULL, NULL);
        idx_all++;
        idx_symb++;
    }
    return temp;
}

Node* C (string st){
    auto tmp = D(st);
    while (st[idx_all] == '*'){
        idx_all++;
        tmp =  new Node('*', {}, {}, true, tmp, NULL);
    }
    return tmp;
}
Node* B (string st){
    Node* tmp;
    if (isalpha(st[idx_all]) || isdigit(st[idx_all]) || st[idx_all] == '(' || st[idx_all] == '#'){
        tmp = C(st);
        while (isalpha(st[idx_all]) || isdigit(st[idx_all]) || st[idx_all] == '(' || st[idx_all] == '#'){
            auto many = C(st);
            tmp = new Node('.', {}, {}, false, tmp, many);
        }
        return tmp;
    }
    // Node* nulls = new Node(st[idx_all], {idx_symb}, {idx_symb}, true, NULL, NULL);
    return new Node(st[idx_all], {}, {}, true, NULL, NULL);
    
}
Node* A (string st){
    Node* tmp = B(st);
    while (st[idx_all] == '|'){
        idx_all++;
        Node* news = B(st);
        tmp = new Node('|',{},{},false, tmp, news);
    }
    return tmp;
}
Node* S(string st){
    st = '('+st+')' + '#';
    return A(st);
}

// https://translated.turbopages.org/proxy_u/en-ru.ru.82d6e354-646b9599-95e1ddde-74722d776562/https/stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
void printBT(const std::string& prefix, const Node* node, bool isLeft)
{
    if( node != nullptr )
    {
        cout << prefix;
        cout << (isLeft ? "├──" : "└──" );
        cout << node->symbol << endl;
        printBT( prefix + (isLeft ? "│   " : "    "), node->left, true);
        printBT( prefix + (isLeft ? "│   " : "    "), node->right, false);
    }
}

void printBT(const Node* node)
{
    printBT("", node, false);    
}
void base_print(Node* tree){
    if (tree == NULL){
        return;
    }
    cout << tree->symbol << endl;
    base_print(tree->left);
    base_print(tree->right);
}
void sett(Node* tree){
    if (tree->left == NULL && tree->right == NULL){
        return;
    }
    if (tree->symbol == '|'){
        sett(tree->left);
        sett(tree->right);
        tree->nullable = tree->left->nullable || tree->right->nullable;
        set<int> fp(tree->left->firstpos);
        fp.insert(tree->right->firstpos.begin(), tree->right->firstpos.end());
        tree->firstpos = fp; 
        set<int> lp(tree->left->lastpos);
        lp.insert(tree->right->lastpos.begin(), tree->right->lastpos.end());
        tree->lastpos = lp;
    }else if (tree->symbol == '.')
    {
        sett(tree->left);
        sett(tree->right);
        tree->nullable = tree->left->nullable && tree->right->nullable;
        if (tree->left->nullable){
            set<int> fp(tree->left->firstpos);
            fp.insert(tree->right->firstpos.begin(), tree->right->firstpos.end());
            tree->firstpos = fp;            
        }else{
            tree->firstpos = tree->left->firstpos;
        }
        if (tree->right->nullable){
            set<int> lp(tree->left->lastpos);
            lp.insert(tree->right->lastpos.begin(), tree->right->lastpos.end());
            tree->lastpos = lp;
        }else{
            tree->lastpos = tree->right->lastpos;
        }
    }else if (tree->symbol == '*')
    {
        sett(tree->left);
        tree->nullable = true;
        tree->firstpos = tree->left->firstpos;
        tree->lastpos = tree->left->lastpos;
    }  
}
vector<set<int>> follow_position;
void follow_pos(Node* tree){
    if (tree->left == NULL && tree->right == NULL){
        return;
    }
    if (tree->symbol == '.'){
        for (auto i:tree->left->lastpos){
            follow_position[i].insert(tree->right->firstpos.begin(), tree->right->firstpos.end());
        }
        follow_pos(tree->left);
        follow_pos(tree->right);
    }else 
    if (tree->symbol == '*'){
        for (auto i:tree->left->lastpos){
            follow_position[i].insert(tree->left->firstpos.begin(), tree->left->firstpos.end());
        }
        follow_pos(tree->left);
    } else if (tree->symbol == '|'){
        follow_pos(tree->left);
        follow_pos(tree->right);   
    }
}
vector<int> finder(string st, char sembol, set<int> Ra){
    vector<int> answer;
    int idx = 0;
    for (int i =0; i < st.size(); i++){
        if (st[i] == sembol && Ra.count(idx) != 0){
            answer.push_back(idx++);
            // cout << sembol << " " << idx << endl;
            // idx++;
        }else if (isalpha(st[i]) || isdigit(st[i]) || st[i] == '#')
        {
            idx++;
        }
        
    }
    return answer;
}
void delete_node(Node* tree){
    if (tree == NULL){
        return;
    }
    delete_node(tree->left);
    delete_node(tree->right);
    delete tree;
}
DFA re2dfa(const std::string &st)
{
    if (st.empty()){
        DFA answer = DFA("s");
        answer.create_state("0", true);
        answer.set_initial("0");
        return answer;
    }
    // cout << "dasdsda" << endl;
    Node* tree = S(st);
    cout << endl<< idx_all << " =idx_all" << endl;
    sett(tree);
    follow_position.resize(idx_symb);
    // cout << "ok" << follow_position.size()<< endl;
    follow_pos(tree);
    for (auto elem:follow_position){
        for (auto item:elem){
            cout << item << " ";
        }
        cout << endl;
    }
    string q_0 = "";
    for (auto elem:tree->firstpos){
        if (elem == idx_symb-1){
            q_0+='#';
        }
        q_0 += to_string(elem) + '}';
    }
    set<string> Q;
    Q.insert(q_0);
    set<string> markered;
    // cout << "TREE";
    // base_print(tree);
    map<string, set<int>> str2set;
    str2set[q_0] = tree->firstpos;
    printBT(tree);
    delete_node(tree);
    DFA answer = DFA(Alphabet(st));
    answer.create_state(q_0, false);
    answer.set_initial(q_0);
    while (markered.size() != Q.size()){
        set<string> diff;
        set_difference(Q.begin(), Q.end(), markered.begin(), markered.end(),
            inserter(diff, diff.end()));
        string R = *(diff.begin());
        markered.insert(R);
        for (auto terminate:Alphabet(st)){
            set<int> S;
            for (int p_i:finder(st, terminate, str2set[R])){
                S.insert(follow_position[p_i].begin(), follow_position[p_i].end());
            }
            if (!S.empty()){
                string ss="";
                for (auto elem:S){ 
                    if (elem == idx_symb-1){
                        ss+='#';
                    }
                    ss += to_string(elem) + '}';
                }

                if (Q.count(ss) == 0){
                    Q.insert(ss);
                    str2set[ss] = S;
                    answer.create_state(ss, false);
                }
                
                answer.set_trans(R, terminate, ss);
            }
        }
    }
    for (auto elem: Q){
        if (elem.find('#') != elem.npos){
            answer.make_final(elem);
        }
    }
    // printBT(tree);
    return answer;
}
