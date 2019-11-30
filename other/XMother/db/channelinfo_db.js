var Sequelize = require('sequelize');
var log = require('../logs');
var db = require('../dbproxy');

////////////////////////////////
var TChannelInfo = db.sequelize.define('channel_info', {
    channel: {
        type: Sequelize.STRING,
        allowNull: false,
        primaryKey: true,
    },
    version: {
        type: Sequelize.STRING,
        allowNull: false,
        primaryKey: true,
    },
    value: {
        type: Sequelize.STRING(2048),
        allowNull: false
    }
});

//---------------------------------
exports.initChannelInfo = function (callBack) {
    TChannelInfo.sync({
        alter: true
    }).then(() => {
        log.logInfo("initChannelInfo done! ");
        if (callBack) {
            callBack();
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}

exports.insertChannelInfo = function (channelInfo, callBack) {
    return TChannelInfo.findOrCreate({
        where: {
            channel: channelInfo.channel,
            version: channelInfo.version
        },
        defaults: {
            channel: channelInfo.channel,
            version: channelInfo.version,
            value: JSON.stringify(channelInfo)
        }

    }).then(([element, created]) => {
        console.log(element.get({
            plain: true
        }))
        console.log(created)
        if (callBack) {
            callBack();
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}


exports.updateChannelInfo = function (channelInfo, callBack) {
    return TChannelInfo.update({
        value: JSON.stringify(channelInfo),
    }, {
        where: {
            channel: channelInfo.channel,
            version: channelInfo.version
        }
    }).then(([element]) => {
        log.logInfo("updateChannelInfo done! " + JSON.stringify(channelInfo));
        if (callBack) {
            callBack(element);
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}


exports.getChannelInfo = function (key, callBack) {

    return TChannelInfo.findAll({
        where: {
            channel: key.channel,
            version: key.version,
        }
    }).then((result) => {
        log.logInfo("getChannelInfo done! " + JSON.stringify(result));
        if (callBack) {
            callBack(result);
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}