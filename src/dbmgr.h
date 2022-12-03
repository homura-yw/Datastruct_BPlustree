#define Ev
#ifdef Ev
#include <bits/stdc++.h>
#include <cstdio>
#include <ctime>
#include <cstring>
#include "BpulsTree.h"

std::string erweima = "## ## ###\n### ## ##\n## ### ##\n# # # ###\n#### ## #\n";

void stdPrint(std::string msg, float second = 1){
    int minisec = (int)(second * 1000);
    system("cls");
    printf(msg.c_str());
    int t = clock(), temp = t;
    while(temp < t + minisec){
        temp = clock();
    }
    system("cls");
}

struct DataBaseLinkPool{
    bool isPassed = false;
    bool Connection(std::string userName, std::string PassWord){
        FILE *ptr = fopen("../etc/users.1v", "r");
        char un[20] = {0}, pwd[20] = {0};
        int num;
        fscanf(ptr, "%d", &num);
        int cnt = 0;
        bool Flag = false;
        do{
            if(cnt >= num) {
                Flag = true;
                break;
            } 
            fscanf(ptr, "%s%s", un, pwd);
            cnt++;
        }while(strcmp(un, userName.c_str()) != 0);
        if(Flag){
            stdPrint("UserName Wrong!!");
            return false;
        }
        if(strcmp(pwd, PassWord.c_str()) == 0){
            isPassed = true;
            stdPrint("Connect Succeed!!");
            return true;
        }
        else {
            stdPrint("Check Your Password!!");
            return false;
        }
    }

    void run(std::string key){
        if(!isPassed){
            stdPrint("No Connection!!");
            return;
        }
        stdPrint("System Run!!");
        B_Plus_Tree bpt = B_Plus_Tree();
        while(true){
            message msg = bpt.search(key);
            if(msg.Number == "You"){
                std::cout<<msg.Number<<' '<<msg.Name<<' '<<msg.Regist_date<<' '<<msg.Seat<<std::endl;
                std::cout<<"Would you regist now? input Y or N"<<std::endl;
                char cmd;
                std::cin>>cmd;
                if(cmd == 'Y'){
                    stdPrint("loading.");
                    stdPrint("loading..");
                    stdPrint("loading...");
                    std::cout<<"Input Your Name(In English,please QWQ):"<<std::flush;
                    std::string name;
                    std::cin>>name;
                    bpt.Add(message(key, name, "2012/07/20", "114514", false));
                    continue;
                }
                else if(cmd == 'N'){
                    stdPrint("Exiting.");
                    stdPrint("Exiting..");
                    stdPrint("Exiting...");
                    break;
                }
                else continue;
            }
            else {
                stdPrint("loading.");
                stdPrint("loading..");
                stdPrint("loading...");
                std::cout<<"number\t\tname\t\tregist_date\t\tseat\t\tIsFeeOrNot"<<std::endl;
                std::cout<<(msg.Number + "\t" + msg.Name + "\t" + msg.Regist_date + "\t" + msg.Seat + "\t" + (msg.IsFee ? "true" : "false"))<<std::endl;
                if(msg.IsFee){
                    std::cout<<"You have Pay , press any to exit"<<std::endl;
                    getchar();getchar();
                    break;
                }
                else {
                    std::cout<<"You haven't Pay, press Y to Pay, or press any to exit"<<std::endl;
                    char cmd;
                    std::cin>>cmd;
                    if(cmd == 'Y'){
                        stdPrint(erweima, 2);
                        stdPrint(erweima + "Nothing but erweima To Pay", 2);
                        bpt.Update(msg.Number);
                        continue;
                    }
                    else break;
                }
            }
        }
    }
};

#endif