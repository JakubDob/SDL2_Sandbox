#include "CollisionSystem2D.h"
#include "../components/Collider2D.h"
#include "../components/Physics2D.h"
#include "../components/Transform2D.h"

namespace ecs {

CollisionSystem2D::CollisionSystem2D(Vec2 worldSize) : worldSize(worldSize) {
    cellSize[0] = worldSize[0] / 100.f;
    cellSize[1] = worldSize[1] / 100.f;
    totalCols = worldSize[0] / cellSize[0];
    totalRows = worldSize[1] / cellSize[1];
}

void CollisionSystem2D::broadPhase(ecs::EcsContainer& ecsContainer) {
    detections.clear();
    potentialCollisions.clear();
    for (auto& colliders : ecs::ForEachComponent<Collider2D>(ecsContainer)) {
        addDetectionData(colliders.getEntity(), colliders.getBoundingBox());
    }
    std::sort(detections.begin(), detections.end(),
              [](DetectionData const& l, DetectionData const& r) {
                  return l.gridCellIndex < r.gridCellIndex;
              });

    for (int i = 1; i < detections.size(); ++i) {
        auto d = detections[i - 1];
        if (d.gridCellIndex == detections[i].gridCellIndex) {
            for (int j = i; j < detections.size(); ++j) {
                if (detections[j].gridCellIndex != d.gridCellIndex) {
                    break;
                }
                auto idA = d.entity;
                auto idB = detections[j].entity;
                potentialCollisions.insert({idA, idB});
            }
        }
    }
}

void CollisionSystem2D::addDetectionData(Entity const& entity,
                                         Vec4 const& boundingBox) {
    /*
    Maps world coordinates to grid cells. Entities inside the same cells are
    potentially colliding. grid(0,0) <=> world(-worldSizeX/2, worldSizeY/2)
    grid(n,n) <=> world(worldSizeX/2, -worldSizeY/2)
    */
    auto halfWorldW = worldSize[0] / 2;
    auto halfWorldH = worldSize[1] / 2;
    if (boundingBox[0] < -halfWorldW || boundingBox[1] < -halfWorldH ||
        boundingBox[0] > halfWorldW || boundingBox[1] > halfWorldH) {
        return;
    }

    auto leftWorldBoundry = -halfWorldW;
    auto topWorldBoundry = halfWorldH;
    int topLeftCol = static_cast<int>(
        std::abs(boundingBox[0] - leftWorldBoundry) / cellSize[0]);
    int topLeftRow = static_cast<int>(
        std::abs(boundingBox[1] - topWorldBoundry) / cellSize[1]);
    int botRightCol = static_cast<int>(
        std::abs(boundingBox[0] + boundingBox[2] - leftWorldBoundry) /
        cellSize[0]);
    int botRightRow = static_cast<int>(
        std::abs(boundingBox[1] - boundingBox[3] - topWorldBoundry) /
        cellSize[1]);

    int width = botRightCol - topLeftCol + 1;
    int height = botRightRow - topLeftRow + 1;

    // add all cells if an object occupies more than one
    for (int i = topLeftRow; i < topLeftRow + height; ++i) {
        for (int j = topLeftCol; j < topLeftCol + width; ++j) {
            int cellIndex = i * totalCols + j;
            detections.push_back({entity, cellIndex});
        }
    }
}

void CollisionSystem2D::checkCollisions(ecs::EcsContainer& ecsContainer,
                                        scalar_t dt) {
    MinimumTranslation mtv;

    broadPhase(ecsContainer);
    for (auto const& pc : potentialCollisions) {
        auto idA = pc.first;
        auto idB = pc.second;
        auto const& collidersA =
            ecsContainer.getComponent<Collider2D>(idA)->getColliders();
        auto const& collidersB =
            ecsContainer.getComponent<Collider2D>(idB)->getColliders();
        for (auto const& cA : collidersA) {
            if (!cA->isActive()) continue;
            auto shape1 = cA->getShape();
            for (auto const& cB : collidersB) {
                if (!cB->isActive()) continue;
                auto shape2 = cB->getShape();
                if (shape1 == Shape::POLYGON) {
                    if (shape2 == Shape::POLYGON) {
                        if (polygonPolygon(*cA, *cB, mtv)) {
                            resolveCollision(
                                ecsContainer,
                                CollisionData(*cA, *cB, idA, idB, mtv), dt);
                        }
                    } else {
                        if (polygonCircle(*cA, *cB, mtv)) {
                            resolveCollision(
                                ecsContainer,
                                CollisionData(*cA, *cB, idA, idB, mtv), dt);
                        }
                    }
                } else {
                    if (shape2 == Shape::POLYGON) {
                        if (polygonCircle(*cB, *cA, mtv)) {
                            resolveCollision(
                                ecsContainer,
                                CollisionData(*cB, *cA, idB, idA, mtv), dt);
                        }
                    } else {
                        if (circleCircle(*cA, *cB, mtv)) {
                            resolveCollision(
                                ecsContainer,
                                CollisionData(*cA, *cB, idA, idB, mtv), dt);
                        }
                    }
                }
            }
        }
    }
    resolveAllCollisions(ecsContainer, dt);
}

bool CollisionSystem2D::areColliding(ecs::EcsContainer& ecsContainer, Entity a,
                                     Entity b,
                                     MinimumTranslation& outMtv) const {
    auto const& colliderA = ecsContainer.getComponent<Collider2D>(a);
    auto const& colliderB = ecsContainer.getComponent<Collider2D>(b);
    return areColliding(ecsContainer, *colliderA, *colliderB, outMtv);
}

bool CollisionSystem2D::areColliding(ecs::EcsContainer& ecsContainer,
                                     Collider2D const& a, Collider2D const& b,
                                     MinimumTranslation& outMtv) const {
    auto const& colliders1 = a.getColliders();
    auto const& colliders2 = b.getColliders();
    for (auto const& collider1 : colliders1) {
        if (!collider1->isActive()) continue;
        auto shape1 = collider1->getShape();
        for (auto const& collider2 : colliders2) {
            if (!collider2->isActive()) continue;
            auto shape2 = collider2->getShape();
            if (shape1 == Shape::POLYGON) {
                if (shape2 == Shape::POLYGON) {
                    if (polygonPolygon(*collider1, *collider2, outMtv)) {
                        return true;
                    }
                } else {
                    if (polygonCircle(*collider1, *collider2, outMtv)) {
                        return true;
                    }
                }
            } else {
                if (shape2 == Shape::POLYGON) {
                    if (polygonCircle(*collider2, *collider1, outMtv)) {
                        return true;
                    }
                } else {
                    if (circleCircle(*collider1, *collider2, outMtv)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void CollisionSystem2D::resolveAllCollisions(ecs::EcsContainer& ecsContainer,
                                             scalar_t dt) {
    for (auto& f : forces) {
        if (f.physics && !f.physics->isStatic()) {
            // f.transform->translate(f.translation / f.count);
            f.clampTranslation();
            f.transform->translate(f.translation * 0.99f);
            // dividing by count makes it impossible to bounce of off another
            // non static object when colliding with more than 1 object
            //  f.physics->applyForce(f.force / f.count);
            f.clampForce();
            f.physics->applyForce(f.force);
            // f.physics->scaleLinearVelocity(1.0f - dt); //remove energy from
            // the system
        }
    }
    this->forces.clear();
}

bool CollisionSystem2D::circleCircle(BaseCollider2D const& c1,
                                     BaseCollider2D const& c2,
                                     MinimumTranslation& out) const {
    auto const& centerOfColliderA = c1.getWorldSpacePosition();
    auto const& centerOfColliderB = c2.getWorldSpacePosition();
    auto radius1 = c1.getRadius();
    auto radius2 = c2.getRadius();
    auto dist = math::distance(centerOfColliderA, centerOfColliderB);
    if (dist < radius1 + radius2) {
        out.normal = Vec2(centerOfColliderA[0] - centerOfColliderB[0],
                          centerOfColliderA[1] - centerOfColliderB[1])
                         .normalize();
        out.magnitude = radius1 + radius2 - dist;
        return true;
    }
    return false;
}

bool CollisionSystem2D::polygonPolygon(BaseCollider2D const& c1,
                                       BaseCollider2D const& c2,
                                       MinimumTranslation& out) const {
    auto const& worldSpaceDataA = c1.getWorldSpaceData();
    auto const& worldSpaceDataB = c2.getWorldSpaceData();
    auto const& centerOfColliderA = c1.getWorldSpacePosition();
    auto const& centerOfColliderB = c2.getWorldSpacePosition();

    scalar_t minTranslationLen = std::numeric_limits<scalar_t>::max();
    Vec2 minTranslationNormal;
    scalar_t minA, maxA, minB, maxB;

    for (auto const& data : worldSpaceDataA) {
        findMinMaxProjectionPolygon(worldSpaceDataA, data.normal, minA, maxA);
        findMinMaxProjectionPolygon(worldSpaceDataB, data.normal, minB, maxB);
        if (maxA < minB || maxB < minA) {
            return false;
        }
        auto localMinLen = std::min(maxB - minA, maxA - minB);
        if (localMinLen < minTranslationLen) {
            minTranslationLen = localMinLen;
            minTranslationNormal = data.normal;
        }
    }
    for (auto const& data : worldSpaceDataB) {
        findMinMaxProjectionPolygon(worldSpaceDataA, data.normal, minA, maxA);
        findMinMaxProjectionPolygon(worldSpaceDataB, data.normal, minB, maxB);
        if (maxA < minB || maxB < minA) {
            return false;
        }
        auto localMinLen = std::min(maxB - minA, maxA - minB);
        if (localMinLen < minTranslationLen) {
            minTranslationLen = localMinLen;
            minTranslationNormal = data.normal;
        }
    }
    // mtv should point towards the first object, so flip the orientation if
    // that's not the case
    auto dir = centerOfColliderA - centerOfColliderB;
    if (math::dot2D(dir, minTranslationNormal) > 0) {
        out.normal = minTranslationNormal;
    } else {
        out.normal = -minTranslationNormal;
    }
    out.magnitude = minTranslationLen;

    return true;
}

bool CollisionSystem2D::polygonCircle(BaseCollider2D const& c1,
                                      BaseCollider2D const& c2,
                                      MinimumTranslation& out) const {
    auto const& worldSpaceDataA = c1.getWorldSpaceData();
    auto const& centerOfColliderA = c1.getWorldSpacePosition();
    auto const& centerOfColliderB = c2.getWorldSpacePosition();

    scalar_t minTranslationLen = std::numeric_limits<scalar_t>::max();
    Vec2 minTranslationNormal;
    scalar_t minA, maxA, minB, maxB;
    scalar_t radius = c2.getRadius();

    for (auto const& data : worldSpaceDataA) {
        findMinMaxProjectionPolygon(worldSpaceDataA, data.normal, minA, maxA);
        findMinMaxProjectionCircle(centerOfColliderB, radius, data.normal, minB,
                                   maxB);

        if (maxA < minB || maxB < minA) {
            return false;
        }
        auto localMinLen = std::min(maxB - minA, maxA - minB);
        if (localMinLen < minTranslationLen) {
            minTranslationLen = localMinLen;
            minTranslationNormal = data.normal;
        }
    }
    auto closestVert =
        findClosestVertexToPoint(centerOfColliderB, worldSpaceDataA);
    auto closestNormal = Vec2(closestVert[0] - centerOfColliderB[0],
                              closestVert[1] - centerOfColliderB[1])
                             .normalize();
    findMinMaxProjectionPolygon(worldSpaceDataA, closestNormal, minA, maxA);
    findMinMaxProjectionCircle(centerOfColliderB, radius, closestNormal, minB,
                               maxB);
    if (maxA < minB || maxB < minA) {
        return false;
    }
    auto localMinLen = std::min(maxB - minA, maxA - minB);
    if (localMinLen < minTranslationLen) {
        minTranslationLen = localMinLen;
        minTranslationNormal = closestNormal;
    }

    auto dir = centerOfColliderA - centerOfColliderB;
    if (math::dot2D(dir, minTranslationNormal) > 0) {
        out.normal = minTranslationNormal;
    } else {
        out.normal = -minTranslationNormal;
    }
    out.magnitude = minTranslationLen;

    return true;
}

Vec4 CollisionSystem2D::findClosestVertexToPoint(
    Vec4 const& point, std::vector<ColliderData> const& worldSpaceData) const {
    auto dist = std::numeric_limits<scalar_t>::max();
    int minIndex = 0;
    for (int i = 0; i < worldSpaceData.size(); ++i) {
        auto currentDistance =
            math::distanceSquared(worldSpaceData[i].vertex, point);
        if (currentDistance < dist) {
            dist = currentDistance;
            minIndex = i;
        }
    }
    return worldSpaceData[minIndex].vertex;
}

void CollisionSystem2D::findMinMaxProjectionPolygon(
    std::vector<ColliderData> const& worldSpaceData, Vec2 const& normal,
    scalar_t& inOutMin, scalar_t& inOutMax) const {
    inOutMin = std::numeric_limits<scalar_t>::max();
    inOutMax = -inOutMin;
    for (auto const& vertex : worldSpaceData) {
        auto proj = math::dot2D(vertex.vertex, normal);
        if (proj < inOutMin) inOutMin = proj;
        if (proj > inOutMax) inOutMax = proj;
    }
}

void CollisionSystem2D::findMinMaxProjectionCircle(Vec4 const& center,
                                                   scalar_t radius,
                                                   Vec2 const& normal,
                                                   scalar_t& inOutMin,
                                                   scalar_t& inOutMax) const {
    inOutMin = std::numeric_limits<scalar_t>::max();
    inOutMax = -inOutMin;
    auto center2 = Vec2(center[0], center[1]);
    auto directionAndRadius = normal * radius;
    auto p1OnCircle = center2 + directionAndRadius;
    auto p2OnCircle = center2 - directionAndRadius;
    inOutMin = math::dot2D(p1OnCircle, normal);
    inOutMax = math::dot2D(p2OnCircle, normal);
    if (inOutMin > inOutMax) {
        std::swap(inOutMin, inOutMax);
    }
}

void CollisionSystem2D::resolveCollision(ecs::EcsContainer& ecsContainer,
                                         CollisionData const& collision,
                                         scalar_t dt) {
    auto* ca = ecsContainer.getComponent<Collider2D>(collision.a);
    auto* cb = ecsContainer.getComponent<Collider2D>(collision.b);
    if (collision.cA.getType() == ColliderType::HITBOX) {
        if (collision.cB.getType() == ColliderType::PHYSICS) {
            if (ca->setCurrentHitboxTarget(collision.b)) {
                collision.cA.notifyHitbox(ecsContainer, collision.b);
            }
        }
        return;
    }

    if (collision.cB.getType() == ColliderType::HITBOX) {
        if (collision.cA.getType() == ColliderType::PHYSICS) {
            if (cb->setCurrentHitboxTarget(collision.a)) {
                collision.cB.notifyHitbox(ecsContainer, collision.a);
            }
        }
        return;
    }

    auto* pa = ecsContainer.getComponent<Physics2D>(collision.a);
    auto* pb = ecsContainer.getComponent<Physics2D>(collision.b);
    if (!pa || !pb) {
        return;
    }

    if (!(pa->isStatic() && pb->isStatic())) {
        collision.cA.notifyHitbox(ecsContainer, collision.b);
        collision.cB.notifyHitbox(ecsContainer, collision.a);
    }

    forces.resize(ecsContainer.getMaximumEntityCount());

    auto* ta = ecsContainer.getComponent<Transform2D>(collision.a);
    auto* tb = ecsContainer.getComponent<Transform2D>(collision.b);
    auto aId = collision.a.getId();
    auto bId = collision.b.getId();
    forces[aId].transform = ta;
    forces[bId].transform = tb;
    forces[aId].physics = pa;
    forces[bId].physics = pb;

    bool groundA = math::dot2D(collision.mtv.normal, Vec2(0.0f, 1.0f)) > 0.99f;
    bool groundB = math::dot2D(collision.mtv.normal, Vec2(0.0f, 1.0f)) < -0.99f;

    if (groundA) {
        pa->setGrounded(true);
    }
    if (groundB) {
        pb->setGrounded(true);
    }

    auto impulse = Physics2D::impulse(*pa, *pb, collision.mtv.normal, 1.0);
    if (!pa->isStatic() && !pb->isStatic()) {
        // separate objects
        Vec2 moveA, moveB;

        if (pa->isPartiallyStatic() ||
            (pa->isGrounded() && !pb->isGrounded())) {
            moveB = -collision.mtv.normal * (collision.mtv.magnitude * 1.0f);
        } else if (pb->isPartiallyStatic() ||
                   (pb->isGrounded() && !pa->isGrounded())) {
            moveA = collision.mtv.normal * (collision.mtv.magnitude * 1.0f);
        } else {
            moveA = collision.mtv.normal * (collision.mtv.magnitude * 0.5f);
            moveB = -collision.mtv.normal * (collision.mtv.magnitude * 0.5f);
        }
        forces[aId].addTranslation(moveA);
        forces[bId].addTranslation(moveB);
        if (!pa->isPartiallyStatic()) {
            forces[aId].addForce(impulse);
            forces[aId].addForce(-impulse * pb->getFriction() / pa->getMass());
        }
        if (!pb->isPartiallyStatic()) {
            forces[bId].addForce(-impulse);
            forces[bId].addForce(impulse * pa->getFriction() / pb->getMass());
        }
    } else if (pa->isStatic() && !pb->isStatic()) {
        auto moveB = -collision.mtv.normal * (collision.mtv.magnitude);
        forces[bId].addTranslation(moveB);
        // prevent partially static object intersecting with static obj but
        // ignore mass
        if (pb->isPartiallyStatic()) {
            pb->applyEqualOppositeForce(collision.mtv.normal);
        } else {
            forces[bId].addForce(-impulse);
            forces[bId].addForce(impulse * pa->getFriction() / pb->getMass());
        }
    } else if (pb->isStatic() && !pa->isStatic()) {
        auto moveA = collision.mtv.normal * (collision.mtv.magnitude);
        forces[aId].addTranslation(moveA);
        if (pa->isPartiallyStatic()) {
            pa->applyEqualOppositeForce(collision.mtv.normal);
        } else {
            forces[aId].addForce(impulse);
            forces[aId].addForce(-impulse * pb->getFriction() / pa->getMass());
        }
    }
    // both static
    else {
    }
}

void CollisionSystem2D::renderBoundingBoxes(ecs::EcsContainer& ecsContainer,
                                            Renderer& renderer,
                                            Shader const& shader) {
    shader.use();
    shader.setVec4("color", Vec4({1, 1, 1, 0.1}));
    float depth = 0.99f;
    for (auto& collider : ecs::ForEachComponent<Collider2D>(ecsContainer)) {
        QuadMesh mesh(QuadMesh::center, collider.getBoundingBox());
        mesh.setDepth(depth);
        auto meshId = renderer.addMesh(mesh);
        RenderCommand rc(meshId, shader);
        rc.opaque = false;
        rc.state.depthState.enabled = true;
        rc.state.depthState.depth = depth;
        renderer.addRenderCommand(rc);
    }
}

// all params are in the world space, checks if line (a<->b) intersects (c<->d)
bool CollisionSystem2D::linesIntersect(Vec4 const& a, Vec4 const& b,
                                       Vec4 const& c, Vec4 const& d,
                                       Vec4& outIntersectPoint) const {
    Vec4 r = b - a;
    Vec4 s = d - c;
    auto cross = cross2DAnalog(r, s);
    auto u = cross2DAnalog(c - a, r) / cross;
    auto t = cross2DAnalog(c - a, s) / cross;
    if ((u >= 0 && u <= 1) && (t >= 0 && t <= 1)) {
        outIntersectPoint = a + t * r;
        return true;
    }
    return false;
}

}  // namespace ecs