# Assignment - 1

One on One chat application can be found on this folder [one on one chat](../test_files/socketchat1on1) .

## Instructions for General Usage

1. run ``make`` from the folder.
2. for to run server ``./build/server`` , the server starts working from port:**8080** .
3. open few more terminals to run the clients ``./build/client 127.0.0.1 8080 <User Name> <Group Name> PROD `` .
4.  change the username and group name accordingly, and create more clients.

---

## Performance Metrics

This metrics is generated using scripts in ``test`` folder.

To generate the below metrics:

1. first ``make`` from the repositories directory.
2. after that ``python test/testing.py`` .

Time represents the time taken from sender to receiver.

| parallel_users | number_of_users | Average_time($\mu s$) | Minimum_time($\mu s$) | Maximum_time($\mu s$) |
| -------------- | --------------- | --------------------- | --------------------- | --------------------- |
| 1              | 10              | 1043.49769737         | 293.0                 | 2345.0                |
| 1              | 20              | 645.876315789         | 340.0                 | 1078.0                |
| 1              | 30              | 750.191954023         | 293.0                 | 1344.0                |
| 1              | 40              | 790.23525641          | 243.0                 | 1766.0                |
| 1              | 50              | 838.826530612         | 193.0                 | 1977.0                |
| 1              | 60              | 937.250564972         | 185.0                 | 2364.0                |
| 1              | 70              | 1061.26149068         | 156.0                 | 2580.0                |
| 1              | 80              | 1134.15522152         | 186.0                 | 2924.0                |
| 1              | 90              | 1201.92709114         | 183.0                 | 3257.0                |
| 1              | 100             | 1359.13717172         | 180.0                 | 3465.0                |
| 2              | 10              | 563.005555556         | 339.0                 | 882.0                 |
| 2              | 20              | 579.573684211         | 185.0                 | 1174.0                |
| 2              | 30              | 673.132183908         | 132.0                 | 1677.0                |
| 2              | 40              | 797.784935897         | 137.0                 | 1821.0                |
| 2              | 50              | 891.258403361         | 111.0                 | 1869.0                |
| 2              | 60              | 1076.06257062         | 118.0                 | 3347.0                |
| 2              | 70              | 1182.48871636         | 138.0                 | 2998.0                |
| 2              | 80              | 1272.21938291         | 127.0                 | 4355.0                |
| 2              | 90              | 1221.27763682         | 163.0                 | 3280.0                |
| 2              | 100             | 1532.91888889         | 134.0                 | 5325.0                |
| 3              | 10              | 596.52962963          | 221.0                 | 997.0                 |
| 3              | 20              | 649.722284434         | 155.0                 | 1267.0                |
| 3              | 30              | 715.708505747         | 121.0                 | 1814.0                |
| 3              | 40              | 915.255555556         | 120.0                 | 2483.0                |
| 3              | 50              | 1069.82217687         | 133.0                 | 2666.0                |
| 3              | 60              | 1098.71815614         | 139.0                 | 3063.0                |
| 3              | 70              | 1482.97964113         | 138.0                 | 44109.0               |
| 3              | 80              | 1450.00139241         | 108.0                 | 4269.0                |
| 3              | 90              | 2122.91514773         | 121.0                 | 50696.0               |
| 3              | 100             | 2297.27872054         | 109.0                 | 50474.0               |
| 4              | 10              | 1080.00555556         | 155.0                 | 42441.0               |
| 4              | 20              | 2812.13765182         | 138.0                 | 46258.0               |
| 4              | 30              | 2322.14168658         | 140.0                 | 46154.0               |
| 4              | 40              | 2655.31278414         | 139.0                 | 47190.0               |
| 4              | 50              | 2227.75643829         | 120.0                 | 49358.0               |
| 4              | 60              | 3215.74795198         | 149.0                 | 48133.0               |
| 4              | 70              | 3424.71030021         | 137.0                 | 51042.0               |
| 4              | 80              | 3637.19572785         | 81.0                  | 50912.0               |
| 4              | 90              | 3157.59131038         | 108.0                 | 51073.0               |
| 4              | 100             | 2994.7326871          | 116.0                 | 51584.0               |
| 5              | 10              | 3555.73922902         | 162.0                 | 44273.0               |
| 5              | 20              | 3732.54290618         | 136.0                 | 43553.0               |
| 5              | 30              | 4285.76477833         | 139.0                 | 46192.0               |
| 5              | 40              | 2254.84580499         | 143.0                 | 49549.0               |
| 5              | 50              | 6867.33281633         | 114.0                 | 45134.0               |
| 5              | 60              | 5350.58551039         | 114.0                 | 48049.0               |
| 5              | 70              | 9060.85374741         | 126.0                 | 47657.0               |
| 5              | 80              | 9610.10791139         | 120.0                 | 48733.0               |
| 5              | 90              | 4823.62874417         | 121.0                 | 50738.0               |
| 5              | 100             | 4573.91798997         | 43.0                  | 50859.0               |

## Conclusion

The above metric is obtained from running the client and server in the same system, so it might not give an ideal representation of the latency. This simulation was run on an UBUNTU 18.04 machine with Intel(R) Core(TM) i7-6700HQ CPU @ 2.60GHz, with 8GB RAM . Also gettimeofday() API is used to generate time stamp, this could be replaced when testing on real distributed system for more accurate representation.(``include/ntp.h ``)

## Acknowledgements

This work is done as a part of Real Time Operating Systems course, with guidance from [Prof. Girish S Kumar](https://www.iiitb.ac.in/faculty/girish-s-kumar) .

