import cv2
import redis
import numpy as np

if __name__ == '__main__':
	r = redis.Redis(host='127.0.0.1', port=6379)

	while True:
		data = r.get('rsd455::rgb')
		img = np.frombuffer(data, dtype=np.uint8).reshape(480,640,3)

		cv2.imshow("Image", img)
		key = cv2.waitKey(1)