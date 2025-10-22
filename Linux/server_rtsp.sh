#!/bin/bash

RTSP_URL="rtsp://192.168.18.97:554/mjpeg/1"  
SAVE_DIR="./rtsp_videos"
DURATION=300  

mkdir -p $SAVE_DIR

echo "尝试拉取RTSP流并直接保存..."
echo "按Ctrl+C停止"

while true; do
    TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
    OUTPUT_FILE="${SAVE_DIR}/esp32_${TIMESTAMP}.mjpeg"  # 保存为原始MJPEG格式

    cvlc -q \
        "$RTSP_URL" \
        --sout "#std{access=file,mux=raw,dst=$OUTPUT_FILE}" \
        --run-time=$DURATION \
        vlc://quit

    if [ -f "$OUTPUT_FILE" ] && [ $(stat -c%s "$OUTPUT_FILE") -gt 1024 ]; then
        echo "保存成功: $OUTPUT_FILE（可直接用VLC播放）"
    else
        echo "保存失败: $OUTPUT_FILE"
        rm -f "$OUTPUT_FILE"
    fi

    sleep 1
done