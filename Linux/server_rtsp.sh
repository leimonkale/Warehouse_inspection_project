#!/bin/bash

# 配置参数
RTSP_URL="rtsp://192.168.137.86:554/mjpeg/1"  # 你的ESP32 RTSP地址
SAVE_DIR="./rtsp_videos"
DURATION=300  # 录制时长（秒）

mkdir -p $SAVE_DIR

echo "尝试拉取RTSP流并直接保存..."
echo "按Ctrl+C停止"

while true; do
    TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
    OUTPUT_FILE="${SAVE_DIR}/esp32_${TIMESTAMP}.mjpeg"  # 保存为原始MJPEG格式

    # 直接保存原始流（不转码，减少兼容性问题）
    cvlc -q \
        "$RTSP_URL" \
        --sout "#std{access=file,mux=raw,dst=$OUTPUT_FILE}" \
        --run-time=$DURATION \
        vlc://quit

    # 检查文件是否生成
    if [ -f "$OUTPUT_FILE" ] && [ $(stat -c%s "$OUTPUT_FILE") -gt 1024 ]; then
        echo "保存成功: $OUTPUT_FILE（可直接用VLC播放）"
    else
        echo "保存失败: $OUTPUT_FILE"
        rm -f "$OUTPUT_FILE"
    fi

    sleep 1
done