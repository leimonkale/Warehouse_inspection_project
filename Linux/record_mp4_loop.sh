#!/bin/bash

RTSP_URL="rtsp://192.168.18.97:554/mjpeg/1"  
SAVE_DIR="./rtsp_videos"                     
DURATION=30                                   
VCODEC="h264"                                 
ACODEC="aac"                                 

mkdir -p "$SAVE_DIR"

echo "开始用VLC循环录制MP4（每次${DURATION}秒），按Ctrl+C停止"

while true; do
   
    TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
    OUTPUT_FILE="${SAVE_DIR}/recording_${TIMESTAMP}.mp4"

    
    cvlc -q \
        "$RTSP_URL" \
        --sout "#transcode{vcodec=${VCODEC},acodec=${ACODEC}}:std{access=file,mux=mp4,dst=$OUTPUT_FILE}" \
        --run-time=$DURATION \
        --play-and-exit  

    if [ -f "$OUTPUT_FILE" ] && [ $(stat -c%s "$OUTPUT_FILE") -gt 1024 ]; then
        echo "✅ 录制成功: $OUTPUT_FILE"
    else
        echo "❌ 录制失败: $OUTPUT_FILE"
        rm -f "$OUTPUT_FILE"  
    fi

    sleep 1
done