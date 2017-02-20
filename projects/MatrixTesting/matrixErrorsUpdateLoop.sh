#!/bin/bash

while true; do
    echo -n $(date) : $(./matrixErrors count-missing) " -> " && \
	./matrixErrors update && \
	./matrixErrors count-missing
    sleep 600
done
