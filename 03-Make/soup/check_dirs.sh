#!/bin/bash

if [ -d $1 ]
then
echo "The $1 directory exists"
else
echo "The $1 directory does not exist. Create"
mkdir $1
fi