#!/bin/bash
set -e

if [ ! -d "hadoop_libs" ]; then
    echo "Copying Hadoop libraries..."
    mkdir -p ./hadoop_libs
    docker cp namenode:/opt/hadoop/share/hadoop/common ./hadoop_libs/
    docker cp namenode:/opt/hadoop/share/hadoop/mapreduce ./hadoop_libs/
fi

echo "Compiling Java code..."
javac -Xlint:-options --release 8 -classpath "hadoop_libs/common/*:hadoop_libs/common/lib/*:hadoop_libs/mapreduce/*:hadoop_libs/mapreduce/lib/*" MatrixTranspose.java
jar cf MatrixTranspose.jar MatrixTranspose*.class
