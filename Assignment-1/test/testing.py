import os
import csv

os.system("mkdir -p log_temp")
os.system("rm log/*")

print "|parallel_users|number_of_users|average_time|minimum_time|maximum_time|"
print "|---|---|---|---|---|"
for parallel_users in range(1,6):
    for number_of_users in range(10,110,10):
        command = "./build/test " + str(number_of_users) +" "+ str(1)+" "+str(parallel_users)+" 500 final_"+str(number_of_users)+"_"+str(parallel_users)+".csv > log/command_log_"+str(number_of_users)+"_"+str(parallel_users)+".txt"
        os.system(command)
        #print command
        csvfile= open('log/'+"final_"+str(number_of_users)+"_"+str(parallel_users)+".csv")
        spamreader = csv.reader(csvfile, delimiter=',')
        total_time=[]
        for row in spamreader:
            total_time.append(float(row[1]))
        print "|",parallel_users,"|",number_of_users,"|",sum(total_time)/len(total_time),"|",min(total_time),"|",max(total_time),"|"

os.system("rm -rf log_temp")
print "Do you want to retain the main log file (y/n) ?"
if(raw_input()=='n'):
    os.system('rm log/*')
