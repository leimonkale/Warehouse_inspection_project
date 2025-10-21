#!/bin/bash

# 配置参数
RTSP_URL="rtsp://192.168.137.86:554/mjpeg/1"  # RTSP流地址
SAVE_DIR="./rtsp_videos"                  # 保存目录
DURATION=30                                   # 每次录制时长(秒)
TIMEOUT=5                                     # 超时时间(秒)

# 创建保存目录
mkdir -p "$SAVE_DIR"

echo "开始循环录制MP4视频（每次${DURATION}秒），按Ctrl+C停止"

# 循环录制
while true; do
    # 生成带时间戳的文件名
    TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
    OUTPUT_FILE="${SAVE_DIR}/recording_${TIMESTAMP}.mp4"

    # 使用ffmpeg录制RTSP流并保存为MP4（自动处理编码）
    ffmpeg -rtsp_transport tcp -i "$RTSP_URL" \
           -t "$DURATION" \
           -timeout "${TIMEOUT}000000" \
           -c:v libx264 -preset ultrafast -crf 28 \
           -c:a aac -b:a 128k \
           "$OUTPUT_FILE" >/dev/null 2>&1

    # 检查录制结果
    if [ -f "$OUTPUT_FILE" ] && [ $(stat -c%s "$OUTPUT_FILE") -gt 1024 ]; then
        echo "录制成功: $OUTPUT_FILE"
    else
        echo "录制失败: $OUTPUT_FILE"
        rm -f "$OUTPUT_FILE"  # 清理空文件
    fi

    # 短暂延迟避免文件名重复（可选）
    sleep 1
done