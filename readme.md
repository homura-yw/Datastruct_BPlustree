
这是我的数据结构大作业。

算是自己实现的一个简单的玩具数据库吧。

肯定能够继续优化的，关键看我心情，如果能逐渐完善成为一个真正可用的数据库再好不过。

etc里的users.1v存的是用户数据，即账户和密码，可以进行修改添加。

database里面初始什么东西都没有，001.1v表示只有一个节点，root.1v存的是根节点的路径，add的时候会自动添加进去
如果你的database里面什么东西都没有，我觉得随便乱搞无所谓。
但是里面要是存的有东西，我还是建议别乱搞，要是节点的路径数据给整乱了那程序可能启动的时候会崩掉的，即使启动不崩掉也不知道什么时候能崩掉。
src里面就是源码。__main__.cpp就是程序的入口，BplusTree.h就是b+树的头文件，dbmgr.h就是对b+树头文件的封装
就这么简单说说吧