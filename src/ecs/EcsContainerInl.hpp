#pragma once
namespace ecs {
/*======================Entity========================================*/
inline constexpr auto INVALID_ENTITY_ID = std::numeric_limits<EntityId>::max();
inline constexpr auto INVALID_ENTITY_VERSION =
    std::numeric_limits<EntityVersion>::max();
inline constexpr auto INVALID_COMPONENT_SIGNATURE = 0;

inline Entity::Entity()
    : id(INVALID_ENTITY_ID), version(INVALID_ENTITY_VERSION) {}

inline Entity::Entity(EntityId id, EntityVersion version)
    : id(id), version(version) {}

inline EntityId Entity::getId() const { return id; }
inline EntityId Entity::getVersion() const { return version; }
/*======================EntityManager========================================*/

inline Entity const& EntityManager::createEntity() {
    if (removedIds.empty()) {
        Entity entity(entities.size(), 0);
        entities.emplace_back(entity);
        versions.emplace_back(0);
        ++maxEntityCount;
        return entities.back();
    } else {
        auto lastRemoved = removedIds.back();
        removedIds.pop_back();
        entities[lastRemoved].version = versions[lastRemoved];
        return entities[lastRemoved];
    }
}

inline void EntityManager::removeEntity(Entity const& entity) {
    versions[entity.id] += 1;
    removedIds.emplace_back(entity.id);
}

inline bool EntityManager::exists(Entity const& entity) const {
    if (entities.size() > entity.id) {
        if (versions[entity.id] == entity.version) {
            return true;
        }
    }
    return false;
}

inline auto const& EntityManager::getEntitites() const { return entities; }

inline size_t EntityManager::getCurrentEntityCount() const {
    return entities.size() - removedIds.size();
}
inline size_t EntityManager::getMaximumEntityCount() const {
    return maxEntityCount;
}
inline Entity const& EntityManager::operator[](size_t index) const {
    return entities[index];
}
/*======================EcsContainerBuffer========================================*/

template <typename T>
inline EcsContainerBuffer<T>::~EcsContainerBuffer() noexcept {
    if (m_buffer) {
        delete[] m_buffer;
    } else if (m_data) {
        delete[] m_data;
    }
}
template <typename T>
inline size_t EcsContainerBuffer<T>::size() const noexcept {
    return m_size;
}
template <typename T>
inline size_t EcsContainerBuffer<T>::typeSize() const noexcept {
    return m_typeSize;
}
template <typename T>
inline size_t EcsContainerBuffer<T>::capacity() const noexcept {
    return m_capacity;
}
template <typename T>
inline T* EcsContainerBuffer<T>::data() const noexcept {
    return m_data;
}
template <typename T>
inline T* EcsContainerBuffer<T>::data() noexcept {
    return m_data;
}
template <typename T>
inline T& EcsContainerBuffer<T>::back() noexcept {
    return m_data[m_size - 1];
}
template <>
inline byte& EcsContainerBuffer<byte>::back() noexcept {
    return m_data[m_typeSize * (m_size - 1)];
}

template <typename T>
inline T& EcsContainerBuffer<T>::back() const noexcept {
    return m_data[m_size - 1];
}
template <>
inline byte& EcsContainerBuffer<byte>::back() const noexcept {
    return m_data[m_typeSize * (m_size - 1)];
}
template <typename T>
inline bool EcsContainerBuffer<T>::empty() const noexcept {
    return m_size == 0;
}
template <typename T>
inline void EcsContainerBuffer<T>::clear() noexcept {
    if constexpr (std::is_same_v<T, byte>) {
        for (size_t i = 0; i < m_size; ++i) {
            auto* base =
                reinterpret_cast<MovableBase*>(m_data + i * m_typeSize);
            base->~MovableBase();
        }
    } else {
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i].~T();
        }
    }
    m_size = 0;
}
template <typename T>
inline utils::Iterator<T> EcsContainerBuffer<T>::begin() noexcept {
    return utils::Iterator<T>(m_data);
}
template <typename T>
inline utils::Iterator<T> EcsContainerBuffer<T>::end() noexcept {
    return utils::Iterator<T>(m_data + m_size);
}
template <typename T>
inline utils::ConstIterator<T> EcsContainerBuffer<T>::begin() const noexcept {
    return utils::ConstIterator<T>(m_data);
}
template <typename T>
inline utils::ConstIterator<T> EcsContainerBuffer<T>::end() const noexcept {
    return utils::Iterator<T>(m_data + m_size);
}
template <typename T>
inline void EcsContainerBuffer<T>::resize(size_t newSize) {
    for (auto i = newSize; i < m_size; ++i) {
        pop_back();
    }
    if (m_capacity <= newSize) {
        if (newSize < 8) {
            reallocate();
        } else {
            reallocate(newSize * 2);
        }
    }
    for (auto i = m_size; i < newSize; ++i) {
        m_data[i] = {};
    }
    m_size = newSize;
}
template <typename T>
inline T& EcsContainerBuffer<T>::operator[](size_t i) noexcept {
    return m_data[i];
}
template <>
inline byte& EcsContainerBuffer<byte>::operator[](size_t i) noexcept {
    return m_data[i * m_typeSize];
}
template <typename T>
inline T& EcsContainerBuffer<T>::operator[](size_t i) const noexcept {
    return m_data[i];
}
template <>
inline byte& EcsContainerBuffer<byte>::operator[](size_t i) const noexcept {
    return m_data[i * m_typeSize];
}

template <typename T>
template <typename S, typename... Args>
inline S* EcsContainerBuffer<T>::emplace_back(Args&&... args) {
    // allocate different type than class template, eg. T = byte, S =
    // SomeComponent
    if (m_size == m_capacity) {
        reallocate<S>();
    }
    S* ret = nullptr;
    if constexpr (std::is_same_v<T, byte>) {
        ret = new (&m_data[m_typeSize * m_size]) S(std::forward<Args>(args)...);
    } else {
        ret = new (&m_data[m_size]) S(std::forward<Args>(args)...);
    }
    ++m_size;
    return ret;
}

template <typename T>
inline void EcsContainerBuffer<T>::pop_back() noexcept {
    auto& last = back();
    last.~T();
    --m_size;
}

template <>
inline void EcsContainerBuffer<byte>::pop_back() noexcept {
    auto* last = castBack<MovableBase>();
    last->~MovableBase();
    --m_size;
}

template <typename T>
template <typename S>
inline void EcsContainerBuffer<T>::reallocate(size_t newCapacity) {
    if (newCapacity == 0) {
        if (m_capacity == 0) {
            m_capacity = 8;
        } else {
            m_capacity *= 2;
        }
    } else {
        m_capacity = newCapacity;
    }
    if (m_typeSize == 0) {
        m_typeSize = sizeof(S);
    } else if (m_typeSize != sizeof(S)) {
        throw std::runtime_error(
            "Trying to allocate a type of a different size than the first one: "
            "Initially " +
            std::to_string(m_typeSize) + ", now " + std::to_string(sizeof(S)) +
            "\n");
    }

    if constexpr (std::is_same_v<T, byte>) {
        // potentially unaligned byte buffer storing polymorphic type S
        byte* newBuffer = new byte[m_capacity * m_typeSize + alignof(S)];
        byte* newData = getAlignedAddress<S>(newBuffer);
        if (m_buffer) {
            for (size_t i = 0; i < m_size; ++i) {
                auto* srcBase =
                    reinterpret_cast<MovableBase*>(m_data + i * m_typeSize);
                auto* dstBase = newData + i * m_typeSize;
                srcBase->moveTo(dstBase);
                srcBase->~MovableBase();
            }
            delete[] m_buffer;
        }
        m_data = newData;
        m_buffer = newBuffer;
    } else {
        // container type matches allocated type, no need to use byte buffer
        T* newData = new T[m_capacity];
        if (m_data) {
            std::move(begin(), end(), utils::Iterator<T>(newData));
        }
        delete[] m_data;
        m_data = newData;
    }
}

template <typename T>
template <typename S>
inline utils::Iterator<S> EcsContainerBuffer<T>::castBegin() {
    return utils::Iterator<S>(reinterpret_cast<S*>(m_data));
}

template <typename T>
template <typename S>
inline utils::Iterator<S> EcsContainerBuffer<T>::castEnd() {
    return utils::Iterator<S>(
        reinterpret_cast<S*>(m_data + m_typeSize * m_size));
}
template <typename T>
template <typename S>
utils::ConstIterator<S> EcsContainerBuffer<T>::castBegin() const {
    return utils::Iterator<S>(reinterpret_cast<S*>(m_data));
}
template <typename T>
template <typename S>
utils::ConstIterator<S> EcsContainerBuffer<T>::castEnd() const {
    return utils::ConstIterator<S>(
        reinterpret_cast<S*>(m_data + m_typeSize * m_size));
}

template <typename T>
template <typename S>
inline S* EcsContainerBuffer<T>::castBack() const noexcept {
    return reinterpret_cast<S*>(&m_data[m_typeSize * (m_size - 1)]);
}

/*======================MovableBase========================================*/

inline MovableBase::MovableBase(MovableBase&& o) noexcept : entity(o.entity) {
    o.entity.id = 0;
}

inline MovableBase& MovableBase::operator=(MovableBase&& o) noexcept {
    if (this != &o) {
        this->entity = o.entity;
        o.entity.id = 0;
    }
    return *this;
}

inline Entity const& MovableBase::getEntity() const noexcept { return entity; }
/*======================ComponentBase========================================*/

template <typename Component, typename Tag, typename AllTags>
inline ComponentBase<Component, Tag, AllTags>&
ComponentBase<Component, Tag, AllTags>::operator=(
    ComponentBase<Component, Tag, AllTags>&& o) noexcept {
    if (this != &o) {
        MovableBase::operator=(std::move(o));
    }
    return *this;
}

template <typename Component, typename Tag, typename AllTags>
inline ComponentBase<Component, Tag, AllTags>::ComponentBase(
    ComponentBase<Component, Tag, AllTags>&& o) noexcept
    : MovableBase(std::move(o)) {}

template <typename Component, typename Tag, typename AllTags>
inline void ComponentBase<Component, Tag, AllTags>::moveTo(byte* destination) {
    auto& instance = static_cast<Component&>(*this);
    new (destination) Component(std::move(instance));
}

/*======================ComponentManager========================================*/

inline ComponentManager::ComponentManager(size_t componentTypesCount) {
    componentData.resize(componentTypesCount);
}

template <typename Component, typename... Args>
inline Component* ComponentManager::addComponent(Entity const& entity,
                                                 Args&&... args) {
    using SingleTag = typename Component::single_tag;
    using AllTags = typename Component::all_tags;
    static_assert(std::is_base_of_v<MovableBase, Component>,
                  "Invalid type of obj");
    static_assert(utils::containsTypeInSequence<SingleTag>(AllTags{}),
                  "Component tag was not defined");
    constexpr ComponentId cId =
        utils::getTypeIndexFromSequence<SingleTag>(AllTags{});

    auto* component =
        componentData[cId].emplace_back<Component>(std::forward<Args>(args)...);
    component->entity = entity;
    metaData[entity.id].bitSig |= 1ULL << cId;
    metaData[entity.id].indices[cId] =
        static_cast<ComponentMetaData::index_type>(componentData[cId].size() -
                                                   1);
    return component;
}

inline void ComponentManager::removeComponent(Entity const& entity,
                                              ComponentId cId) {
    if (hasComponent(entity, cId)) {
        auto delI = metaData[entity.id].indices[cId];
        auto* del = reinterpret_cast<MovableBase*>(&componentData[cId][delI]);
        auto* lastOfType = componentData[cId].castBack<MovableBase>();
        auto lastOfTypeI = lastOfType->entity.id;
        del->~MovableBase();
        lastOfType->moveTo(&componentData[cId][delI]);
        componentData[cId].pop_back();
        metaData[entity.id].bitSig &= ~(1ULL << cId);
        metaData[lastOfTypeI].indices[cId] = metaData[entity.id].indices[cId];
    }
}

template <typename Component>
inline Component* ComponentManager::getComponent(Entity const& entity,
                                                 ComponentId cId) const {
    if (hasComponent(entity, cId)) {
        auto componentIndex = metaData[entity.id].indices[cId];
        auto found = componentData[cId].castBegin<Component>() + componentIndex;
        return &*found;
    }
    return nullptr;
}

inline EcsContainerBuffer<byte>& ComponentManager::operator[](size_t i) {
    return componentData[i];
}

inline EcsContainerBuffer<byte>& ComponentManager::operator[](size_t i) const {
    return componentData[i];
}

inline bool ComponentManager::hasComponent(Entity const& entity,
                                           ComponentId cId) const {
    return metaData[entity.id].bitSig & (1ULL << cId);
}

inline auto ComponentManager::getSignature(Entity const& entity) const {
    return metaData[entity.id].bitSig;
}

inline auto const& ComponentManager::getMetaData() const { return metaData; }

inline void ComponentManager::addEntity(Entity const& entity) {
    if (entity.id >= metaData.size()) {
        metaData.resize(entity.id + 1);
    }
}

/*======================EcsContainer========================================*/

template <typename... ComponentTags>
inline EcsContainer::EcsContainer(TypeSequence<ComponentTags...>)
    : componentTypesCount(sizeof...(ComponentTags)),
      componentManager(sizeof...(ComponentTags)) {}

inline Entity const& EcsContainer::createEntity() {
    auto const& entity = entityManager.createEntity();
    componentManager.addEntity(entity);
    return entity;
}

template <typename Component, typename... Args>
inline Component* EcsContainer::addComponent(Entity const& entity,
                                             Args&&... args) {
    using SingleTag = typename Component::single_tag;
    using AllTags = typename Component::all_tags;
    static_assert(std::is_base_of_v<MovableBase, Component>,
                  "Invalid type of obj");
    static_assert(utils::containsTypeInSequence<SingleTag>(AllTags{}),
                  "Component tag was not defined");
    constexpr ComponentId cId =
        utils::getTypeIndexFromSequence<SingleTag>(AllTags{});

    if (entityManager.exists(entity) &&
        !componentManager.hasComponent(entity, cId)) {
        entityQueryCache.purge(cId);
        return componentManager.addComponent<Component>(
            entity, std::forward<Args>(args)...);
    }
    return nullptr;
}

template <typename Component>
inline void EcsContainer::removeComponent(Entity const& entity) {
    using SingleTag = typename Component::single_tag;
    using AllTags = typename Component::all_tags;
    static_assert(std::is_base_of_v<MovableBase, Component>,
                  "Invalid type of obj");
    static_assert(utils::containsTypeInSequence<SingleTag>(AllTags{}),
                  "Component tag was not defined");
    constexpr ComponentId cId =
        utils::getTypeIndexFromSequence<SingleTag>(AllTags{});

    removeComponentImpl(entity, cId);
}

inline void EcsContainer::removeComponentImpl(Entity const& entity,
                                              ComponentId cId) {
    if (entityManager.exists(entity) &&
        componentManager.hasComponent(entity, cId)) {
        componentManager.removeComponent(entity, cId);
        entityQueryCache.purge(cId);
    }
}

template <typename Component>
inline auto EcsContainer::begin() {
    using SingleTag = typename Component::single_tag;
    using AllTags = typename Component::all_tags;
    static_assert(utils::containsTypeInSequence<SingleTag>(AllTags{}),
                  "Component tag was not defined");
    constexpr ComponentId cId =
        utils::getTypeIndexFromSequence<SingleTag>(AllTags{});

    return componentManager[cId].castBegin<Component>();
}

template <typename Component>
inline auto EcsContainer::end() {
    using SingleTag = typename Component::single_tag;
    using AllTags = typename Component::all_tags;
    static_assert(utils::containsTypeInSequence<SingleTag>(AllTags{}),
                  "Component tag was not defined");
    constexpr ComponentId cId =
        utils::getTypeIndexFromSequence<SingleTag>(AllTags{});

    return componentManager[cId].castEnd<Component>();
}

template <typename... Components>
inline EcsContainerBuffer<std::tuple<Components*...>> const&
EcsContainer::getEntitiesWithComponents() {
    using result_t = EcsContainerBuffer<std::tuple<Components*...>>;
    static_assert(
        (utils::containsTypeInSequence<typename Components::single_tag>(
             typename Components::all_tags()) &&
         ...),
        "Component was not defined during initialization");

    constexpr auto sig = [] {
        ComponentSig bitSig = 0;
        std::array<byte, 64> byteSig{};
        size_t i = 0;
        ((bitSig |=
          (1LL
           << utils::getTypeIndexFromSequence<typename Components::single_tag>(
                  typename Components::all_tags()))),
         ...);
        ((byteSig
              [utils::getTypeIndexFromSequence<typename Components::single_tag>(
                  typename Components::all_tags())] = ++i),
         ...);
        return std::pair(bitSig, byteSig);
    }();

    auto cacheResult = entityQueryCache.find(sig);
    size_t cacheIndex = 0;
    bool reuseIndex = false;
    if (cacheResult.second) {
        if (cacheResult.second->dirty) {
            static_cast<result_t*>(cacheResult.second->buffer)->clear();
            cacheResult.second->dirty = false;
            reuseIndex = true;
            cacheIndex = cacheResult.first;
        } else {
            auto* genericBuffer =
                static_cast<result_t*>(cacheResult.second->buffer);
            return *genericBuffer;
        }
    }
    if (!reuseIndex) {
        cacheIndex = entityQueryCache.createCache<Components*...>(sig);
    }
    auto const& metaData = componentManager.getMetaData();
    auto const& entities = entityManager.getEntitites();
    for (size_t i = 0; i < metaData.size(); ++i) {
        if ((metaData[i].bitSig & sig.first) == sig.first) {
            entityQueryCache.addTuple(
                cacheIndex,
                std::tuple(getComponent<Components>(entities[i])...));
        }
    }
    return *static_cast<result_t*>(entityQueryCache[cacheIndex].buffer);
}

inline void EcsContainer::removeEntity(Entity const& entity) {
    if (entityManager.exists(entity)) {
        for (ComponentId cId = 0; cId < componentTypesCount; ++cId) {
            removeComponentImpl(entity, cId);
        }
        entityManager.removeEntity(entity);
    }
}

template <typename Component>
inline Component* EcsContainer::getComponent(Entity const& entity) {
    using SingleTag = typename Component::single_tag;
    using AllTags = typename Component::all_tags;
    static_assert(std::is_base_of_v<MovableBase, Component>,
                  "Invalid type of obj");
    static_assert(utils::containsTypeInSequence<SingleTag>(AllTags{}),
                  "Component tag was not defined");
    constexpr ComponentId cId =
        utils::getTypeIndexFromSequence<SingleTag>(AllTags{});

    return componentManager.getComponent<Component>(entity, cId);
}

inline void EcsContainer::printEntityCount() const {
    for (int i = 0; i < componentTypesCount; ++i) {
        std::cout << "cID " << i
                  << ", component count: " << componentManager[i].size()
                  << '\n';
    }
}

inline size_t EcsContainer::getCurrentEntityCount() const {
    return entityManager.getCurrentEntityCount();
}
inline size_t EcsContainer::getMaximumEntityCount() const {
    return entityManager.getMaximumEntityCount();
}
inline bool EcsContainer::exists(Entity const& entity) const {
    return entityManager.exists(entity);
}

/*======================ComponentQueryCache========================================*/

template <typename TupleType>
inline void ComponentQueryCache::addTuple(size_t index,
                                          TupleType const& cTuple) {
    auto* genericBuffer =
        static_cast<EcsContainerBuffer<TupleType>*>(cache[index].buffer);
    genericBuffer->emplace_back(cTuple);
}
// returns index to the new cache
template <typename... PointersToComponents>
inline size_t ComponentQueryCache::createCache(sig_t const& sig) {
    QueryCacheEntry entry;
    entry.bitSig = sig.first;
    std::copy(sig.second.begin(), sig.second.end(), entry.byteSig.begin());
    entry.buffer =
        new EcsContainerBuffer<std::tuple<PointersToComponents...>>();
    cache.emplace_back(entry);
    return cache.size() - 1;
}

inline ComponentQueryCache::~ComponentQueryCache() {
    for (auto& entry : cache) {
        delete static_cast<EcsContainerBufferBase*>(entry.buffer);
    }
}

inline std::pair<size_t, QueryCacheEntry*> ComponentQueryCache::find(
    sig_t const& sig) {
    for (size_t i = 0; i < cache.size(); ++i) {
        if (sig.first == cache[i].bitSig) {
            if (sig.second == cache[i].byteSig) {
                return std::pair(i, &cache[i]);
            }
        }
    }
    return std::pair(0, nullptr);
}

inline void ComponentQueryCache::purge(ComponentId const& cId) {
    for (size_t i = 0; i < cache.size(); ++i) {
        if (cache[i].bitSig & (1ULL << cId)) {
            cache[i].dirty = true;
        }
    }
}

inline QueryCacheEntry& ComponentQueryCache::operator[](size_t i) {
    return cache[i];
}
}  // namespace ecs