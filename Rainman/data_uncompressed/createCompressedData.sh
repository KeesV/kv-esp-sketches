#/bin/bash
rm -rf ../data/*
cp index.html ../data/index.html
mkdir -p ../data/bootstrap/css
gzip -c bootstrap/css/bootstrap.min.css > ../data/bootstrap/css/bootstrap.min.gz
