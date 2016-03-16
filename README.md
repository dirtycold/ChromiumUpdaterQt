ChromiumUpdaterQt [![Build Status](https://travis-ci.org/dirtycold/ChromiumUpdaterQt.svg)](https://travis-ci.org/dirtycold/ChromiumUpdaterQt)
=================

ChromiumUpdaterQt - A Chromium Updater written in Qt.

Screenshot
---
![ChromiumUpdaterQt](http://wstaw.org/m/2015/07/26/ChromiumUpdaterQt.png)

Explanation for settings 
---
* `Version` Lastest successfully installed version number.
* `BaseUrl` Base url for a Chromium build repository. 
* `Platform` The operating system and bitness for Chromium build. Currently `Win32` and `Win64` are supported.
* `Protocol` The way this updater connect to a Chromium build repository. Currently `HTTP` and `HTTPS` are supported;
* `AutoCheck` Checks for lastest version automatically. Also hides `Check Version` button.
* `AutoDownload` Download lastest Chromium build when new version found. Also hides `Download and Install` button.
* `AutoRemove` Remove downloaded Chromium installer after installation.
* `UseSystemProxy` Checks system proxy for download urls. This argument somewhat conflicts with `UseManualProxy`, they cannot be `true` at the same time otherwise direct connection will be used eventually.
* `UseManualProxy` Use specified manual proxy settings for download urls. This argument somewhat conflicts with `UseSystemProxy`, they cannot be `true` at the same time otherwise direct connection will be used eventually.
* `ManualProxyType` Specify manual proxy type. Currently `HTTP` and `SOCKS5` are supported. Effective only when `UseManualProxy` is `true`;
* `ManualProxyHost` Specify manual proxy host name. Effective only when `UseManualProxy` is `true`;
* `ManualProxyPort` Sepcify manual proxy host port. Effective only when `UseManualProxy` is `true`;
* `ManualProxyUsername` Sepcify manual proxy username if needed. Effective only when `UseManualProxy` is `true`;
* `ManualProxyPassword` Sepcify manual proxy password if needed. Effective only when `UseManualProxy` is `true`;
