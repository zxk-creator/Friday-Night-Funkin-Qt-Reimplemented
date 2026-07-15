//
// Created by kkplay on 7/9/26.
//

#pragma once
#include <QString>
#include "../type/Dynamic.h"
#include "../type/HClass.h"

class TestClass : public HClass
{
    int a = 1;
    float b = 2.0f;

public:
    TestClass() : HClass("TestClass",nullptr)
    {
        registerNativeMethod("c", FunctionType([this](const std::vector<Dynamic>& args) -> Dynamic
        {

        }));
    }

    int c(int val)
    {

        return a + val;
    }

    Dynamic getField(const QString& fieldName) override
    {
        if (fieldName == "a") return a;
        if (fieldName == "b") return b;

        return HClass::getField(fieldName);
    }

    void setField(const QString& fieldName, Dynamic value) override
    {
        if (fieldName == "a") { a = value.asNumber(); return; }
        if (fieldName == "b") { b = value.asNumber(); return; }

        HClass::setField(fieldName, value);
    }

    bool hasField(const QString& fieldName) override
    {
        if (fieldName == "a" || fieldName == "b") return true;

        return false;
    }
};
