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


class Point(ct.Structure):
    _fields_ = [('x', ct.c_double), ('y', ct.c_double)]


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


@time_method
def create_struct_array(points: np.ndarray):
    x = points[:, 0]
    y = points[:, 1]
    elems = (Point*len(x))()
    struct_arr = ct.cast(elems, ct.POINTER(Point))
    for i in range(len(x)):
        struct_arr[i].x = x[i]
        struct_arr[i].y = y[i]
    return struct_arr


class ConvexHull:
    def __init__(self):
        dll = ct.cdll.LoadLibrary('../build/src/Release/convex_hull.dll')

        self._hull_from_points = wrap_function(dll, 'convex_hull',
                                               ct.c_int, [ct.POINTER(Point),
                                                          ct.c_int,
                                                          ct.POINTER(Point)])

    @time_method
    def run_from_points(self, points: np.ndarray):
        struct_array = create_struct_array(points)
        elems = (Point*len(points))()
        vertices = ct.cast(elems, ct.POINTER(Point))
        size = self._hull_from_points(struct_array,
                                      ct.c_int(len(points)),
                                      vertices)
        vertex_points = []
        for i in range(size):
            vertex_points.append([vertices[i].x, vertices[i].y])
        return vertex_points


if __name__ == '__main__':
    points = create_points(1000000)
    # run the CGAL method with the ctypes bindings
    hull = ConvexHull()
    vertices = hull.run_from_points(points)
    print(vertices)
