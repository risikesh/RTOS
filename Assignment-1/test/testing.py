import subprocess
import os
import signal
import time
print os.getcwd()
server= subprocess.Popen('./build/server',stdin=subprocess.PIPE,stdout=subprocess.PIPE)
time.sleep(0.1)
client1=subprocess.Popen(['./build/client', '127.0.0.1', '8080', 'rkr1', 'test' ,'DEV'],stdin=subprocess.PIPE,stdout=subprocess.PIPE)
time.sleep(0.1)
client2=subprocess.Popen(['./build/client', '127.0.0.1', '8080', 'rkr2', 'test' ,'DEV'],stdin=subprocess.PIPE,stdout=subprocess.PIPE)
time.sleep(0.1)

client1.stdin.write('HI\n')
time.sleep(0.5)
client2.stdin.write('HI\n')
time.sleep(0.5)
client1.stdin.close()
'''
time.sleep(0.1)
client2.stdin.write('HI\n')
time.sleep(0.1)
client2.stdin.write('HI\n')
client2.stdin.close()
'''
time.sleep(1)
os.kill(server.pid,signal.SIGINT)
os.kill(client1.pid,signal.SIGINT)
os.kill(client2.pid,signal.SIGINT)
print "done"