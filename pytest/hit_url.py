import sys
import urllib.request

url = sys.argv[1]
count = int(sys.argv[2])
request = urllib.request.Request(url)

for i in range(0, count):
	response = urllib.request.urlopen(request)
	print(i, response.info())
	print(i, response.read().decode('utf-8'))


