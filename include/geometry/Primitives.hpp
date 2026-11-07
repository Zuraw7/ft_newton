#ifndef GEOMETRY_PRIMITIVES_HPP
#define GEOMETRY_PRIMITIVES_HPP

#include "geometry/MeshData.hpp"

namespace newton::geometry
{
	MeshData makeCube();
	MeshData makePlane();
	MeshData makeSphere(unsigned int stacks = 16, unsigned int sectors = 32);

} // namespace newton::geometry

#endif // GEOMETRY_PRIMITIVES_HPP
