#! /usr/bin/env python3

import readline
import argparse
#import binascii
import chipwhisperer as cw
import numpy as np
import matplotlib.pyplot as plt
import time
import os
import sys
#import yaml
#import random
import re
from tqdm import tqdm

class Device:

    def __init__(self, deviceID, target, friendlyName):
        self.deviceID = deviceID
        self.target = target
        self.friendlyName = friendlyName

KnownDevices = {
    # STM32F0 devices:
    Device(0x444, 'stm32f0', "STM32F03xx4/03xx6"),
    Device(0x445, 'stm32f0', "STM32F04xxx"),
    Device(0x448, 'stm32f0', "STM32F071xx/STM32F072xx"),

    # STM32F1 devices:
    Device(0x410, 'stm32f1', "STM32F10xxx Medium-density"),
    Device(0x412, 'stm32f1', "STM32F10xxx Low-density"),
    Device(0x414, 'stm32f1', "STM32F10xxx High-density"),
    Device(0x416, 'stm32f1', "STM32F10xxx XL-density"),
    Device(0x420, 'stm32f1', "STM32F10xxx Medium-density value line"),
    Device(0x428, 'stm32f1', "STM32F10xxx High-density value line"),

    # STM32F2 devices:
    Device(0x411, 'stm32f2', "STM32F20x and STM32F21x"),

    # STM32F3 devices:
    Device(0x422, 'stm32f3', "STM32F303xB/C and STM32F358"),
    Device(0x438, 'stm32f3', "STM32F303x6/8 and STM32F328"),
    Device(0x446, 'stm32f3', "STM32F303xD/E and STM32F398xE"),

    # STM32F4 devices:
    Device(0x413, 'stm32f4', "STM32F40xxx/41xxx"),
}

class WorkBench:

    def __init__(self, cfg, verbose):
        self.verbose = verbose
        self.scope = cw.scope()
        self.scope.default_setup()
        if 'scope' in cfg:
            for k in cfg['scope']:
                if k == 'gain.db':
                    offset = float(cfg['scope'][k])
                    if self.verbose:
                        print("[local] Offseting scope.{} by: {} dB".format(k, offset))
                    self.scope.gain.db += offset

        self.target = cw.target(self.scope)

    def write(self, cmd):
        self.target.write(cmd)

    def read(self, num_bytes = 0, timeout = 250):
        return self.target.read(num_bytes, timeout)

    def write_buffer(self, array):
        array_b = np.array([np.frombuffer(array,dtype='B')]).T
        it = 0
        for byte in array_b:
            self.write(byte)
            it += 1
            if it == 64:
                self.read(1)
                it = 0

    def read_buffer(self, num_bytes, res_type):
        array_b = bytearray(num_bytes)
        it = 0
        for i in range(num_bytes):
            array_b[i] = ord(self.read(1, 500))
            it = it+1
            if it == 64:
                self.read(1)
                self.write('a')
                it = 0

        return np.frombuffer(array_b, dtype=res_type);

    def upload_firmware(self, hex_file):
        if self.verbose:
            print("Uploading file '{}'".format(hex_file))

        prog = cw.programmers.STM32FProgrammer
        cw.program_target(self.scope, prog, hex_file)

    def reset(self):
        # Reset the STM32 target.
        self.scope.io.nrst = 'low'
        time.sleep(0.020)
        self.scope.io.nrst = 'high'
        time.sleep(0.050)

        self.write('s')
        if not self.read(50) == 'ok':
            print("Waiting for target to come up (a manual reset may be required)... ")
            self.write('s')
            while not self.read(50) == 'ok':
                self.write('s')

        print("Target has been reset")

    def config_capture(self, num_samples):
        self.scope.adc.samples = num_samples

    def capture(self):
        self.scope.capture()

    def arm(self):
        self.scope.arm()

    def get_last_trace(self):
        return self.scope.get_last_trace()

# Save the traces and messages in .npy format.
def save_as_npy(traces_dir, traces, messages, verbose):
    if not os.path.exists(traces_dir):
        os.makedirs(traces_dir)

    np.save(os.path.join(traces_dir, 'traces'), traces)
    np.save(os.path.join(traces_dir, 'inputs'), messages)

# Save execution time information in text format.
def save_timing_information(traces_dir, timings, verbose):
    if not os.path.exists(traces_dir):
        os.makedirs(traces_dir)

    if verbose:
        print("Timing information (min, ave, max): {}".format(" ".join(["{}".format(t) for t in timings])))

    with open(os.path.join(traces_dir, 'timing.txt'), "w") as f:
        f.write(" ".join(["{}".format(t) for t in timings]))

def main():
    """Interact with the STM32 on the chipwhisperer board.
    """
    usage = """Usage: %(prog)s [options]

%(prog)s is used to optionaly upload a firmware, and run some trace acquisition or timing measurement.

"""
    init_parser = argparse.ArgumentParser(usage=usage)
    init_parser.add_argument("-v", "--verbose",
        action = 'count',
        default = 0,
        help = 'be more verbose, may be specified multiple times'
        )
    init_parser.add_argument("-f", "--firmware",
        metavar = 'HEX_FILE',
        help = """upload the firmware given as argument (in hex format). If this
        option is not used, it is assumed the firware on the target is the
        expected one."""
        )
    init_parser.add_argument("-i", "--board-information",
        action = "store_true",
        default = False,
        help = 'get board information'
        )    
    init_options = init_parser.parse_args()
    
    # Read the local configuration if one is found.
    local_config = dict()
    
    # ==========================================================================
    # Initialisation of the chipwhisperer target.
    # --------------------------------------------------------------------------
    wb = WorkBench(local_config.get('workbench', dict()), init_options.verbose)
    
    if init_options.firmware:
        wb.upload_firmware(init_options.firmware)
    
    wb.reset()
    
    if init_options.board_information:
        wb.write('i');
        buffer = wb.read_buffer(8, 'uint32') # We expect 2 x uint32_t (8 bytes).
        device = buffer[0]
        revision = buffer[1]
        Found = False
        for d in KnownDevices:
            if d.deviceID == device:
                print("Found known '{}' (DEV_ID: 0x{:04X}, REV_ID: 0x{:04X})".format(d.friendlyName, d.deviceID, revision))
                Found = True
                break
        if not Found:
            print("Unknown device found. Device: 0x{:04X} Revision: 0x{:04X}".format(device, revision))
        
    parser = argparse.ArgumentParser(usage=usage)
    parser.add_argument("action")
    parser.add_argument("-c", "--capture",
        action = "store_true",
        default = False,
        help = 'capture trace'
        )
    parser.add_argument("-p", "--plot",
        action = "store_true",
        default = False,
        help = 'plot trace'
        )
    parser.add_argument("-t", "--print",
        action = "store_true",
        default = False,
        help = 'print trace'
        )
    parser.add_argument("-a", "--accumulate",
        action = "store_true",
        default = False,
        help = 'print trace'
        )
    parser.add_argument("-s", "--samples",
        default = 500,
        help = 'Set the number of samples captured'
        )
    loop = True
    while loop:
        input_var = input(">")
        options = parser.parse_args(args=input_var.split(' '))
            
        if re.match("[0-9][0-9][0-9][0-9]$",options.action):
            wb.write('c');
            if options.capture:
                wb.config_capture(int(options.samples))
                wb.arm()
            wb.write(options.action)
            if options.capture:
                wb.capture()
            print(wb.read(50))
            if options.capture:
                trace = wb.get_last_trace()
                if options.plot:
                    if options.accumulate:
                        plt.ion()
                    else:
                        plt.clf()
                        plt.ioff()
                    plt.plot(trace)
                    plt.show()
                    if options.accumulate:
                        plt.pause(0.001)
                if options.print:
                    print(trace)
        
        elif options.action == 'clear':
            plt.clf()
            plt.pause(0.001)
        elif options.action == 'quit':
            loop = False
        elif options.action == 'status':
            wb.write('s');
            print(wb.read(50))
        elif options.action == 'board':
            wb.write('i');
            buffer = wb.read_buffer(8, 'uint32') # We expect 2 x uint32_t (8 bytes).
            device = buffer[0]
            revision = buffer[1]
            Found = False
            for d in KnownDevices:
                if d.deviceID == device:
                    print("Found known '{}' (DEV_ID: 0x{:04X}, REV_ID: 0x{:04X})".format(d.friendlyName, d.deviceID, revision))
                    Found = True
                    break
            if not Found:
                print("Unknown device found. Device: 0x{:04X} Revision: 0x{:04X}".format(device, revision))
        else:
            print('Unknown command')
if __name__ == "__main__":
    main()

