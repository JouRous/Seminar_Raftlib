# coding=utf8

import sys

from pyspark import SparkContext
from pyspark.streaming import StreamingContext

def mapF(x):
    s = x.split(" ")
    return (s[0], int(s[1]))

def updateF(values, runningValue):
    if runningValue is None:
        runningValue = 0
    return sum(values) + runningValue

if __name__ == "__main__":
    sc = SparkContext("local[2]", "NetworkWordCount")
    ssc = StreamingContext(sc, 3)
    ssc.checkpoint("/tmp")

    lines = ssc.socketTextStream("localhost", 9999)

    inR = lines.map(mapF)
    inR = inR.filter(lambda x: int(x[1]) > 5)

    accInr = inR.updateStateByKey(updateF)

    accInr.pprint(9999)


    ssc.start()
    ssc.awaitTermination()
