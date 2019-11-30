var cfg = require('./config.json');
var log = require('./logs');

var Sequelize = require('sequelize');

var serverCfg = cfg.serverCfg[cfg.databaseCfg];
exports.sequelize = new Sequelize(serverCfg.database, serverCfg.user, serverCfg.password, {
    host: serverCfg.dbhost,
    dialect: serverCfg.dialect,
    pool: {
        max: 5,
        min: 0,
        idle: 10000
    },
    logging: false
});

exports.channelinfo_db = require('./db/channelinfo_db');
exports.serverinfo_db = require('./db/serverinfo_db');
exports.noticeinfo_db = require('./db/noticeinfo_db');

var urlRoot = 'http://' + serverCfg.outhost + ":" + serverCfg.port;
log.logInfo("urlRoot", urlRoot)


exports.dbinit = function (callBack) {
    var self = this;
    self.channelinfo_db.initChannelInfo(
        function todo() {
            self.channelinfo_db.insertChannelInfo({
                channel: 'test_eos',
                version: '1.0.0'
            }, null);
            self.channelinfo_db.getChannelInfo({
                channel: 'test_eos',
                version: '1.0.0'
            }, null);

            self.channelinfo_db.updateChannelInfo({
                channel: 'test_eos',
                version: '1.0.0',
                assetbundle_url: 'http://blueone.oss-cn-hangzhou.aliyuncs.com/ab/Android__GameFun__R_1.0.0/AssetBundles/Android',
                auth_url: 'http://' + serverCfg.outhost + '/dbl_web/web/account/Server.php',
                comment_url: 'http://' + serverCfg.outhost,
                enable_debug: '1',
                feedback_url: '',
                white: '0',
                notice_url: urlRoot + '/noticeinfo',
                pay_url: 'http://' + serverCfg.outhost + ':8081',
                privacy_url: 'http://' + serverCfg.outhost + '/privacy.html',
                server_list_url: urlRoot + '/serverinfo',
                skip_update: 'show',
                termofservice_url: 'http://' + serverCfg.outhost + '/termofservice.html'
            }, null);
        }
    );

    self.serverinfo_db.initServerInfo(
        function todo() {
            // let count = 0;
            // for (let i = 0; i < 10; i++) {
            //     self.serverinfo_db.deleteById(i,
            //        function () {
            //             console.log("deleteById ok!");
            //             count+=10;
            //             self.serverinfo_db.updateServerInfo({
            //                 Id: i,
            //                 Name: '虚空测试服',
            //                 Ip: serverCfg.outhost,
            //                 Port: '9981',
            //                 Group: 'shenhe|android',
            //                 WhiteFlag: 0,
            //                 Status: count
            //             }, 
            //             function(){
            //                 console.log("insertServerInfo ok!");
            //             });
            //        }
            //     )
            // }

            self.serverinfo_db.updateServerInfo({
                Id: 1,
                Name: '虚空测试服',
                Ip: serverCfg.outhost,
                Port: '9981',
                Group: 'shenhe|android',
                WhiteFlag: 0,
                Status: 3
            }, null);

            self.serverinfo_db.insertServerInfo({
                Id: 2,
                Name: '虚空体验服',
                Ip: serverCfg.outhost,
                Port: '9982',
                Group: 'shenhe|android',
                WhiteFlag:0,
                Status:3
            }, null);

            // self.serverinfo_db.getServerList(null);
        }
    );

    self.noticeinfo_db.initNoticeInfo(
        function todo() {
            self.noticeinfo_db.insertNoticeInfo({
                channel: 'test_eos',
                language: 'English',
                notice: JSON.stringify(
                    [{
                            title: 'welcome diablo wolrd!',
                            content: 'this is a game base on the eos!'
                        },
                        {
                            title: 'welcome diablo wolrd!',
                            content: 'this is a game base on the eos!'
                        }
                    ]
                ),
            }, null);

            self.noticeinfo_db.insertNoticeInfo({
                channel: 'test_eos',
                language: 'Chinese',
                notice: JSON.stringify(
                    [{
                            title: '欢迎来到<虚空>!',
                            content: '<虚空>是一个在线游戏!'
                        },
                        {
                            title: '欢迎来到<虚空>!',
                            content: '<虚空>是一个在线游戏!'
                        }
                    ]
                ),
            }, null);

            self.noticeinfo_db.getNoticeInfo({
                channel: 'test_eos',
                language: 'Chinese'
            }, null);
        }
    )
}