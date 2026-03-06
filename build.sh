#!/bin/bash
set -e  # 任何命令失败则退出

# 获取脚本所在目录的绝对路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

show_help() {
    echo "Usage: $0 [OPTION]"
    echo "Options:"
    echo "  clean               删除 build 目录（清理所有构建文件）"
    echo "  -h, --help          显示此帮助信息"
    echo "  (无参数)            执行编译，同时确保 CJIEBA_PATH 正确设置"
    echo ""
    echo "说明：每次编译时会自动检查 CJIEBA_PATH 环境变量。"
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

# ---------- 环境变量 CJIEBA_PATH 检查与设置 ----------
if [ -z "$CJIEBA_PATH" ]; then
    # 情况1：当前环境未设置 CJIEBA_PATH
    echo "CJIEBA_PATH 未设置，临时设为 '$SCRIPT_DIR' 并永久添加到 ~/.bashrc"
    export CJIEBA_PATH="$SCRIPT_DIR"
    # 检查 ~/.bashrc 是否已存在相关行，避免重复添加
    if ! grep -q "^export CJIEBA_PATH=" "$HOME/.bashrc" 2>/dev/null; then
        {
            echo ""
            echo "# Added by build script on $(date)"
            echo "export CJIEBA_PATH=\"$SCRIPT_DIR\""
        } >> "$HOME/.bashrc"
        echo "已永久添加 CJIEBA_PATH 到 ~/.bashrc，下次终端启动自动生效。"
    else
        echo "警告：~/.bashrc 中已定义 CJIEBA_PATH 但当前环境未生效，请执行 'source ~/.bashrc' 或重启终端。"
    fi
else
    # 情况2：已设置，检查值是否正确
    if [ "$CJIEBA_PATH" != "$SCRIPT_DIR" ]; then
        echo "警告：CJIEBA_PATH 当前为 '$CJIEBA_PATH'，但脚本期望值为 '$SCRIPT_DIR'"
        echo "本次会话临时覆盖为 '$SCRIPT_DIR'（不会修改 ~/.bashrc）"
        export CJIEBA_PATH="$SCRIPT_DIR"
    else
        echo "CJIEBA_PATH 已正确设置为 '$SCRIPT_DIR'"
    fi
fi

# ---------- 编译流程 ----------
echo "开始编译，当前目录：$(pwd)"
echo "CJIEBA_PATH 环境变量：$CJIEBA_PATH"
mkdir -p build          # 不存在则创建 build 目录
cd build
cmake ..                # 生成构建文件
make                    # 编译
echo "编译完成。生成的可执行文件位于 build/ 目录下（如有）。"