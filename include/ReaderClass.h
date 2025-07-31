#include <string>
#include <vector>

class Triangle 
{
public:
    // Define the vertices of the triangle
    std::vector<float> vertices; 
    
    // Constructor to initialize the triangle with vertices
    Triangle(const std::vector<float>& verts) : vertices(verts) {}


};
class Reader
{
public:
    // get all triangles from the model file
    virtual std::vector<Triangle> getTriangles() const = 0;

};