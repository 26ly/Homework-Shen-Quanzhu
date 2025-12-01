import cv2
import numpy as np

object_points = np.array([
    [0, 0, 0],
    [1, 0, 0],
    [1, 1, 0],
    [0, 1, 0]
], dtype=np.float32)

image_points = np.array([
    [320, 240],
    [400, 240],
    [400, 320],
    [320, 320]
], dtype=np.float32)

camera_matrix = np.array([
    [800, 0, 320],
    [0, 800, 240],
    [0, 0, 1]
], dtype=np.float32)

dist_coeffs = np.zeros((4, 1))

_, rvec, tvec = cv2.solvePnP(object_points, image_points, camera_matrix, dist_coeffs)

print("旋转向量:\n", rvec)
print("平移向量:\n", tvec)

r_matrix, _ = cv2.Rodrigues(rvec)
print("旋转矩阵:\n", r_matrix)
rt_matrix = np.hstack((r_matrix, tvec))
matrix_44 = np.vstack((rt_matrix, [0, 0, 0, 1]))
print("4x4变换矩阵:\n", matrix_44)