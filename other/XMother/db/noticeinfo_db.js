var Sequelize = require('sequelize');
var log = require('../logs');
var db = require('../dbproxy');

////////////////////////////////
var TNoticeInfo = db.sequelize.define('notice_info', {
    channel: {
        type: Sequelize.STRING,
        allowNull: false,
        primaryKey: true,
    },
    language: {
        type: Sequelize.STRING,
        allowNull: false,
        primaryKey: true,
    },
    notice: {
        type: Sequelize.TEXT
    }
});

//---------------------------------
exports.initNoticeInfo = function (callBack) {
    TNoticeInfo.sync({
        alter: true
    }).then(() => {
        log.logInfo("initNoticeInfo done! ");
        if (callBack) {
            callBack();
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}

exports.insertNoticeInfo = function (noticeInfo, callBack) {
    return TNoticeInfo.findOrCreate({
        where: {
            channel: noticeInfo.channel,
            language: noticeInfo.language
        },
        defaults: {
            channel: noticeInfo.channel,
            language: noticeInfo.language,
            notice: noticeInfo.notice,
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


exports.updateNoticeInfo = function (noticeInfo, callBack) {
    return TNoticeInfo.update({
        notice: noticeInfo.notice
    }, {
        where: {
            channel: noticeInfo.channel,
            language: noticeInfo.language
        }
    }).then(([element]) => {
        log.logInfo("updateNoticeInfo done! " + JSON.stringify(noticeInfo));
        if (callBack) {
            callBack(element);
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}


exports.getNoticeInfo = function (key, callBack) {

    return TNoticeInfo.findAll({
        where: {
            channel: key.channel,
            language: key.language
        }
    }).then((result) => {
        log.logInfo("getNoticeInfo done! " + JSON.stringify(result));
        if (callBack) {
            callBack(result);
        }
    }).catch((e) => {
        log.logInfo(e.stack);
    });
}