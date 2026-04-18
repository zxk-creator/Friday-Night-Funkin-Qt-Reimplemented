# 用于复制include目录结构到src源码目录结构

import os
import sys

def replicate_dir_structure(src_path, dst_path):
    """
    将 src_path 的目录结构复制到 dst_path，不触及任何文件。
    """
    # 确保源路径存在
    if not os.path.exists(src_path):
        print(f"源路径 '{src_path}' 不存在!!!")
        return

    # 遍历源目录
    for root, dirs, files in os.walk(src_path):
        relative_path = os.path.relpath(root, src_path)

        target_dir = os.path.join(dst_path, relative_path)

        try:
            # exist_ok=True 会在目录已存在时跳过，不会报错或替换原有内容
            os.makedirs(target_dir, exist_ok=True)
            print(f"已同步目录:{target_dir}")
        except OSError as e:
            print(f"无法创建目录{target_dir}: {e}")

if __name__ == "__main__":
    # 通过命令行参数传入：python3 script.py /path/to/src /path/to/dst
    if len(sys.argv) == 3:
        source = sys.argv[1]
        destination = sys.argv[2]
        replicate_dir_structure(source, destination)
    else:
        print("使用方法: python3 script.py '源路径' '目标路径'")