"""This package provides altimetry computation from Topography and Infrastructures.

It relies on :

* The Shapely library for preprocessing
  (http://toblerity.org/shapely/index.html)
* The CGAL library and its bindings for triangulation and meshing:

  - http://code.google.com/p/cgal-bindings/
  - http://doc.cgal.org/latest/Triangulation_2/index.html#Chapter_2D_Triangulations
  - http://doc.cgal.org/latest/Mesh_2/index.html

It is devided in several modules:

* The ``datamodel`` module provides a representation for the
  geometrical feature of a Code_TYMPAN site
* The ``merge`` module provides for merging subsite and cleaning
  out-of-bounds elements
* The ``mesh`` modules provide for building a constrained
  triangulation for a single-node (i.e. already merged) site
* The ``builder`` module chain operation from all modules to actually
  build the altimetry from the topography.
* The ``visu`` provides visualtisation utilities mainly aimed as
  diagnostic helpers
"""



import sys, os
import os.path as osp

from ._export import export_to_ply


_HERE = osp.realpath(osp.dirname(__file__))
_PROJECT_BASE = osp.abspath(osp.join(_HERE, '..', '..', '..'))


try:
    from CGAL import CGAL_Kernel
except ImportError:
    # TODO For now we assume CGAL_BINDINGS_PATH is set as an environment variable
    #      It would be better to compute it from _PROJECT_BASE once cgal-bindings
    #      are integrated into the project as 3rd party dependency
    CGAL_BINDINGS_PATH = os.environ.get('CGAL_BINDINGS_PATH')

    if CGAL_BINDINGS_PATH is None or not osp.isdir(CGAL_BINDINGS_PATH):
        raise ImportError("A `CGAL_BINDINGS_PATH` environment variable is expected"
                           " to point to the directory holding cgal-bindings binary modules")
    try:
        # Adjusting sys.path in order to find modules shipped with Code_TYMPAN
        sys.path.append(CGAL_BINDINGS_PATH)
        from CGAL import CGAL_Kernel
    except ImportError as exc:
        raise ImportError("Could not load the CGAL bindings even when looking in the "
                          "`CGAL_BINDINGS_PATH` environment variable ({0}). "
                          "The errors is : {1}".format(CGAL_BINDINGS_PATH, exc))


class AltimetryMesh(object):
    """Holds a merged SiteNode and a mesh (i.e. faces and nodes)"""
    from tympan.altimetry import builder

    def __init__(self, site, mesh, feature_by_face):
        self.equivalent_site = site
        self.mesh = mesh
        self.feature_by_face = feature_by_face.copy()
        self._material_by_face = None

    @classmethod
    def from_site(cls, site, **kwargs):
        """Build an altimetry mesh from a tympan Site.

        Extra keyword arguments are passed to the mesh builder (see
        tympan.altimetry.builder.build_altimetry).
        """
        asite = builder.build_sitenode(site)
        # Compute altimetry and retrieve the resulting mesh
        merged_site, mesh, feature_by_face = builder.build_altimetry(
            asite, **kwargs)
        return cls(merged_site, mesh, feature_by_face)

    @property
    def material_by_face(self):
        """Return a material_by_face mapping"""
        if self._material_by_face is None:
            self._material_by_face = builder.material_by_face(
                self.feature_by_face)
        return self._material_by_face

    def to_ply(self, fpath):
        """Export mesh content to 'fpath' (PLY format)"""
        export_to_ply(self.mesh, self.material_by_face, fpath)
