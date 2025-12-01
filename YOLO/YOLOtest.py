from ultralytics import YOLO
import cv2
from PIL import Image
import requests
from io import BytesIO
model = YOLO('yolov8n.pt')

def detect_image(image_source, model, conf_threshold=0.25):
    """
    对图片进行目标检测
    
    参数:
        image_source: 图片路径或URL
        model: YOLO模型
        conf_threshold: 置信度阈值，低于此值的检测结果会被过滤
    """
    # 进行检测
    # conf: 置信度阈值
    # verbose: 是否打印详细信息
    results = model(image_source, conf=conf_threshold, verbose=False)
    
    # 获取第一张图片的结果
    result = results[0]
    
    # 打印检测结果
    print('\\n========== 检测结果 ==========')
    boxes = result.boxes
    if len(boxes) == 0:
        print('未检测到任何物体')
    else:
        for i, box in enumerate(boxes):
            # 获取类别名称
            cls_id = int(box.cls[0])
            cls_name = model.names[cls_id]
            
            # 获取置信度
            confidence = float(box.conf[0])
            
            # 获取边界框坐标 (x1, y1, x2, y2)
            x1, y1, x2, y2 = box.xyxy[0].tolist()
            
            print(f'{i+1}. {cls_name}: {confidence:.2%} | 位置: ({x1:.0f}, {y1:.0f}) - ({x2:.0f}, {y2:.0f})')
    
    return result


def detect_video(source=0, model=None, show_fps=True):
    """
    对视频或摄像头进行实时目标检测
    
    参数:
        source: 视频源
                - 0: 默认摄像头
                - 1, 2...: 其他摄像头
                - 'video.mp4': 视频文件路径
        model: YOLO模型
        show_fps: 是否显示帧率
    
    操作:
        - 按 'q' 键退出
        - 按 's' 键保存当前帧
    """
    # 打开视频源
    cap = cv2.VideoCapture(source)
    
    if not cap.isOpened():
        print(f'无法打开视频源: {source}')
        return
    
    print('开始实时检测...')
    print('按 q 退出 | 按 s 保存当前帧')
    
    frame_count = 0
    
    while True:
        # 读取一帧
        ret, frame = cap.read()
        if not ret:
            print('视频结束或无法读取帧')
            break
        
        # 记录开始时间（用于计算FPS）
        start_time = cv2.getTickCount()
        
        # 进行检测
        results = model(frame, verbose=False)
        
        # 获取带标注的帧
        annotated_frame = results[0].plot()
        
        # 计算并显示FPS
        if show_fps:
            end_time = cv2.getTickCount()
            fps = cv2.getTickFrequency() / (end_time - start_time)
            cv2.putText(annotated_frame, f'FPS: {fps:.1f}', (10, 30),
                       cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        
        # 显示结果
        cv2.imshow('YOLO Detection', annotated_frame)
        
        # 键盘控制
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):  # 按q退出
            break
        elif key == ord('s'):  # 按s保存
            cv2.imwrite(f'detection_{frame_count}.jpg', annotated_frame)
            print(f'已保存: detection_{frame_count}.jpg')
        
        frame_count += 1
    
    # 释放资源
    cap.release()
    cv2.destroyAllWindows()
    print(f'检测结束，共处理 {frame_count} 帧')

# 注意: 在Jupyter中运行以下代码可能会有问题
# 建议在本地Python环境中运行
# 
# 使用摄像头:
detect_video(source=0, model=model)
#
# 使用视频文件:
# detect_video(source='your_video.mp4', model=model)

print('视频检测函数已定义，可在本地环境中调用')