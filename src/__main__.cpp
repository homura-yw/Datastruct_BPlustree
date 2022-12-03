#include "dbmgr.h"

using namespace std;

int main(){
    DataBaseLinkPool *dblp = new DataBaseLinkPool();
    dblp->Connection("Elaina1v", "114514");
    dblp->run("Elaina1v");
}