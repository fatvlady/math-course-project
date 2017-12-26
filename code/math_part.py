from random import uniform
import numpy as np

# TODO: optimize code

parking_length = 1000
times = 10000

def resolve_interval(length):
    if length < 1:
        return tuple()
    left_length = uniform(0, length - 1)
    right_length = length - left_length - 1
    return left_length, right_length

def calculate_cars(x, consumer):
    intervals = [np.float128(x)]
    count = 0
    while len(intervals) > 0:
        pick = intervals.pop(0)
        if pick >= 1:
            count += 1
        intervals.extend(list(consumer(pick)))
    return count

import itertools
print(np.average([calculate_cars(x, resolve_interval) for x in itertools.repeat(parking_length, times)])
      / parking_length)

def middle_placer(length):
    if length < 1:
        return tuple()
    return (length - 1) / 2, (length - 1) / 2

# [calculate_cars(consumer=middle_placer, x = value) for value in range(1, 50)]