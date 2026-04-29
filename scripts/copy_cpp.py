#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
自动创建.cpp文件的脚本
功能：根据.h文件自动创建对应的.cpp文件，并在第一行包含该.h文件
用途：用于复制include目录结构到src源码目录结构
"""

import os
import sys
import re

def create_cpp_from_h(h_file_path, src_base_path, include_base_path):
    """
    根据.h文件创建对应的.cpp文件

    参数:
        h_file_path: .h文件的完整路径
        src_base_path: src目录的根路径
        include_base_path: include目录的根路径
    """
    # 检查文件是否存在
    if not os.path.exists(h_file_path):
        print(f"错误: 头文件 '{h_file_path}' 不存在")
        return False

    # 只处理.h文件
    if not h_file_path.endswith('.h'):
        return False

    # 计算相对路径（从include根目录开始的路径）
    relative_path = os.path.relpath(h_file_path, include_base_path)

    # 生成对应的.cpp文件路径（将.h替换为.cpp）
    cpp_relative_path = relative_path.replace('.h', '.cpp')
    cpp_full_path = os.path.join(src_base_path, cpp_relative_path)

    # 确保目标目录存在
    cpp_dir = os.path.dirname(cpp_full_path)
    try:
        os.makedirs(cpp_dir, exist_ok=True)
        print(f"✓ 创建目录: {cpp_dir}")
    except OSError as e:
        print(f"✗ 无法创建目录 {cpp_dir}: {e}")
        return False

    # 检查.cpp文件是否已存在
    if os.path.exists(cpp_full_path):
        response = input(f"文件 '{cpp_full_path}' 已存在，是否覆盖? (y/N): ")
        if response.lower() != 'y':
            print(f"跳过: {cpp_full_path}")
            return False

    # 生成#include语句中的路径
    # 使用相对于include根目录的路径
    include_path = relative_path.replace('\\', '/')  # 统一使用正斜杠

    # 准备文件内容
    content = f'#include "{include_path}"\n\n'

    # 可选：添加基本的函数定义框架
    # 这里可以进一步解析.h文件，提取函数声明并生成空实现
    # 简单版本只添加包含语句

    # 写入.cpp文件
    try:
        with open(cpp_full_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"✓ 创建文件: {cpp_full_path}")
        return True
    except IOError as e:
        print(f"✗ 无法写入文件 {cpp_full_path}: {e}")
        return False

def generate_cpp_files_from_include(include_path, src_path, recursive=True):
    """
    从include目录遍历所有.h文件，在src目录下创建对应的.cpp文件

    参数:
        include_path: include目录的路径
        src_path: src目录的路径
        recursive: 是否递归处理子目录
    """
    if not os.path.exists(include_path):
        print(f"错误: include路径 '{include_path}' 不存在")
        return

    if not os.path.exists(src_path):
        print(f"警告: src路径 '{src_path}' 不存在，将自动创建")
        os.makedirs(src_path, exist_ok=True)

    print(f"开始处理: {include_path} -> {src_path}")
    print("-" * 60)

    success_count = 0
    fail_count = 0

    # 遍历include目录
    if recursive:
        for root, dirs, files in os.walk(include_path):
            for file in files:
                if file.endswith('.h'):
                    h_file_path = os.path.join(root, file)
                    if create_cpp_from_h(h_file_path, src_path, include_path):
                        success_count += 1
                    else:
                        fail_count += 1
    else:
        # 只处理根目录
        for file in os.listdir(include_path):
            if file.endswith('.h'):
                h_file_path = os.path.join(include_path, file)
                if create_cpp_from_h(h_file_path, src_path, include_path):
                    success_count += 1
                else:
                    fail_count += 1

    print("-" * 60)
    print(f"完成! 成功: {success_count}, 失败: {fail_count}")

def generate_single_cpp(h_file, src_path, include_path=None):
    """
    为单个.h文件生成对应的.cpp文件

    参数:
        h_file: .h文件的路径
        src_path: src目录的路径
        include_path: include根目录路径（可选，如果不提供则自动推断）
    """
    if not os.path.exists(h_file):
        print(f"错误: 文件 '{h_file}' 不存在")
        return

    if not h_file.endswith('.h'):
        print(f"错误: '{h_file}' 不是.h文件")
        return

    # 如果没有指定include_path，尝试从h_file路径推断
    if include_path is None:
        # 查找包含 'include' 的父目录
        path_parts = os.path.normpath(h_file).split(os.sep)
        include_pos = -1
        for i, part in enumerate(path_parts):
            if part.lower() == 'include':
                include_pos = i
                break

        if include_pos != -1:
            include_path = os.sep.join(path_parts[:include_pos+1])
        else:
            include_path = os.path.dirname(h_file)

    create_cpp_from_h(h_file, src_path, include_path)

def main():
    """主函数：处理命令行参数"""
    if len(sys.argv) < 3:
        print(__doc__)
        print("\n使用方法:")
        print("  批量模式: python script.py <include_path> <src_path>")
        print("  单文件模式: python script.py -f <header_file> <src_path> [include_path]")
        print("\n示例:")
        print("  python script.py ./include ./src")
        print("  python script.py -f ./include/myclass.h ./src")
        print("  python script.py -f ./include/dir1/myclass.h ./src ./include")
        print("\n说明:")
        print("  批量模式: 递归遍历include目录下所有.h文件，在src目录下创建对应的.cpp文件")
        print("  单文件模式: 为指定的.h文件创建对应的.cpp文件")
        return

    # 单文件模式
    if sys.argv[1] == '-f' or sys.argv[1] == '--file':
        if len(sys.argv) < 4:
            print("错误: 单文件模式需要指定头文件和src路径")
            return

        h_file = sys.argv[2]
        src_path = sys.argv[3]
        include_path = sys.argv[4] if len(sys.argv) > 4 else None

        generate_single_cpp(h_file, src_path, include_path)
    else:
        # 批量模式
        include_path = sys.argv[1]
        src_path = sys.argv[2]
        recursive = True  # 默认递归

        # 检查是否有--no-recursive参数
        if len(sys.argv) > 3 and sys.argv[3] == '--no-recursive':
            recursive = False

        generate_cpp_files_from_include(include_path, src_path, recursive)

if __name__ == "__main__":
    main()