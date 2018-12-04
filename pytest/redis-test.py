import redis
import time

REDIS_HOST="10.102.34.200"
REDIS_PORT="6379"

LIST="admin_msg_list"

r = redis.Redis(REDIS_HOST, REDIS_PORT)
f = open('/var/log/admin_msg.txt', 'a+')

while 1:
	print "checking redis for new msgs..."
	cnt = r.llen(LIST)
	print 'found %d new msgs' % cnt
	i = 0
	while i < cnt:
		val = r.rpop(LIST)
		print val
		f.write(val)
		f.write("\n")
		i += 1
	f.flush()
	time.sleep(5);
