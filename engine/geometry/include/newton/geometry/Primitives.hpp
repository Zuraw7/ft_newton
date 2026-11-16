#ifndef NEWTON_GEOMETRY_PRIMITIVES_HPP
#define NEWTON_GEOMETRY_PRIMITIVES_HPP

#include "newton/geometry/MeshData.hpp"

namespace newton::geometry
{
	MeshData makeCube();
	MeshData makePlane();
	MeshData makeSphere(unsigned int stacks = 16, unsigned int sectors = 32);

} // namespace newton::geometry

#endif // NEWTON_GEOMETRY_PRIMITIVES_HPP
