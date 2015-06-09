"""Shared models for Code_TYMPAN"""

import cython as cy
from cython.view cimport array as cyarray

import numpy as np


cdef ospectre2spectrum(OSpectre os):
    """Spectrum (cython object) wrapping an OSpectre (c++)"""
    spectrum = Spectrum()
    spectrum.thisobj = os
    return spectrum

cdef opoint3d2point3d(OPoint3D pt):
    """Point3D (cython object) wrapping a Point3D (c++)"""
    point = Point3D()
    point.thisobj = pt
    return point

cdef ovector3d2vector3d(OVector3D vect):
    """Vector3D (cython object) wrapping a OVector3D (c++)"""
    vector = Vector3D()
    vector.thisobj = vect
    return vector

cdef otriangle2triangle(OTriangle* tri):
    """Triangle (cython object) wrapping an OTriangle(c++)"""
    assert tri != NULL
    triangle = cy.declare(Triangle)
    triangle = Triangle()
    triangle.thisptr = tri
    return triangle



cdef class Spectrum:

    @property
    def nvalues(self):
        """Number of values contained in the spectrum"""
        return self.thisobj.getNbValues()

    @property
    def values(self):
        """Values of the spectrum"""
        cdef cyarray cy_array = <double[:self.nvalues]> self.thisobj.getTabValReel()
        spec_val = np.array(cy_array, dtype=np.double)
        return spec_val

    def to_dB(self):
        """Convert the spectrum to a dB scale (by default: linear)"""
        return ospectre2spectrum(self.thisobj.toDB())

    @property
    def dBA(self):
        """Balanced spectrum value in DBA (for audible frequencies)"""
        return self.thisobj.valGlobDBA()


cdef class Triangle:

    def __cinit__(self):
        self.thisptr = NULL

    @property
    def p1(self):
        return self.thisptr._p1

    @property
    def p2(self):
        return self.thisptr._p2

    @property
    def p3(self):
        return self.thisptr._p3


cdef class Point3D:

    @property
    def x(self):
        return self.thisobj._x

    @property
    def y(self):
        return self.thisobj._y

    @property
    def z(self):
        return self.thisobj._z


cdef class Vector3D:

    @property
    def vx(self):
        return self.thisobj._x

    @property
    def vy(self):
        return self.thisobj._y

    @property
    def vz(self):
        return self.thisobj._z


cdef class Box:

    def __init__(self, length, width, height):
        """Build a 3D non iso-oriented box of dimensions (length, width, height)
        expressed in meters. The box will be centered in (0, 0, 0)
        """
        self.thisobj = OBox2(length, width, height)

