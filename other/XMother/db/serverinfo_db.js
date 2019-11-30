var Sequelize = require('sequelize');
var log = require('../logs');
var db = require('../dbproxy');

////////////////////////////////
var TServerInfo = db.sequelize.define('server_info', {
    Id: {
        type: Sequelize.BIGINT,
        allowNull: false,
        primaryKey: true,
    },
    Name: {
        type: Sequelize.STRING,
        allowNull: false,
    },
    Ip: {
        type: Sequelize.STRING,
        allowNull: false
    },
    Port: {
        type: Sequelize.STRING,
        allowNull: false
    },
    Group: {
        type: Sequelize.STRING,
        defaultValue: 1
    },
    Status: {
        type: Sequelize.TINYINT,
        defaultValue: 1
    },
    WhiteFlag: {
        type: Sequelize.TINYINT,
        defaultValue: 0
    }
});

//---------------------------------
exports.initServerInfo = function (callBack) {
    TServerInfo.sync({
        alter: true
    }).then(() => {
        log.logInfo("initServerInfo create done! ");
        if (callBack) {
            callBack();
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}

exports.insertServerInfo = function (serverInfo, callBack) {
    return TServerInfo.findOrCreate({
        where: {
            Id: serverInfo.Id,
        },
        defaults: serverInfo

    }).then(([element, created]) => {
        // console.log(element.get({
        //     plain: true
        // }))
        // console.log(created)
        if (callBack) {
            callBack();
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}


exports.deleteById = function (id, callBack) {

    return TServerInfo.destroy({
        where: {
            Id: id
        }
    }).then((result) => {
        callBack();
    }).catch((e) => {
        log.logInfo(e);
    });
}


exports.updateServerInfo = function (serverInfo, callBack) {
    return TServerInfo.findOne({
            where: {
                Id: serverInfo.Id
            }
        })
        .then(function (obj) {

            if (obj) {
                TServerInfo.update(
                    serverInfo, {
                        where: {
                            Id: serverInfo.Id
                        }
                    }).then(([element]) => {
                    //log.logInfo("updateServerInfo update done! " + JSON.stringify(element));
                    if (callBack) {
                        callBack(element);
                    }
                }).catch((e) => {
                    log.logInfo(e.stack);
                });
            } else { // insert
                TServerInfo.create(serverInfo).then((element) => {
                    //log.logInfo("updateServerInfo create done! " + JSON.stringify(element));
                    if (callBack) {
                        callBack(element);
                    }
                }).catch((e) => {
                    log.logInfo(e);
                });
            }
        }).catch((e) => {
            log.logInfo(e);
        });

}


exports.getServerInfo = function (sid, callBack) {
    return TServerInfo.findAll({
        where: {
            Id: sid
        }
    }).then(([element]) => {
        log.logInfo("getServerInfo done! " + JSON.stringify(element));
        if (callBack) {
            callBack(element);
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}

exports.getServerList = function (callBack) {
    return TServerInfo.findAll({
        where: {}
    }).then((result) => {
        log.logInfo("getServerList done! " + JSON.stringify(result));
        if (callBack) {
            callBack(result);
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}