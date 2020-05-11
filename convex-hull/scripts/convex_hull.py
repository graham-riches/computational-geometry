# -*- coding: utf-8 -*-
"""
@brief
@author: Graham Riches
@date: Sun May  3 14:04:31 2020
@description
    python ctypes binding for the C++ convex hull algorithm with data
    passed between the two platforms in a json string format (probably not
    the most performant, but useful!)
"""

import ctypes as ct
import time
import numpy as np
import sys
import matplotlib.pyplot as plt

sys.path.append('../lib/json-cgal/Scripts')
from json_cgal import JsonCGAL, Point_2


def time_method(method):
    """ wrapper decorator for timing functions """
    def timed(*args, **kwargs):
        start_time = time.perf_counter_ns()
        result = method(*args, **kwargs)
        end_time = time.perf_counter_ns()
        print('PERF_TIME: {} : {}ms'.format(method.__name__,
                                            (end_time-start_time)/1E6))
        return result
    return timed


def wrap_function(lib, funcname, restype, argtypes):
    """Simplify wrapping ctypes functions"""
    func = lib.__getattr__(funcname)
    func.restype = restype
    func.argtypes = argtypes
    return func


@time_method
def create_points(n: int):
    points = np.random.randn(n, 2)*100
    return points


class ConvexHull:
    def __init__(self):
        dll = ct.cdll.LoadLibrary('../build/src/Release/convex_hull.dll')

        self._run = wrap_function(dll, 'convex_hull',
                                  None, [ct.c_char_p,
                                         ct.c_char_p])

    @time_method
    def run(self, points: np.ndarray):
        data = JsonCGAL()
        data.points = [Point_2(point[0], point[1]) for point in points]
        string_data = data.encode()
        input_buffer = ct.create_string_buffer(string_data.encode('utf-8'))
        output_buffer = ct.create_string_buffer(''.encode(),
                                                size=len(input_buffer))
        self._run(input_buffer, output_buffer)
        output_data = JsonCGAL()
        output_data.decode(output_buffer.value.decode('utf-8'))
        vertices = output_data.points
        output_points = np.ndarray((len(vertices), 2))
        for i in range(len(vertices)):
            output_points[i] = [vertices[i].x, vertices[i].y]
        return output_points


if __name__ == '__main__':
    points = create_points(1000)
    hull = ConvexHull()
    vertices = hull.run(points)
    # plot the data
    x_points = [point[0] for point in points]
    y_points = [point[1] for point in points]
    x_vertices = [vertex[0] for vertex in vertices]
    y_vertices = [vertex[1] for vertex in vertices]
    # re-add the first point to the end of the list to create a closed segment
    x_vertices.append(x_vertices[0])
    y_vertices.append(y_vertices[0])
    fig = plt.figure()
    plt.plot(x_points, y_points, 'ok')
    plt.plot(x_vertices, y_vertices, '-or')
    plt.show()
