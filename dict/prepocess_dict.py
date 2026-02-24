"""
将结巴原本词典进行处理，避免 "学号" 被分为两个词
去除掉无用的单个字与全英文单词
"""

def filter_single_char_first_word(input_file, output_file):
    """
    过滤文件行：去除“首词Unicode字符长度为1”的行（无论首词是汉字、英文、符号等）
    :param input_file: 输入文件路径（如 dict.txt）
    :param output_file: 输出文件路径（过滤后的结果文件）
    """
    # 读取输入文件并执行过滤
    with open(input_file, 'r', encoding='utf-8') as f_in, \
         open(output_file, 'w', encoding='utf-8') as f_out:
        
        for line in f_in:
            # 去除行首尾空白（避免换行符、空格干扰，同时跳过空行）
            cleaned_line = line.strip()
            if not cleaned_line:
                continue
            
            # 提取首词：按空格分割，取第一个非空元素（处理多空格情况）
            first_word = cleaned_line.split()[0]
            
            # 核心过滤条件：首词的Unicode字符长度 != 1 → 保留该行
            # （Unicode长度1包含：单个汉字、单个英文、单个符号等，均会被过滤）
            if len(first_word) != 1:
                f_out.write(cleaned_line + '\n')

# 主程序入口
if __name__ == "__main__":
    # 配置文件路径（可根据实际位置修改）
    INPUT_FILE_PATH = "/home/whs/code/tokenizer/cjieba/dict/dict.txt"    # 原始输入文件
    OUTPUT_FILE_PATH = "/home/whs/code/tokenizer/cjieba/dict/jieba.dict.utf8"  # 过滤后输出文件
    
    # 执行过滤操作
    filter_single_char_first_word(INPUT_FILE_PATH, OUTPUT_FILE_PATH)
    print(f"过滤完成！结果已保存至：{OUTPUT_FILE_PATH}")
