#!/bin/bash
set -e  # 任何命令失败则退出

# 获取脚本所在目录的绝对路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

show_help() {
    echo "Usage: $0 [OPTION]"
    echo "Options:"
    echo "  clean               删除 build 目录（清理所有构建文件）"
    echo "  -h, --help          显示此帮助信息"
    echo "  (无参数)            执行编译"
    echo ""
    echo "      - 如果未设置，则临时导出为 '$SCRIPT_DIR' 并永久写入 ~/.bashrc"
    echo "      - 如果已设置但指向其他路径，则临时覆盖为 '$SCRIPT_DIR'（仅本次会话）"
    echo "      - 如果已正确设置，则直接使用"
}

# 帮助选项
if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    show_help
    exit 0
fi

# 清理功能
if [ "$1" = "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build
    echo "Clean done."
    exit 0
fi

# 如果传了其他未知参数，报错
if [ -n "$1" ]; then
    echo "Unknown option: $1"
    show_help
    exit 1
fi

# ---------- 编译流程 ----------
echo "开始编译，当前目录：$(pwd)"
echo "CJIEBA_PATH 环境变量：$CJIEBA_PATH"
mkdir -p build          # 不存在则创建 build 目录
cd build
cmake ..                # 生成构建文件
make                    # 编译
echo "编译完成。生成的可执行文件位于 build/ 目录下（如有）。"