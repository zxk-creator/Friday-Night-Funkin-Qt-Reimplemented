//
// Created by kkplay on 4/22/26.
//

#pragma once
#include <ostream>

#include "string"
#include "semver/semver.h"
#include <QString>
#include <QDebug>

using std::string;

class VersionUtil
{
public:
      static bool validateVersionStr(const string& versionStr,const string& ruleStr)
      {
            try {
                  semver::version<> version;
                  auto parseResult = semver::parse(versionStr, version);
                  if (!parseResult) {
                        qWarning() << "[VERSIONUTIL] 解析版本失败: " << QString::fromStdString(ruleStr);
                        return false;
                  }

                  semver::range_set<> rule;
                  parseResult = semver::parse(ruleStr, rule);
                  if (!parseResult) {
                        qWarning() << "[VERSIONUTIL] 解析规则失败"
                                  << QString::fromStdString(ruleStr);
                        return false;
                  }

                  return rule.contains(version);
            } catch (const std::exception& e) {
                  qWarning() << "[VERSIONUTIL] 无效的semantic版本: "
                            << QString::fromStdString(versionStr);
                  return false;
            }
      }
      static bool validateVersionStr(const QString& versionStr,const QString& ruleStr)
      {
            try {
                  semver::version<> version;
                  auto parseResult = semver::parse(versionStr.toStdString(), version);
                  if (!parseResult) {
                        qWarning() << "[VERSIONUTIL] 解析版本失败: " << ruleStr;
                        return false;
                  }

                  semver::range_set<> rule;
                  parseResult = semver::parse(ruleStr.toStdString(), rule);
                  if (!parseResult) {
                        qWarning() << "[VERSIONUTIL] 解析规则失败"
                                  << ruleStr;
                        return false;
                  }

                  return rule.contains(version);
            } catch (const std::exception& e) {
                  qWarning() << "[VERSIONUTIL] 无效的semantic版本: "
                            << versionStr;
                  return false;
            }
      }

      static bool isValidSemVer(const std::string& vStr) {
            return semver::valid(vStr);
      }
};