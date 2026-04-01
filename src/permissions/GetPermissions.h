//
// Created by 44224 on 3/29/2026.
//
#pragma once

#ifdef Q_OS_ANDROID
// 我实在是无法让CMake找到jni这个库，于是采用了绝对路径的方法包含，你如果无法配置好的话，也可以直接采用绝对路径，定位到ndk里面的jni
#include "D:\ProgramFiles\ASDK\ndk\27.2.12479018\toolchains\llvm\prebuilt\windows-x86_64\sysroot\usr\include\jni.h"
#include "string"

using std::string;

static JavaVM* g_vm = nullptr;

// 当库被加载的时候，系统自动调用这个，我们可以通过这个获取上下文
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    // 保存全局虚拟机指针
    g_vm = vm;
    return JNI_VERSION_1_6;
}

// 之后在任何地方手动获取 env
JNIEnv* getJniEnv() {
    JNIEnv* env = nullptr;
    // 尝试获取当前线程的 env
    jint res = g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    if (res == JNI_EDETACHED) {
        // 如果当前线程没在 JVM 里就挂载它
        g_vm->AttachCurrentThread(&env, nullptr);
    }
    return env;
}

// 通过反射获取到当前应用的activity的上下文对象
jobject getGlobalContext(JNIEnv* env) {
    // 找到ActivityThread类
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);
    // 获取Context
    jmethodID getApplication = env->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");
    jobject context = env->CallObjectMethod(at, getApplication);
    return context;
}

void requestAllFilesAccess() {
    JNIEnv *env = getJniEnv();
    jobject activity = getGlobalContext(env);

    // 1. 检查权限 (保持不变)
    jclass envClass = env->FindClass("android/os/Environment");
    jmethodID isManagerMethod = env->GetStaticMethodID(envClass, "isExternalStorageManager", "()Z");
    jboolean hasPermission = env->CallStaticBooleanMethod(envClass, isManagerMethod);

    if (!hasPermission) {
        // 2. 构造 Intent
        jclass intentClass = env->FindClass("android/content/Intent");
        jmethodID intentConstructor = env->GetMethodID(intentClass, "<init>", "(Ljava/lang/String;)V");

        // 使用针对特定应用的 Action
        jstring actionStr = env->NewStringUTF("android.settings.MANAGE_APP_ALL_FILES_ACCESS_PERMISSION");
        jobject intentObj = env->NewObject(intentClass, intentConstructor, actionStr);

        // 3. 构造 Data Uri: "package:com.your.app.id"
        // 先获取包名
        jclass contextClass = env->FindClass("android/content/Context");
        jmethodID getPackageNameMethod = env->GetMethodID(contextClass, "getPackageName", "()Ljava/lang/String;");
        jstring packageName = (jstring)env->CallObjectMethod(activity, getPackageNameMethod);

        // 拼接成 package:xxx
        const char* rawPkg = env->GetStringUTFChars(packageName, nullptr);
        std::string uriString = "package:";
        uriString += rawPkg;
        env->ReleaseStringUTFChars(packageName, rawPkg);
        jstring jUriString = env->NewStringUTF(uriString.c_str());

        // 调用 Uri.parse()
        jclass uriClass = env->FindClass("android/net/Uri");
        jmethodID uriParseMethod = env->GetStaticMethodID(uriClass, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
        jobject uriObj = env->CallStaticObjectMethod(uriClass, uriParseMethod, jUriString);

        // 设置 Intent 的 Data
        jmethodID setDataMethod = env->GetMethodID(intentClass, "setData", "(Landroid/net/Uri;)Landroid/content/Intent;");
        env->CallObjectMethod(intentObj, setDataMethod, uriObj);

        // 4. 执行跳转
        jclass activityClass = env->GetObjectClass(activity);
        jmethodID startActivityMethod = env->GetMethodID(activityClass, "startActivity", "(Landroid/content/Intent;)V");
        env->CallVoidMethod(activity, startActivityMethod, intentObj);

        // 释放局部引用
        env->DeleteLocalRef(actionStr);
        env->DeleteLocalRef(packageName);
        env->DeleteLocalRef(jUriString);
        env->DeleteLocalRef(uriObj);
        env->DeleteLocalRef(intentObj);
    }
}

#endif