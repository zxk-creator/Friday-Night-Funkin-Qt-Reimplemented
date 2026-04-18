//
// Created by kkplay on 4/18/26.
#pragma once


#ifdef Q_OS_ANDROID
// 这个移动端授权问题是真的麻烦，先让AI生成吧，回头我再研究

#include <jni.h>
#include <QtCore/qnativeinterface.h>

// 建议使用 Qt 提供的辅助类来获取 Activity，更安全
#include <QCoreApplication>
#include <QJniObject>
#include <QJniEnvironment>
typedef QJniObject NativeObject;


static JavaVM* g_vm = nullptr;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}

void requestAllFilesAccess() {
    // 通过 Qt 内部已有的静态方法直接获取当前 Activity
    // 注意：Qt 6 的包名路径是 org/qtproject/qt/android/QtNative
    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative",
        "activity",
        "()Landroid/app/Activity;"
        );

    if (activity.isValid()) {
        // 将获取到的 activity 作为参数传给你写的 PermissionHelper
        QJniObject::callStaticMethod<void>(
            "com/kunkun/fnfqe/PermissionHelper",
            "requestAllFilesAccess",
            "(Landroid/content/Context;)V", // Java 里的 Context 可以接受 Activity
            activity.object<jobject>()
            );
    }
}

bool hasAllFilesPermission() {
    JNIEnv* env;
    if (g_vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        g_vm->AttachCurrentThread(&env, nullptr);
    }

    // 1. API 30 以下默认认为有权限
    jclass versionClass = env->FindClass("android/os/Build$VERSION");
    jfieldID sdkIntField = env->GetStaticFieldID(versionClass, "SDK_INT", "I");
    jint sdkInt = env->GetStaticIntField(versionClass, sdkIntField);
    if (sdkInt < 30) return true;

    // 2. 检查 Environment.isExternalStorageManager()
    jclass envClass = env->FindClass("android/os/Environment");
    jmethodID isManagerMethod = env->GetStaticMethodID(envClass, "isExternalStorageManager", "()Z");
    return env->CallStaticBooleanMethod(envClass, isManagerMethod);
}

#endif