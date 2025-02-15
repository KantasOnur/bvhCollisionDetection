#include "AABB.h"

AABB::AABB(const MeshLoader::Mesh* mesh, const float& scale, const glm::vec3& position)
	: m_scale(scale), m_position(position), Mesh("cube")
{
    _updateModelMatrix();
	m_min = mesh->min;
	m_max = mesh->max;
}

void AABB::draw(const Camera& camera)
{
    
    _updateModelMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
	m_shader.bind();
	m_shader.setMatrix4f("projectionMatrix", camera.getProjection());
	m_shader.setMatrix4f("viewMatrix", camera.getView());
	m_shader.setMatrix4f("modelMatrix", m_modelMatrix);
    m_shader.setVec4f("min", m_min);
    m_shader.setVec4f("max", m_max);

	_drawMesh(camera);
    /*
	//draw minkowksi sum 
	m_shader.setMatrix4f("modelMatrix", glm::mat4(1.0f));
    m_shader.setVec4f("min", glm::vec4(summed_min, 1.0f));
    m_shader.setVec4f("max", glm::vec4(summed_max, 1.0f));

	_drawMesh(camera);
    */
	m_shader.unbind();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void AABB::MinkowskiSum(const AABB& aabb)
{
    glm::vec3 min, max, my_min, my_max;
    aabb.getMinMax(min, max);
    getMinMax(my_min, my_max);

    float width = (max - min).x;
    float height = (max - min).y;
    float depth = (max - min).z;


    summed_min = my_min;
    summed_max = my_max;

    summed_max.x += width * 0.5f;
    summed_max.y += height * 0.5f;
    summed_max.z += height * 0.5f;

    summed_min.x -= width  * 0.5f;
    summed_min.y -= height * 0.5f;
    summed_min.z -= depth  * 0.5f;

    summed_min = glm::translate(glm::mat4(1.0f), m_position) * glm::vec4(summed_min, 1.0f);
    summed_max = glm::translate(glm::mat4(1.0f), m_position) * glm::vec4(summed_max, 1.0f);
}

void AABB::getMinMax(glm::vec3& min, glm::vec3& max) const
{
    min = m_scale * m_min;
    max = m_scale * m_max;
}

bool AABB::rayMinkowksiSumIntersection(const AABB& aabb, const glm::vec3& rayEndPoint,
	const glm::vec3& rayOrigin, float& t)
{
    
	MinkowskiSum(aabb);
	return rayIntersectsAABB(rayOrigin, rayEndPoint, summed_min , summed_max, t);
    
}

void AABB::_updateModelMatrix()
{
    m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(m_scale));
}

bool AABB::rayIntersectsAABB(const glm::vec3& rayOrigin,
    const glm::vec3& rayEnd,
    const glm::vec3& boxMin,
    const glm::vec3& boxMax,
    float& tMinOut)
{
    
    glm::vec3 rayDir = rayEnd - rayOrigin; 
    glm::vec3 invDir = 1.0f / rayDir;

    float tMin = -std::numeric_limits<float>::infinity();
    float tMax = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; i++)
    {
        float t1 = (boxMin[i] - rayOrigin[i]) * invDir[i];
        float t2 = (boxMax[i] - rayOrigin[i]) * invDir[i];

        if (t1 > t2) std::swap(t1, t2);

        tMin = glm::max(tMin, t1);
        tMax = glm::min(tMax, t2);

        if (tMax < tMin)
            return false;
    }

    tMinOut = tMin;

    return (tMin >= 0.0f && tMin <= 1.0f);
}

glm::vec3 AABB::getMidPoint()
{
    glm::vec3 localMidpoint = (m_min + m_max) * 0.5f;
    return glm::vec3(m_modelMatrix * glm::vec4(localMidpoint, 1.0f));
}