#pragma once
#include "../TemplateUtils.h"
#include "../Types.h"
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <iostream>
#include <string>
#include <tuple>
#include <array>

namespace ecs {
template <typename T>
byte* getAlignedAddress(byte* memory) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(memory);
    uintptr_t alignedAddr = (addr + alignof(T) - 1) & ~(alignof(T) - 1);
    return reinterpret_cast<byte*>(alignedAddr);
}

class ComponentManager;
class EntityManager;
class MovableBase;

class Entity final {
    friend class EntityManager;
    friend class ComponentManager;
    friend class MovableBase;

   public:
    Entity();
    Entity(EntityId id, EntityVersion version);
    EntityId getId() const;
    EntityVersion getVersion() const;

   private:
    EntityId id;
    EntityVersion version;
};

inline bool operator==(Entity const& a, Entity const& b) {
    return (a.getId() == b.getId()) && (a.getVersion() == b.getVersion());
}
inline bool operator!=(Entity const& a, Entity const& b) {
    return (a.getId() != b.getId()) || (a.getVersion() != b.getVersion());
}
inline bool operator<(Entity const& a, Entity const& b) {
    return a.getId() < b.getId();
}
inline bool operator>(Entity const& a, Entity const& b) {
    return a.getId() > b.getId();
}

class MovableBase {
    friend class ComponentManager;  // sets entity
   public:
    virtual ~MovableBase() = default;
    MovableBase() = default;
    MovableBase(MovableBase const&) = delete;
    MovableBase& operator=(MovableBase const&) = delete;
    MovableBase& operator=(MovableBase&& o) noexcept;
    MovableBase(MovableBase&& o) noexcept;

    Entity const& getEntity() const noexcept;
    virtual void moveTo(byte* destination) = 0;

   private:
    Entity entity;
};

struct EcsContainerBufferBase {
    virtual ~EcsContainerBufferBase() = default;
};

template <typename T>
class EcsContainerBuffer final : public EcsContainerBufferBase {
   public:
    using iterator = utils::Iterator<T>;
    using const_iterator = utils::ConstIterator<T>;

    EcsContainerBuffer() = default;
    ~EcsContainerBuffer() noexcept;
    EcsContainerBuffer(EcsContainerBuffer const&) = delete;
    EcsContainerBuffer(EcsContainerBuffer&&) = default;
    EcsContainerBuffer& operator=(EcsContainerBuffer const&) = delete;
    EcsContainerBuffer& operator=(EcsContainerBuffer&&) = default;
    size_t size() const noexcept;
    size_t capacity() const noexcept;
    void pop_back() noexcept;
    T* data() const noexcept;
    T* data() noexcept;
    T& back() noexcept;
    T& back() const noexcept;
    void clear() noexcept;
    template <typename S>
    S* castBack() const noexcept;
    bool empty() const noexcept;
    void resize(size_t newSize);
    size_t typeSize() const noexcept;
    utils::Iterator<T> begin() noexcept;
    utils::Iterator<T> end() noexcept;
    utils::ConstIterator<T> begin() const noexcept;
    utils::ConstIterator<T> end() const noexcept;
    template <typename S = T>
    utils::Iterator<S> castBegin();
    template <typename S = T>
    utils::Iterator<S> castEnd();
    template <typename S = T>
    utils::ConstIterator<S> castBegin() const;
    template <typename S = T>
    utils::ConstIterator<S> castEnd() const;
    T& operator[](size_t i) noexcept;
    T& operator[](size_t i) const noexcept;
    template <typename S = T, typename... Args>
    S* emplace_back(Args&&... args);

   private:
    template <typename S = T>
    void reallocate(size_t newCapacity = 0);

    size_t m_capacity = 0;
    size_t m_size = 0;
    size_t m_typeSize = 0;
    T* m_data = nullptr;
    byte* m_buffer = nullptr;
};

struct QueryCacheEntry {
    std::array<byte, 64> byteSig{};
    ComponentSig bitSig = 0;
    void* buffer = nullptr;
    bool dirty = false;
};

class ComponentQueryCache {
   public:
    using sig_t = std::pair<ComponentSig, std::array<byte, 64>>;
    template <typename... Components>
    size_t createCache(sig_t const& sig);
    template <typename TupleType>
    void addTuple(size_t index, TupleType const& cTuple);
    std::pair<size_t, QueryCacheEntry*> find(sig_t const& sig);
    void purge(ComponentId const& cId);
    QueryCacheEntry& operator[](size_t i);
    ~ComponentQueryCache();

   private:
    EcsContainerBuffer<QueryCacheEntry> cache;
};

class EntityManager final {
   public:
    Entity const& createEntity();
    bool exists(Entity const& entity) const;
    auto const& getEntitites() const;
    void removeEntity(Entity const& entity);
    size_t getCurrentEntityCount() const;
    size_t getMaximumEntityCount() const;
    Entity const& operator[](size_t index) const;

   private:
    EcsContainerBuffer<Entity> entities;
    EcsContainerBuffer<EntityId> removedIds;
    EcsContainerBuffer<EntityVersion> versions;
    size_t maxEntityCount = 0;
};

struct ComponentMetaData {
    // assumes 65536 max number of entities
    using index_type = uint16_t;
    ComponentSig bitSig = 0;
    index_type indices[64]{};
};

class ComponentManager final {
   public:
    ComponentManager(size_t componentTypesCount);
    template <typename Component, typename... Args>
    Component* addComponent(Entity const& entity, Args&&... args);
    template <typename Component>
    Component* getComponent(Entity const& entity, ComponentId cId) const;
    void removeComponent(Entity const& entity, ComponentId cId);
    bool hasComponent(Entity const& entity, ComponentId cId) const;
    auto getSignature(Entity const& entity) const;
    auto const& getMetaData() const;
    void addEntity(Entity const& entity);
    EcsContainerBuffer<byte>& operator[](size_t i);
    EcsContainerBuffer<byte>& operator[](size_t i) const;

   private:
    EcsContainerBuffer<EcsContainerBuffer<byte>> componentData;
    EcsContainerBuffer<ComponentMetaData> metaData;
};

template <typename Component, typename Tag, typename AllTags>
class ComponentBase : public MovableBase {
   public:
    using type = Component;
    using single_tag = Tag;
    using all_tags = AllTags;

    virtual ~ComponentBase() = default;
    ComponentBase() = default;
    ComponentBase(ComponentBase const&) = delete;
    ComponentBase& operator=(ComponentBase const&) = delete;
    ComponentBase& operator=(ComponentBase&& o) noexcept;
    ComponentBase(ComponentBase&& o) noexcept;

   private:
    virtual void moveTo(byte* destination) override;
};

class EcsContainer final {
   public:
    template <typename... ComponentTags>
    EcsContainer(TypeSequence<ComponentTags...>);
    ~EcsContainer() = default;
    EcsContainer(EcsContainer&) = delete;
    EcsContainer(EcsContainer&&) = delete;
    EcsContainer& operator=(EcsContainer&) = delete;
    EcsContainer& operator=(EcsContainer&&) = delete;

    Entity const& createEntity();
    bool exists(Entity const& entity) const;
    void removeEntity(Entity const& entity);
    size_t getCurrentEntityCount()
        const;  // eg. added 10 entities, removed 3 -> return 7
    size_t getMaximumEntityCount()
        const;  // returns a range of potentially valid ids (new entities reuse
                // ids of the deleted ones)
    template <typename Component>
    Component* getComponent(Entity const& entity);
    template <typename... Components>
    EcsContainerBuffer<std::tuple<Components*...>> const&
    getEntitiesWithComponents();
    template <typename Component, typename... Args>
    Component* addComponent(Entity const& entity, Args&&... args);
    template <typename Component>
    void removeComponent(Entity const& entity);
    template <typename Component>
    auto begin();
    template <typename Component>
    auto end();

    void printEntityCount() const;

   private:
    void removeComponentImpl(Entity const& entity, ComponentId cId);

    EntityManager entityManager;
    ComponentManager componentManager;
    ComponentQueryCache entityQueryCache;
    size_t const componentTypesCount;
};

template <typename Component>
struct ForEachComponent {
    ForEachComponent(EcsContainer& container) : container(container) {}
    auto begin() { return container.begin<Component>(); }
    auto end() { return container.end<Component>(); }
    EcsContainer& container;
};
}  // namespace ecs

#include "EcsContainerInl.hpp"