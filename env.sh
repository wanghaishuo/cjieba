#!/bin/bash
# env.sh - 设置 CJIEBA_PATH 环境变量（增强版）

# === 通用检测部分（POSIX 兼容）===
# 检测是否被 source 执行
(return 0 2>/dev/null) && SOURCED=1 || SOURCED=0
if [ "$SOURCED" -eq 0 ]; then
    echo "错误：此脚本必须通过 source 执行才能影响当前终端。" >&2
    echo "正确用法：source $0 或 . $0" >&2
    exit 1
fi

# 检测当前 shell 是否为 bash（后续需要使用 BASH_SOURCE 等特性）
if [ -z "$BASH_VERSION" ]; then
    echo "错误：此脚本需要在 bash 中 source 执行。" >&2
    echo "请先启动 bash：bash" >&2
    echo "然后再 source 此脚本。" >&2
    # source 模式下使用 return 退出脚本，但不退出整个 shell
    return 1 2>/dev/null
fi

# === 以下为 bash 特有代码 ===
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

show_help() {
    echo "用法：source $0 [选项]"
    echo "选项："
    echo "  --install   将 CJIEBA_PATH 永久写入 ~/.bashrc，并在当前 Shell 立即生效"
    echo "  无参数      仅临时设置当前 Shell 的 CJIEBA_PATH"
    echo ""
    echo "当前脚本目录：$SCRIPT_DIR"
}

# 处理 --help 参数
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    show_help
    return 0
fi

# 永久设置（--install）
if [[ "$1" == "--install" ]]; then
    # 更新 ~/.bashrc
    if grep -q "^export CJIEBA_PATH=" "$HOME/.bashrc" 2>/dev/null; then
        CURRENT_LINE=$(grep "^export CJIEBA_PATH=" "$HOME/.bashrc" | head -1)
        if [[ "$CURRENT_LINE" != "export CJIEBA_PATH=\"$SCRIPT_DIR\"" ]]; then
            cp "$HOME/.bashrc" "$HOME/.bashrc.backup.$(date +%Y%m%d%H%M%S)" 2>/dev/null
            sed -i.bak "s|^export CJIEBA_PATH=.*|export CJIEBA_PATH=\"$SCRIPT_DIR\"|" "$HOME/.bashrc"
            rm -f "$HOME/.bashrc.bak" 2>/dev/null
            echo "已更新 ~/.bashrc 中的 CJIEBA_PATH 为：$SCRIPT_DIR"
        else
            echo "~/.bashrc 中的 CJIEBA_PATH 已经是正确的值。"
        fi
    else
        echo "export CJIEBA_PATH=\"$SCRIPT_DIR\"" >> "$HOME/.bashrc"
        echo "已添加 CJIEBA_PATH 到 ~/.bashrc"
    fi

    export CJIEBA_PATH="$SCRIPT_DIR"
    echo "当前终端已设置 CJIEBA_PATH=$CJIEBA_PATH"
    echo "永久设置已生效，后续新终端将自动获得此变量。"
    return 0
fi

# 无参数：仅临时设置当前 Shell
if [[ -z "$CJIEBA_PATH" ]]; then
    export CJIEBA_PATH="$SCRIPT_DIR"
    echo "已临时设置 CJIEBA_PATH=$CJIEBA_PATH"
elif [[ "$CJIEBA_PATH" != "$SCRIPT_DIR" ]]; then
    echo "警告：当前 CJIEBA_PATH 指向 '$CJIEBA_PATH'，与脚本目录 '$SCRIPT_DIR' 不同。"
    echo "已临时覆盖为 '$SCRIPT_DIR'（仅本次会话）。"
    export CJIEBA_PATH="$SCRIPT_DIR"
else
    echo "CJIEBA_PATH 已经正确设置为 '$SCRIPT_DIR'"
fi

echo "提示：如需永久保存此设置，请运行：source ${BASH_SOURCE[0]} --install"
