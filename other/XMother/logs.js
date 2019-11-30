/*jslint bitwise: true, node: true */
'use strict';
//npm install --save log4js
var log4js = require('log4js');
var request = require('request');
var cfg = require('./config.json');

function getNowFormatDate() {
    var date = new Date();
    var seperator1 = "_";
    var seperator2 = "_";
    var month = date.getMonth() + 1;
    var strDay = date.getDate();
    if (month >= 1 && month <= 9) {
        month = "0" + month;
    }
    if (strDay >= 0 && strDay <= 9) {
        strDay = "0" + strDay;
    }
    var currentdate = date.getFullYear() + seperator1 + month + seperator1 + strDay +
        "-" + date.getHours() + seperator2 + date.getMinutes() +
        seperator2 + date.getSeconds();
    return currentdate;
}


var times = getNowFormatDate();

log4js.configure({
    appenders: {
        out: {
            type: 'stdout'
        }, //设置是否在控制台打印日志
        info: {
            type: 'file',
            filename: './log/' + times + '_info.log'
        },
        just_errors: {
            type: 'file',
            filename: './log/' + times + '_error.log'
        },
        'error': {
            type: 'logLevelFilter',
            appender: 'just_errors',
            level: 'error'
        }
    },
    categories: {
        default: {
            appenders: ['out', 'info', 'error'],
            level: 'info'
        } //去掉'out'。控制台不打印日志
    },
    disableClustering: true
});

// var LogFile = log4js.getLogger();
// LogFile.info(new Date());

var logInfoFile = log4js.getLogger('info');
exports.logInfo = function (...log) {
    logInfoFile.info(log);
}

var logErrorFile = log4js.getLogger('error');
exports.logError = function (...log) {
    logErrorFile.error(log);
}
