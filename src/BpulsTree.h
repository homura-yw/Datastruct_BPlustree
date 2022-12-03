#define __event
#ifdef __event
#include <cstdio>
#include <cstring>
#include <bits/stdc++.h>
constexpr int MAXNUM = 10;
//根据ORM思想创建的对象
int numfile = 1;
struct message{
    std::string Name;
    std::string Number;
    std::string Regist_date;
    std::string Seat;
    bool IsFee = false;
    message(std::string number, std::string name, std::string regist_date, std::string seat, bool isfee = false){
        Name = name;
        Number = number;
        Regist_date = regist_date;
        Seat = seat;
        IsFee = isfee;
    }
    bool operator < (const message & o) const{
        return Number < o.Number;
    }
    bool operator == (const message & o) const{
        return Number == o.Number;
    }
};

std::string getFileName(){
    numfile++;
    char str[4] = {(char)(numfile / 100 + 48), (char)(numfile % 100 / 10 + 48), (char)(numfile % 10 + 48), '\0'};
    return str;
}

std::set<int> memer;
/******************************这里是不知道为甚么要有的分割线****************************************/

/*
node形成的tree是对文件的B+树结构的映射
*/
int cnt(0), root(1);
struct node{
    std::string key;
    std::string path;
    int father = 0;
} memoryPool[114514];

struct cmpElement{
    std::string key;
    int idx;
    cmpElement(std::string str, int a):key(str), idx(a){}
};

struct nodecmp{
    bool operator () (const cmpElement& a, const cmpElement& b) const{
        return a.key < b.key;
    }
};

std::set<cmpElement, nodecmp> tree[114514];
/*************
 * 递归建树的过程
*/
cmpElement dfsbuild(std::string bptnode, int fa){//返回的是自己分配到的节点在内存池中的位置
    cnt++;
    memoryPool[cnt].path = bptnode;
    FILE *tt = fopen(("../database/" + memoryPool[cnt].path).c_str(), "r");
    memoryPool[cnt].father = fa;
    int isleaf, num;
    char str[20] = {0}, str2[20] = {0};
    fscanf(tt, "%d%d%s", &isleaf, &num, str);//第一个判断是否为叶节点， 第二个是孩子的数量，第三个是这个节点的key值
    memoryPool[cnt].key = str;
    if(isleaf == 1) return cmpElement(str, cnt);//叶节点每组的信息分别是：每组的key值(即number)， 姓名， 注册时期， 座位号，是否付费(01) 
    else {
        int temp = cnt;
        for(int i = 1; i <= num; i++){
            fscanf(tt, "%s%s", str, str2);//前一个是key，后一个是地址，即中间节点的每组结构就是这样
            tree[temp].insert(dfsbuild(str2, temp));
        }
        return cmpElement(str, temp);
    }
    fclose(tt);
}
/*------------------------这是处理非叶子节点的分裂操作的方法------------------*/
void split(node &treenode, int idx){
    /*----先更新节点----*/
    if(memer.find(idx) != memer.end()){
        memer.erase(memer.find(idx));
    }
    if(tree[idx].size() < 10){
        FILE* ops = fopen(("../database/" + treenode.path).c_str(), "w");
        fprintf(ops, "0 %d %s\n", (int)tree[idx].size(), treenode.key.c_str());
        for(cmpElement a : tree[idx]){
            fprintf(ops, "%s %s\n", a.key.c_str(), memoryPool[a.idx].path.c_str());
        }
        fclose(ops);
        return;
    }
    /*----再进行分裂----*/
    else {
        memoryPool[++cnt].path = getFileName() + ".1v";
        FILE *ops = fopen(("../database/" + treenode.path).c_str(), "w");
        int i = 0;
        for(auto it = tree[idx].begin(); i < 5; i++, it++){
            if(i == 4){
                fprintf(ops, "0 5 %s\n", it->key.c_str());
                treenode.key = it->key;
                break;
            }
        }
        i = 0;
        auto it = tree[idx].begin();
        while(i < 5){
            fprintf(ops, "%s %s\n", it->key.c_str(), memoryPool[it->idx].path.c_str());
            it++;
            i++;
        }
        auto it2 = it;
        while(it != tree[idx].end()){
            memoryPool[it->idx].father = cnt;
            tree[cnt].insert(cmpElement(it->key, it->idx));
            it++;
        }
        memoryPool[cnt].key = (--it)->key;
        FILE *next_file = fopen(("../database/" + memoryPool[cnt].path).c_str(), "w");
        fprintf(next_file, "0 5 %s\n", memoryPool[cnt].key.c_str());
        for(auto a : tree[cnt]){
            fprintf(next_file, "%s %s\n", a.key.c_str(), memoryPool[a.idx].path.c_str());
        }
        fclose(next_file);
        fclose(ops);
        tree[idx].erase(it2, tree[idx].end());
        int father = treenode.father;
        /*----判断有没有父节点----*/
        if(father != 0){
            memoryPool[cnt].father = father;
            auto it3 = tree[father].find(cmpElement(memoryPool[cnt].key, idx));
            tree[father].erase(it3);
            tree[father].insert(cmpElement(memoryPool[cnt].key, cnt));
            tree[father].insert(cmpElement(memoryPool[idx].key, idx));
            split(memoryPool[father], father);
        }
        else {
            int temp = cnt;
            memoryPool[++cnt].path = getFileName() + ".1v";
            tree[cnt].insert(cmpElement(memoryPool[temp].key, temp));
            tree[cnt].insert(cmpElement(memoryPool[idx].key, idx));
            treenode.father = cnt;
            memoryPool[temp].father = cnt;
            memoryPool[cnt].key = memoryPool[temp].key;
            root = cnt;
            split(memoryPool[cnt], cnt);
            FILE *r = fopen("../database/root.1v", "w");
            fprintf(r, "%s\n", memoryPool[cnt].path.c_str());
            fclose(r);
        }
    }
}

/*------------------------这是处理叶子节点的分裂操作的方法--------------------*/
void split(node &treenode, FILE *ops, std::set<message> &set2, int isleaf, int num, int idx){
    int CNT = 0, I = 0;
    std::set<message>::iterator it = set2.begin(), it2 = set2.end();
    std::string key = (*(--it2)).Number;
    for(auto i = set2.begin(); I < 5; i++,I++){
        if(I == 4) {
            fprintf(ops, "1 5 %s\n", i->Number.c_str());
            treenode.key = i->Number;
            break;
        }
    }
    while(CNT < 5){
        fprintf(ops, "%s %s %s %s %d\n", it->Number.c_str(), it->Name.c_str(), it->Regist_date.c_str(), it->Seat.c_str(), (int)it->IsFee);
        it++;
        CNT++;
    }
    fclose(ops);
    std::string filename = getFileName() + ".1v";
    FILE *file_next = fopen(("../database/" + filename).c_str(), "w");
    fprintf(file_next, "1 5 %s\n", key.c_str());
    while(it != set2.end()){
        fprintf(file_next, "%s %s %s %s %d\n", it->Number.c_str(), it->Name.c_str(), it->Regist_date.c_str(), it->Seat.c_str(), (int)it->IsFee);
        it++;
    }
    memoryPool[++cnt].path = filename;
    memoryPool[cnt].key = key;
    int father = treenode.father;
    int temp = cnt;
    fclose(file_next);
    if(father == 0){
        memoryPool[++cnt].key = memoryPool[temp].key;
        memoryPool[cnt].path = getFileName() + ".1v";
        treenode.father = cnt;
        memoryPool[temp].father = cnt;
        tree[cnt].insert(cmpElement(treenode.key, idx));
        tree[cnt].insert(cmpElement(memoryPool[temp].key, temp));
        root = cnt;
        FILE *r = fopen("../database/root.1v", "w");
        fprintf(r, "%s\n", memoryPool[root].path.c_str());
        fclose(r);
        split(memoryPool[root], root);
    }
    else {
        std::set<cmpElement, nodecmp>::iterator it2 = tree[father].find(cmpElement(key, 0));
        memoryPool[temp].father = father;
        temp = it2->idx;
        tree[father].erase(it2);
        tree[father].insert(cmpElement(treenode.key, temp));
        tree[father].insert(cmpElement(key, cnt));
        split(memoryPool[father], father);
    }
}
//更新非叶子节点的信息
void update(node& treenode, int idx){
    auto it = tree[idx].end();
    it--;
    int temp = it->idx;
    tree[idx].erase(it);
    tree[idx].insert(cmpElement(treenode.key, temp));
}

struct B_Plus_Tree{
    B_Plus_Tree(){
        memset(memoryPool, 0, sizeof(memoryPool));
        FILE *t = fopen("../database/root.1v", "r");
        char str[20] = {0};
        fscanf(t, "%s", str);
        dfsbuild(str, 0);
        numfile = cnt;
    }

    message search(std::string key){
        int now = root;
        while(tree[now].size() != 0){
            std::set<cmpElement,nodecmp>::iterator i = tree[now].lower_bound(cmpElement(key, 0));
            if(i == tree[now].end()){
                return message("You", "Hasn't", "Registed", "Compitition", false);
            }
            now = i->idx;
        }
        FILE* ips = fopen(("../database/" + memoryPool[now].path).c_str(), "r");
        int num, isleaf;
        char str[4][20];
        fscanf(ips, "%d%d%s", &isleaf, &num, str[0]);
        int isfee;
        while(num--){
            fscanf(ips, "%s%s%s%s%d", str[0], str[1], str[2], str[3], &isfee);
            if(strcmp(str[0], key.c_str()) == 0){
                fclose(ips);
                return message(str[0], str[1], str[2], str[3], isfee != 0);
            }
        }
        fclose(ips);
        return message("You", "Hasn't", "Registed", "Compitition", false);
    }
    
    void Add(message val){//插入操作
        int now = root;
        while(tree[now].size() != 0){//找到叶子节点
            std::set<cmpElement, nodecmp>::iterator i = tree[now].lower_bound(cmpElement(val.Number, 0));
            if(i == tree[now].end()) {
                i--;
                memoryPool[now].key = val.Number;
                update(memoryPool[now], now);
                memer.insert(now);
            }
            now = i->idx;
        }
        char key[20] = {0};
        int isleaf, num;
        FILE *tt = fopen(("../database/" + memoryPool[now].path).c_str(), "r");
        fscanf(tt, "%d%d%s", &isleaf, &num, key);
        std::set<message> set2;
        char str[4][20];
        int isfee;
        memset(str, 0, sizeof(str));
        for(int i = 1; i <= num; i++){//将文件内容读入内存
            fscanf(tt, "%s%s%s%s%d", str[0], str[1], str[2], str[3], &isfee);//将叶子节点的内容读入
            set2.insert(message(str[0], str[1], str[2], str[3], isfee));
        }
        set2.insert(val);
        num = set2.size();
        auto it = set2.end();
        it--;
        memoryPool[now].key = (*it).Number;
        FILE *ops = fopen(("../database/" + memoryPool[now].path).c_str(), "w");
        if(num >= 10){//需要调整树结构时
            split(memoryPool[now], ops, set2, isleaf, num, now);
        }
        else {//不需要时,直接打印即可
            fprintf(ops, "%d %d %s\n", isleaf, num, memoryPool[now].key.c_str());
            for(message ojb : set2){
                fprintf(ops, "%s %s %s %s %d\n", ojb.Number.c_str(), ojb.Name.c_str(), ojb.Regist_date.c_str(), ojb.Seat.c_str(), (int)ojb.IsFee);
            }
            fclose(ops);
        }
    }
    void Update(std::string key){
        int now = root;
        while(tree[now].size() != 0){
            std::set<cmpElement,nodecmp>::iterator i = tree[now].lower_bound(cmpElement(key, 0));
            if(i == tree[now].end()){
                return;
            }
            now = i->idx;
        }
        FILE* ips = fopen(("../database/" + memoryPool[now].path).c_str(), "r");
        int num, isleaf;
        char k[20] = {0};
        char str[4][20];
        fscanf(ips, "%d%d%s", &isleaf, &num, k);
        int isfee;
        std::set<message> s;
        for(int i = 0; i < num; i++){
            fscanf(ips, "%s%s%s%s%d", str[0], str[1], str[2], str[3], &isfee);
            s.insert(message(str[0], str[1], str[2], str[3], isfee));
        }
        fclose(ips);
        auto it = s.find(message(key, " ", " ", " ", 0));
        if(it == s.end()) return;
        else {
            message msg = message(it->Number, it->Name, it->Regist_date, it->Seat, true);
            s.erase(it);
            s.insert(msg);
            FILE* ops = fopen(("../database/" + memoryPool[now].path).c_str(), "w");
            fprintf(ops, "1 %d %s\n", num, k);
            for(auto a : s){
                fprintf(ops, "%s %s %s %s %d\n", a.Number.c_str(), a.Name.c_str(), a.Regist_date.c_str(), a.Seat.c_str(), (int)a.IsFee);
            }
            fclose(ops);
            return;
        }
    }
};

#endif