#include <cqcppsdk/cqcppsdk.hpp>
#include <iostream>
#include <fstream>
#include <io.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <random>
#include <string>

using namespace cq;
using MessageSegment = message :: MessageSegment;
using cq :: utils :: s2ws;
using cq :: utils :: ws2s;
using cq :: utils :: ansi;

int getrandint(int l, int r) {//获取[l, r]的随机整数
    return rand() % (r - l + 1) + l;
}

int min(int a, int b) {
    return a < b ? a : b;
}

long gettime() {
    time_t t;
    t = time(NULL);
    long time_stamp = time(&t);
    return time_stamp;
}

const std :: string buff_list[] = {"空", "傲", "怒", "惰", "反", "冰", "欧", "魂"};
const std :: string burn_random_buff_list[] = {"空", "无", "无", "无", "无", "无", "无", "无", "欧", "惰", "魂"};

CQ_INIT {
    on_enable([] {
        logging :: info("启用", "插件已启用");
        on_group_message([](const auto &event) {
            //读入配置文件
            srand((unsigned)time(NULL));
            const std :: string data_file = ansi(dir :: app(to_string(event.user_id)) + "data.ini");
            
            long last_sign_time = 0;
            int alive = 0;
            std :: string kun_name = "none";
            std :: string buff = "无";
            long long weight = 0;
            int eggs = 0;
            int coins = 0;
            int pill = 0;
            int relife = 0;


            if(_access(data_file.c_str(), 0) == -1) {//当文件不存在，初始化文件。
                std :: ofstream data;
                data.open(data_file);
                data << "0\n0\nnone\n无\n0\n0\n0\n0\n0\n";
                data.close();
            } else {//当文件存在，读取。
                std :: ifstream data;
                data.open(data_file);
                data >> last_sign_time >> alive >> kun_name >> buff >> weight >> eggs >> coins >> pill >> relife;
                data.close();
            }
            //语句判断
            if (event.message == "/sign") {
                if (last_sign_time / 86400 == gettime() / 86400) {
                    send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 今天你都签过到了，企图重复签到？10金币，扣走了！");
                    coins -= 10;
                } else {
                    int addEggs = getrandint(1, 4) * 5;
                    int addCoins = getrandint(1, 20) * 5;
                    eggs += addEggs;
                    coins += addCoins;
                    send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 签到成功！\n获得奖励：\n" + to_string(addEggs) + "颗蛋\n" + to_string(addCoins) +"枚金币\n ---------- \n当前数量：\n蛋*" + to_string(eggs) + "\n金币*" + to_string(coins));
                    last_sign_time = gettime();
                }
            }

            if (event.message.find("/query") == 0) {
                if (event.message.size() == 6 || (event.message.find("[CQ:at,qq=") == std :: string :: npos && event.message.find("@s") == std :: string :: npos)) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你没有指定查询的对象！\n使用/query @someone 规定查询对象。（也可以使用@s表示自己）");
                else {
                    long long query_user = 0;
                    if (event.message.find("@s") != std :: string :: npos) query_user = event.user_id;
                    else {
                        int startpos = event.message.find("=");
                        int endpos = event.message.find_last_of("]");
                        try {
                            query_user = std :: stoll(event.message.substr(startpos + 1, endpos - startpos - 1));
                        } catch (std :: exception& e) {
                            send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 查询命令不合法！\n请检查你的查询命令格式。正确格式应该为：/query @someone 或 /query @s（表示查询自己）");
                        }
                    }
                    if (query_user) {
                        std :: string query_data_file = ansi(dir :: app(to_string(query_user)) + "data.ini");
                        if (_access(query_data_file.c_str(), 0) == -1) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 这个人都还没开始玩养鲲游戏呢。\n可以邀请Ta来一起玩呀QAQ");
                        else {
                            long query_user_last_sign_time = 0;
                            int query_user_alive = 0;
                            std :: string query_user_kun_name = "none";
                            std :: string query_user_buff = "无";
                            long long query_user_weight = 0;
                            int query_user_eggs = 0;
                            int query_user_coins = 0;
                            int query_user_pill = 0;
                            int query_user_relife = 0;

                            std :: ifstream data;
                            data.open(query_data_file);
                            data >> query_user_last_sign_time >> query_user_alive >> query_user_kun_name >> query_user_buff >> query_user_weight >> query_user_eggs >> query_user_coins >> query_user_pill >> query_user_relife;
                            data.close();
                            if (!query_user_last_sign_time) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 这个人都还没开始玩养鲲游戏呢。\n可以邀请Ta来一起玩呀QAQ");
                            else send_group_message(event.group_id, MessageSegment :: at(event.user_id) + "\n" + MessageSegment :: at(query_user) + "的鲲数据信息：\n名字：" + query_user_kun_name + "\n体重：" + to_string(query_user_weight) + "公斤\n属性：" + query_user_buff + (alive ? "\n他还有的物件：\n" : "[已死]\n他还有的物件：\n") + to_string(query_user_eggs) + "颗蛋\n" + to_string(query_user_coins) + "枚金币\n" + to_string(query_user_pill) + "颗药\n" + to_string(query_user_relife) + "枚九转还魂丹");
                        }
                    }
                }
            }

            if (event.message.find("/hatch") == 0) {
                if (alive) {
                    send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你似乎有一条名为" + kun_name + "的鲲！");
                } else {
                    if (event.message.size() == 6) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你没有指定孵化的鲲的名字！\n使用/hatch name （name表示你的鲲的名字）指定名字。");
                    else {
                        alive = 1;
                        kun_name = event.message.substr(7);
                        weight = getrandint(20, 100);
                        buff = burn_random_buff_list[getrandint(1, sizeof(burn_random_buff_list) / sizeof(burn_random_buff_list[0]) - 1)];
                        send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 恭喜你获得了一条名为" + kun_name + "的" + to_string(weight) + "公斤" + buff + "属性鲲！");
                    }
                }
            }

            if(event.message.find("/feed") == 0) {
                if (alive) {
                    int FeedEggs = 0;
                    if (event.message == "/feed") FeedEggs = 1;
                    else {
                        try {
                            FeedEggs = std :: stoi(event.message.substr(6));
                        } catch (std :: exception& e) {
                            send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 喂食命令不合法！\n请检查你的喂食命令格式。正确格式应该为：/feed [num]（[num]表示喂食数量） 或 /feed（表示喂1个蛋，相当于/feed 1）");
                        }
                    }
                    if (FeedEggs) {
                        if (FeedEggs > eggs) {
                            send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 哎呀，你还没有这么多的蛋！你目前有" + to_string(eggs) + "颗蛋，还差" + to_string(FeedEggs - eggs) + "颗蛋。购买这么多蛋需要" + to_string((FeedEggs - eggs) * 5) + "枚金币！");
                        } else if (FeedEggs <= 0) {
                            send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 喂负数颗蛋？你把我搞不会了。");
                        } else {
                            long long addWeight = FeedEggs * getrandint(10, 30);
                            weight += addWeight;
                            eggs -= FeedEggs;
                            send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 哦吼，你喂了" + to_string(FeedEggs) + "颗蛋给你的" + kun_name + "，它很开心，涨了" + to_string(addWeight) + "公斤的体重！\n当前你的" + kun_name + "的体重是：" + to_string(weight) + "公斤\n你还剩" + to_string(eggs) + "颗蛋！");
                        }
                    }
                } else send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你没有鲲。\n你喂了个寂寞。" + MessageSegment :: face(194));
            }

            if (event.message.find("/buy") == 0) {
                if (event.message.find("/buy egg") == 0) {
                    int buynum = 0;
                    if (event.message == ("/buy egg")) buynum = 1;
                    else {
                        try {
                            buynum = std :: stoi(event.message.substr(9));
                        } catch (std :: exception& e) {
                            send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 购买命令不合法！\n请检查你的购买命令格式。正确格式应该为：/buy [name] [num]（[name]表示购买物品的名称，[num]表示购买数量） 或 /buy [name]（表示买一个这种物品，相当于/buy [name] 1）\n目前支持购买的物品：\n物品\t[name]\t价格\n蛋\tegg\t5金币/颗");
                        }
                    }
                    if (buynum) {
                        if (buynum < 0) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你买负数个蛋是啥意思" + MessageSegment :: face(32));
                        else if (coins < buynum * 5) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 买东西要先看自己的钱够不够。买这么多蛋需要" + to_string(buynum * 5) + "枚金币，但你只有" + to_string(coins) + "枚金币，你还需要赚" + to_string(buynum * 5 - coins) + "枚金币才能来买这么多蛋哦。" + MessageSegment :: face(22));
                        else {
                            int costCoins = buynum * 5;
                            eggs += buynum;
                            coins -= costCoins;
                            send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你花了" + to_string(costCoins) + "枚金币，买来了" + to_string(buynum) + "颗蛋！\n目前你有" + to_string(eggs) + "颗蛋\n你还剩" + to_string(coins) + "枚金币！");
                        }
                    }
                }
            }

            if (event.message.find("/smash") == 0) {
                int smashnum = 0;
                if (event.message == "/smash") smashnum = 1;
                else {
                    try {
                        smashnum = std :: stoi(event.message.substr(7));
                    } catch (std :: exception& e) {
                        send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 砸蛋命令不合法！\n请检查你的砸蛋命令格式。正确格式应该为：/smash [num]（[num]表示砸蛋数量） 或 /smash（砸一颗蛋）\n目前可以砸出的物品：\n蛋\n金币\n药\n九转还魂丹");
                    }
                }
                if (smashnum) {
                   if (smashnum > eggs) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 砸蛋要先看自己的蛋够不够。你要砸" + to_string(smashnum) + "颗蛋，但你只有" + to_string(eggs) + "颗蛋，你还需要购买" + to_string(smashnum - eggs) + "颗蛋才能来砸这么多蛋哦。买这些蛋需要" + to_string((smashnum - eggs) * 5) + "枚金币。" + MessageSegment :: face(22));
                    else {
                        int addEggs = 0;
                        int addCoins = 0;
                        int addpill = 0;
                        int addRelife = 0;
                        const int randomSmash[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4};
                        const int o_randomSmash[] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4};
                        for (int i = 1; i <= smashnum; i++) {
                            int type;
                            if (buff == "欧") type = o_randomSmash[getrandint(1, sizeof(o_randomSmash) / sizeof(o_randomSmash[0]) - 1)];
                            else type = randomSmash[getrandint(1, sizeof(randomSmash) / sizeof(randomSmash[0]) - 1)];
                            if (type == 1) addEggs += getrandint(1, 3);
                            else if (type == 2) addCoins += getrandint(1, 6) * 5;
                            else if (type == 3) addpill += getrandint(1, 4) * 5;
                            else if (type == 4) addRelife++;
                        }
                        eggs -= smashnum;
                        eggs += addEggs;
                        coins += addCoins;
                        pill += addpill;
                        relife += addRelife;
                        if (!addEggs && !addCoins && !addpill && !addRelife) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你砸的" + to_string(smashnum) + "颗蛋中，都是尚未成型的鲲宝宝。" + MessageSegment :: face(97));
                        else {
                            std :: string sendMessage = MessageSegment :: at(event.user_id) + " 你砸了" + to_string(smashnum) + "颗蛋，获得了以下物品：\n";
                            if (addEggs) sendMessage += "蛋*" + to_string(addEggs) + "\n";
                            if (addCoins) sendMessage += "金币*" + to_string(addCoins) + "\n";
                            if (addpill) sendMessage += "药*" + to_string(addpill) + "\n";
                            if (addRelife) sendMessage += "九转还魂丹*" + to_string(addRelife) + "\n";
                            sendMessage += "你目前有：\n蛋*" + to_string(eggs) + "\n金币*" + to_string(coins) + "\n药*" + to_string(pill) + "\n九转还魂丹*" + to_string(relife);
                            send_group_message(event.group_id, sendMessage);
                        }
                    }
                }
            }

            if (event.message == "/relife") {
                if (alive) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你的" + kun_name + "还活着呢，不需要九转还魂丹复活的说。" + MessageSegment :: face(22));
                else if (!relife) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 虽然你的" + kun_name + "死了，可你的九转还魂丹也没了。你可以选择砸蛋获取九转还魂丹（挺难得到的），也可以选择放弃这条生命，重新孵化一条鲲。" + MessageSegment :: face(5));
                else {
                    relife--;
                    alive = 1;
                    int RecoverWeightPercent = getrandint(30, 70);
                    if (buff == "魂") {
                        weight *= 0.75;
                        send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 普通的鲲复活后只能保留30%~70%的体重，不过由于你的" + kun_name + "不是普通鲲，而是魂属性鲲，本来就没有肉体，所以这次你的" + kun_name + "仅仅损失了25%的体重。你的" + kun_name + "现在的体重是" + to_string(weight) + "公斤。" + MessageSegment :: face(13));
                    } else if (buff == "欧") {
                        int o_addRecoverWeightPercent = getrandint(6, 8);
                        weight *= (RecoverWeightPercent + o_addRecoverWeightPercent);
                        weight /= 100;
                        send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你的欧属性鲲虽然死了，但是欧气没有消失。复活后的体重相比于没有欧气增加了" + to_string(o_addRecoverWeightPercent) + "个百分点！你的" + kun_name + "的体重可以恢复" + to_string(RecoverWeightPercent + o_addRecoverWeightPercent) + "%~你的" + kun_name + "现在的体重是" + to_string(weight) + "公斤。" + MessageSegment :: face(99));
                    } else {
                        weight *= RecoverWeightPercent;
                        weight /= 100;
                        send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你的" + kun_name + "复活辣，不过……你的" + kun_name + "只能恢复" + to_string(RecoverWeightPercent) + "%的体重。所以你的" + kun_name + "现在的体重是" + to_string(weight) + "公斤。" + MessageSegment :: face(36));
                    }
                    int addCoins = getrandint(4, 8) * 5;
                    coins += addCoins;
                    send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 哦对了，你的" + kun_name + "因为感谢你复活了他，吐出了" + to_string(addCoins) + "枚金币。正道的光~" + MessageSegment :: face(13) + "\n你现在有" + to_string(coins) + "枚金币！");
                }
            }

            if (event.message == "/pill") {
                if (!alive) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你没有鲲。\n你幻化了个寂寞。" + MessageSegment :: face(194));
                else if (!pill) send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你没有药丸了！\n通过砸蛋可以获得药丸哦！" + MessageSegment :: face(176));
                else {
                    int loseWeight = getrandint(10, 100);
                    weight -= loseWeight;
                    pill--;
                    int success = (getrandint(1, 4) == 1);
                    if (success && buff == "魂") success = (getrandint(1, 10) != 1);
                    if (success) {
                        std :: string new_buff = buff_list[getrandint(1, sizeof(buff_list) / sizeof(buff_list[0]) - 1)];
                        if (new_buff == buff) {
                            pill++;
                            send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 虽然你幻化成功了，但是幻化后的属性似乎和你之前的属性是一样的，都是" + buff + "……" + MessageSegment :: face(27) + "\n这样吧，把药丸退给你吧。你目前还是剩" + to_string(pill) + "颗药丸。\n不过你的" + kun_name + "在幻化时失去的" + to_string(loseWeight) + "公斤的体重我是没办法还回来啦。你现在鲲的体重是" + to_string(weight) + "公斤。" + MessageSegment :: face(22));
                        } else {
                            buff = new_buff;
                            send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 恭喜，你的" + kun_name + "成功幻化为了" + buff + "属性！" + MessageSegment :: face(99) + "\n你还剩下" + to_string(pill) + "颗药丸~\n不过幻化成功也是需要付出代价滴，你的" + kun_name + "在幻化时损失了" + to_string(loseWeight) + "公斤的体重哦。它还剩下" + to_string(weight) + "公斤。");
                        }
                    } else send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 啊哦。幻化失败。" + MessageSegment :: face(107) + "\n你还剩下" + to_string(pill) + "颗药丸。\n你的" + kun_name + "在幻化时损失了" + to_string(loseWeight) + "公斤的体重。它还剩下" + to_string(weight) + "公斤。" + MessageSegment :: face(22));
                }
            }

            if (weight < 0) {
                alive = 0;
                weight = 0;
                send_group_message(event.group_id, MessageSegment :: at(event.user_id) + " 你的" + kun_name + "已经没了体重，死了！" + MessageSegment :: face(26));
            }
            //更新配置文件
            std :: ofstream data;
            data.open(data_file);
            data << last_sign_time << std :: endl << alive << std :: endl << kun_name << std :: endl << buff << std :: endl << weight << std :: endl << eggs << std :: endl << coins << std :: endl << pill << std :: endl << relife << std :: endl;
            data.close();
        });
    });
}