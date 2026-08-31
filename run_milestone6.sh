#!/bin/bash
set -e

echo "Building Jar..."
if [ ! -f "MatrixTranspose.jar" ]; then
    ./build_jar.sh
fi

echo "Compiling verification program..."
if [ ! -f "verify_transpose" ]; then
    g++ -std=c++17 -O2 verify_transpose.cpp -o verify_transpose
fi

echo "Starting YARN..."
docker exec -d namenode yarn resourcemanager || true
docker exec -d datanode1 yarn nodemanager || true
docker exec -d datanode2 yarn nodemanager || true

echo "Checking HDFS input..."
if ! docker exec namenode hdfs dfs -test -e /matrix_data/format_b; then
    if [ ! -d "format_b" ]; then
        if [ ! -f "generate_matrix" ]; then
            g++ -std=c++17 -O2 generate_matrix.cpp -o generate_matrix
        fi
        ./generate_matrix 1024 256
    fi
    docker exec namenode hdfs dfs -mkdir -p /matrix_data/format_b
    docker cp format_b namenode:/tmp/format_b
    docker exec namenode hdfs dfs -put -f /tmp/format_b /matrix_data/format_b
fi

echo "Submitting MapReduce job..."
docker cp MatrixTranspose.jar namenode:/tmp/MatrixTranspose.jar
docker exec namenode hdfs dfs -rm -r -f /matrix_data/output_milestone6 >/dev/null 2>&1 || true
docker exec namenode rm -rf /tmp/output_milestone6 >/dev/null 2>&1 || true

docker exec namenode hadoop jar /tmp/MatrixTranspose.jar MatrixTranspose /matrix_data/format_b /matrix_data/output_milestone6

echo "Fetching output..."
rm -rf output_milestone6
docker exec namenode hdfs dfs -get /matrix_data/output_milestone6 /tmp/output_milestone6
docker cp namenode:/tmp/output_milestone6 ./output_milestone6

echo "Verifying output..."
./verify_transpose ./output_milestone6
