/*jslint bitwise: true, node: true */
'use strict';

//引入内置模块http模块
var http = require('http');

var log = require('./logs');
var cfg = require('./config.json');
var db = require('./dbproxy');

const express = require('express');
const app = new express();
const bodyParser = require('body-parser');


//application/x-www-form-urlencoded
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
    extended: true
}));
app.use(express.static('public'));

app.post('/channelinfo', (req, response) => {

    log.logInfo(req.body);

    db.channelinfo_db.getChannelInfo({
        channel: req.body.channel,
        version: req.body.version
    }, function result(info) {
        if (info.length > 0) {
            response.write(info[0].value);
            response.end();
        }
    }).catch((e) => {
        log.logError(e.stack);
    });
});

//http://172.104.112.229:3001/serverinfo
app.post('/serverinfo', (req, response) => {
    log.logInfo(req.body);
    db.serverinfo_db.getServerList(
        function result(info) {
            response.write(JSON.stringify(info));
            response.end();
        }).catch((e) => {
        log.logError(e.stack);
    });
});

app.post('/noticeinfo', (req, response) => {
    log.logInfo(req.body);
    db.noticeinfo_db.getNoticeInfo({
            channel: req.body.channel,
            language: req.body.language
        },
        function result(info) {
            if (info.length > 0) {
                response.write(info[0].notice);
                response.end();
            }
        }).catch((e) => {
        log.logError(e.stack);
    });
});

var serverCfg = cfg.serverCfg[cfg.databaseCfg];
app.listen(serverCfg.port, () => {
    console.log('server started at ', serverCfg.port);
});

log.logInfo("start ok! port=", serverCfg.port);

db.dbinit(
    function () {
        log.logInfo("dbinit done!");
    });