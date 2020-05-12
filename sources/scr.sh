#!/bin/bash

sed 's/\t/ /g' source.cpp > header1.cpp
rm source.cpp
mv header1.cpp source.cpp
