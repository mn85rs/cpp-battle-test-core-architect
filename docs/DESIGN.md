# Задача

## Функциональные требования

Требуется создать пошаговую симуляцию битвы юнитов на карте для экспериментов с механиками.

Можно создать карту размером W×H клеток и разместить на ней N юнитов.
- Юниты могут быть разного типа с разными действиями и характеристиками.
- Каждый юнит может выполнить одно действие за ход.
    - Юниты действуют в порядке их создания.
- Одну клетку может занимать только один юнит.
- У всех юнитов есть уникальный идентификатор **Id** (целое число) 
- У большинства юнитов есть характеристика **HP** (Health Points).
    - Если **HP** ≤ 0, юнит исчезает на следующий ход и больше не действует.
- Некоторые юниты могут перемещаться:
    - За ход могут переместиться в любую из восьми соседних клеток.
    - Если не могут достичь цели за один ход, продолжают двигаться к ней в следующих ходах.

Симуляция заканчивается, когда нет юнитов, способных действовать в следующем ходу, или на карте остается только один юнит.

При запуске приложение получает путь к файлу со сценарием симуляции, где описаны команды для создания карты и юнитов, а также перемещения юнитов по карте. Приложение должно выводить все происходящие события в stdout.

Файл сценария создает первоначальное состояние симуляции. После никак не влияет на процесс симуляции

Базовый список юнитов, комманд и событий см. README.md

## Технические требования

- Простой, аккуратный код, по которому легко понять, как он работает.
- Четкие зоны ответственности каждой сущности в проекте.
- Архитектуру, позволяющую легко добавлять новые сущности и механики взаимодействия.
- Представьте, что после вас придут 10 джуниор-разработчиков, которые будут одновременно добавлять новые фичи.
- Разработчик уровня как минимум junior может взять это описание и реализовать по нему задачу

## Архитектура

### Объект сценария

- Главным компонентом, который инкапсулирует в себе всю логику симуляции, является объект сценария. Он реализует клиентский интерфейс **IScenario**, через который происходит вся работа с ним.
Метод **IScenario::run** запускает симуляцию. Возврат из метода происходит по окончании симуляции. Предполагается что сценарий не может выполняться вечно. В будущем, при неоходимости прерывания сценария, метод может быть расширен параметром StopToken, через который можно будет остановить симуляцию. Метод **IScenario::subscribeEvents** позволяет подписаться на события, генерируемые в ходе симуляции, например для логгирования. Остальные методы позволяют задать началное состояния сценария, с которого начнется симуляция.

     Базовая логика метода **IScenario::run** предполагает последовательный вызов метода **IUnit::takeTurn** с проверкой, что юнит не уничтожен (через метод **IUnit::isAlive**). Это делается на каждом шаге симуляции для всех юнитов в порядке спавна, с удалением всех уничтоженым юнитов в конце каждой итерации, до тех пока не останется только один выживший юнит или в ходе очередной итерации все живые юниты вернут **TurnStatus::NoActions** в качестве результата вызова **IUnit::takeTurn**, что говорит о том, что больше нет юнитов, способных действовать.

     Сценарий создается через вызов ф-ии **createScenario**.

    ```cpp
    struct IScenario
    {
        virtual void subscribeEvents(EventHandlers handlers) = 0;

        virtual void createMap(Width<Cells> width, Height<Cells> height) = 0;
        virtual void spawnUnit(const Coord<Cells>& cell, UnitFaсtory unitFactory) = 0;
        virtual void marchUnit(EntityId unitId, const Coord<Cells>& cell) = 0;

        virtual void run() = 0;

        virtual ~IScenario() = default;
    };

    using IScenarioPtr = std::unique_ptr<IScenario>;

    IScenarioPtr createScenario();
    ```

    Для добавления юнита в сценарий используется метод **IScenario::spawnUnit**, получающий на вход фабрику создания юнита **UnitFaсtory**, которая принимает на вход интерфейс **IUnitCreationContext**, позволяющий получить доступ ко всех подсистемам, которые нужны юниту.

    ```cpp
    struct IUnit;
    struct ICombatSystem;
    struct IMarchSystem;
    
    struct IUnitCreationContext
    {
        ICombatSystem& getCombatSystem();
        IMarchSystem& getMarchSystem();
        ...
    };

    using IUnitPtr = std::unique_ptr<IUnit>;

    using UnitFaсtory = std::function<IUnitPtr (IUnitCreationContext&)>;
    ```

    См. диаграмму *architecture_diagram_create_scenario.puml*


- Базовая реализация объекта сценария состоит из следующих компонентов:
    - Объект карты, реализующий интерфейс **IMap**
    - Объекты юнитов, реализующие интерфейс **IUnit**
    - Объекты систем взаимодействия юнитов, реализующие интерфейсы этих систем **ICombatSystem**, **IMarchSystem**

    См. диаграмму *architecture_diagram_scenario.puml*

    Далее рассмотрим все эти объекты и их интерфейсы подробнее.

### Карта

- Объект карты реализует интерфейс **IMap** и отвечает за хранение информации о расположении юнитов на карте, и позволяет получить позицию юнита по его индентификатору и наоборот, позволяет получить идентификатор юнита в конкретной ячейке, если он там есть. Позволяет передвинуть юнит в другое место. Позволяет получить все юниты в определенном радиусе.

    Ф-ия **IMap::subscribeUnitMovedEvent** позволяет подписаться на событие перемещения юнита на карте.

    Карта создается через вызов ф-ии **createMap**.

    ```cpp
    struct IMap
    {
        using UnitMovedHandler = std::function<void(EntityId, const Coord<Cells>&)>;

		virtual Coord<Cells> getUnitCell(EntityId unitId) const = 0;
		virtual std::set<EntityId> getUnitsInArea(const Coord<Cells>& center, Cells range) const = 0;
		virtual Width<Cells> width() const = 0;
		virtual Height<Cells> height() const = 0;
		virtual Coord<Cells> moveUnitToDestinationCell(
			EntityId unitId, const Coord<Cells>& destinationCell, Cells totalCellsToMove) = 0;
		virtual void addUnitOnMap(EntityId unitId, const Coord<Cells>& cell) = 0;
		virtual void removeUnitFromMap(EntityId unitId) = 0;

        virtual void subscribeUnitMovedEvent(UnitMovedHandler handler) = 0;

        virtual ~IMap() = default;
    };

    IMapPtr createMap();
    ```

- Вспомогательным классом карты является класс **Coord\<Cells\>**, который отвечает за хранение координат ячейки. 

    ```cpp
    using Cells = uint32_t;

    template <typename T>
    class Coord
    {
    public:
        T x() const;
        T y() const;
    };
    ```

### Подсистемы взаимодействия юнитов

Для уменьшения зависимости между юнитами и отделения ньюансов взаимодейсвия между ними от самих юнитов используется паттерн **Mediator**. Т.е. вся логика взаимодействия юнитов изолирована в отдельных подсистемах, каждая из которых отвечает за отдельный тип взаимодействия. Если юнит хочет оказать какое-то действие на другие юниты или взаимодействовать с миром, он делает это через интерфейс соответствующей подсистемы.

- #### **Подсистема боя**
    Данная подсистема состоит из следующих классов:

    - **Объект атаки**.

        Отвечает за все характеристики атаки. Состоит из типа атаки (ближняя/дальняя), количества урона, дистанции атаки и указания "массовости" урона. Последнее означает, что урон будет нанесен случайному противнику или всем в пределах радиуса атаки

        ```cpp
        enum class TargetCoverage
        {
            RandomTarget,
            AllTargets
        };

        enum class AttackType
        {
            CloseAttack,
            RangeAttack
        };

        using HitPoints = uint32_t;

        template <typename T>
        using AttackRange = T;

        template <typename T>
        using Damage = T;

        template <typename T>
        using DetectRange = T;

        struct Attack
        {
            AttackType type;
            Damage<HitPoints> damage;
            TargetCoverage targetCoverage;
            AttackRange<Cells> minRange;
            AttackRange<Cells> maxRange;
        };
        ```

    - **Подсистема**

        Компонент отвечает за взаимодействие юнитов в бою и реализует интерфейс **ICombatSystem**, через который юниты могут наносить урон другим юнитам и регистрироваться в качестве цели для получения урона.

        Базовая реализация подсистемы при получении запроса на нанесение урона каким-то юнитом (через метод **ICombatSystem::attackByUnit**), ищет юниты зарегистрированные в качестве цели (через метод **ICombatSystem::registerUnitAsTarget**) в пределах дистанции атаки, и наносит им урон через вызов колбэка **ICombatSystem::TakeDamageHandler**. Если в радиусе атаки нету целей, то считается что юнит промахнулся и возвращается **AttackResult::MissedAttack**.

        Предполагается что юнит сам решает, получил он урон или нет, умер или нет и т.д. и сам удаляет себя из списка целей через вызов **ICombatSystem::unRegisterUnitAsTarget**.

        Юниты могут регистрироваться в качестве цели независимо для ближнего и дальнего боя, что позволяет юнитам самим решать к какому типу урона они восприимчевы.

        Ф-ия **ICombatSystem::subscribeUnitAttackedEvent** позволяет подписаться на событие атаки одного юнита на другого.

        Подсистема создается через вызов ф-ии **createCombatSystem**.

        ```cpp
        struct ICombatSystem
        {
            using TargetStatusHandler = std::function<void(HitPoints)>;
            using TakeDamageHandler = std::function<void(AttackType, Damage<HitPoints>, TargetStatusHandler)>;
            using AttackTypes = std::unordered_set<AttackType>;

            using EventHandler = std::function<void(const Event<io::UnitAttacked>&)>;

            virtual AttackResult attackByUnit(EntityId attackerUnitId, const Attack& attack) = 0;
            virtual void registerUnitAsTarget(
                EntityId targetUnitId, AttackTypes types, TakeDamageHandler takeDamageHandler) = 0;
            virtual void unRegisterUnitAsTarget(EntityId targetUnitId, AttackTypes types) = 0;

            virtual void subscribeEvents(EventHandler handler) = 0;

            virtual ~ICombatSystem() = default;
        };

        ICombatSystemPtr createCombatSystem(const struct IMap& map);
        ```

    **Замечания по дизайну**
    - Данная архитектура позволяет изолировать добавление новых типов взаимодействия в ближнем бою, типа механики отбрасывания цели (когда мы меняет позицию цели) или атак не по радиусу, а по линии (если мы реализуем юнит копейщика) например. Выделение характеристик атаки в отдельный тип, позволяется юниту использовать разные типа атак в зависимости от выбранно умения например.

    - **Диаграммы**

        *architecture_diagram_combat_system.puml*
        
        *architecture_diagram_combat_system_sequence.puml*


- #### **Подсистема марширования**
    Данная подсистема предназначена для инкапсуляции алгоритма перемещения юнита (марширования) в заданную точку на карте (задаваемую через метод **IMarchSystem::setMarchDestinationForUnit**), т.к. юнит не знает своего расположения на карте и при маршировании в точку назначения полагается на эту систему. Доступ к этой системе имеют юниты, которые могут маршировать по карте. В базовой реализации, на каждом шаге симуляции, юнит который марширует, просит данную систему передвинуть его на количество клеток, задаваемых скоростью юнита, ближе к точке назначения используя метод **IMarchSystem::marchUnit**. Подсистема имеет доступ к объекту карты и делегирует передвижение юнита ей. При достижении точки назначения, подсистему говорит юниту что он на месте.

    Ф-ия **IMarchSystem::subscribeUnitMarchEvents** позволяет подписаться на события начала и конца марширования юнита.

    Подсистема создается через вызов ф-ии **createMarchSystem**.

    - **Подсистема**

        ```cpp
        template <typename T>
        using Speed = T;

        enum class MarchStatus
        {
            OnPlace,
            Marching
        };

        struct IMarchSystem
        {
            using EventHandler = std::function<void(const Event<io::MarchStarted, io::MarchEnded>&)>;

            virtual MarchStatus marchUnit(EntityId unitId, Speed<Cells> speed) = 0;
            virtual void setMarchDestinationForUnit(EntityId unitId, const Coord<Cells>& destination) = 0;

            virtual void subscribeEvents(EventHandler handler) = 0;

            virtual ~IMarchSystem() = default;
        };

        IMarchSystemPtr createMarchSystem(struct IMap& map);
        ```

    - **Диаграммы**

        *architecture_diagram_movement_system.puml*

        *architecture_diagram_movement_sequence.puml*

- #### **Юнит**
    Юнит это базовая единица симуляции, основной объект взаимодействия. Может быть воином, миной, магом, драконом и т.д.. Каждый юнит имеет уникальный id, по которому его можно идентифицировать.
    Объект сценария владеет всеми юнитами и в процессе симуляции, поочередно ходит каждым юнитом по очереди вызывая метод **IUnit::takeTurn**. В ходе очередной симуляционной итерации, на момент наступления очереди ходить, юнит может быть уничтожен, поэтому перед ходом, сценарий проверяет что юнит жив через метод **IUnit::isAlive**. Если юнит успешно сходил, т.е. выполнил какое-то действие, то метод **IUnit::takeTurn** возвращает **TurnStatus::Acted**. Если ни один из оставшихся в живых юнитов не может выполнить ход, симуляция прекращается. 

    ```cpp
    using EntityId = uint32_t;

    enum class TurnStatus
    {
        NoActions,
        Acted
    };

    struct IUnit
    {
		using EventHandler = std::function<void(const Event<io::UnitDied>&)>;

		virtual EntityId id() const = 0;
		virtual const char* type() const = 0;
		virtual bool isAlive() const = 0;
		virtual TurnStatus takeTurn() = 0;

		virtual void subscribeEvents(EventHandler handler) = 0;

        virtual ~IUnit() = default;
    };
    ```

    Какие действия совершать во время хода и состояние жив\мертв определяется конкретным подклассом юнита. Типичное условие жизни юнита это запас здоровья выше нуля. Расширенные механики реакции на урон могут включать различные модификаторы типа бессмертия, дополнительного щита, отравления и т.д.

    Ф-ия **IUnit::subscribeUnitDestroyedEvent** позволяет подписаться на событие уничтожения юнита.

    - **Классы юнитов**

        В качестве примера юнита рассмотрим класс Hunter. Данный класс может являться целью как для ближней, так и для дальней атаки, может атаковать как в ближнем, так и в дальнем бою. Может маршировать.

        ```cpp
        class Unit : public IUnit
        {
        public:
            Unit(EntityId id);

            virtual ~Unit() = default;

            EntityId id() const override;
            void subscribeEvents(EventHandler eventHandler) override;

        protected:
            void destroy();

            ...
        };

        class Hunter : public Unit
        {
        public:
            Hunter(
                EntityId id,
                HitPoints maxHP,
                Strength strenght,
                Agility agility,
                Range range,
                ICombatSystem& combatSystem,
                IMarchSystem& marchSystem
            )
            ...
            {
                registerAsTarget();
            }

            const char* type() const override;

            bool isAlive() const override
            {
                return _health.HP() > 0;
            }

            TurnStatus takeTurn() override
            {
                if (AttackResult::SuccessfulAttack == attack(getCloseAttack()))
                {
                    return TurnStatus::Acted;
                }

                if (AttackResult::SuccessfulAttack == attack(getRangeAttack()))
                {
                    return TurnStatus::Acted;
                }

                if (MarchStatus::Marching == march())
                {
                    return TurnStatus::Acted;
                }

                return TurnStatus::NoActions;
            }
        private:
            void registerAsTarget()
            {
                _combatSystem.registerUnitAsTarget(
                    id,
                    {AttackType::CloseAttack, AttackType::RangeAttack},
                    [this](auto&&... args) { return takeDamage(std::forward<decltype(args)>(args)...); });
            }

            void unRegisterAsTarget()
            {
                _combatSystem.unRegisterUnitAsTarget(id(), {AttackType::CloseAttack, AttackType::RangeAttack});
            }

        	AttackResult attack(const Attack& attack)
            {
                return _combatSystem.attackByUnit(id(), attack);
            }

            Attack getCloseAttack() const
            {
                return {AttackType::CloseAttack, _strenght.value(), TargetCoverage::RandomTarget, 1, 1};
            }

            Attack getRangeAttack() const
            {
                return {AttackType::CloseAttack, _agility.value(), TargetCoverage::RandomTarget, 2, _range.value()};
            }

            MarchStatus march()
            {
                return _movementSystem.marchUnit(id(), _speed);
            }

            void takeDamage(Damage<HitPoints> damage, ICombatSystem::TargetStatusHandler targetStatusHandler)
            {
                _health.decrease(damage);

                targetStatusHandler(_health.HP());

                if (0 == _health.HP())
                {
                    destroy();
                }
            }

            void destroy()
            {
                unRegisterAsTarget();
                Unit::destroy();
            }

        private:
            Strength _strenght;
            Agility _agility;
            Range _range;
            Health _health;
            Speed<Cells> _speed;
            ICombatSystem& _combatSystem;
            IMarchSystem& _movementSystem;
        };
        ```

    - **Характеристики юнитов**

        Для характеристик юнита используются отдельный набор классов. Для большинства характеристик достаточно использовать псевдоним типа, т.к. в базовом варианте это просто число. Но для избежания ошибочного использования одной характеристики вместо другой, сделан специальный класс хелпер **Ability**, который защищает от этого, генерируя разные типы для каждой характеристики.

        ```cpp
        template <typename T, typename Tag>
        class Ability
        {
        public:
            Ability(T value);
            T value() const;

        private:
            T _value;
        };

        class StrengthTag;
        using Strength = Ability<uint32_t, StrengthTag>;

        class AgilityTag;
        using Agility = Ability<uint32_t, AgilityTag>;
        ```

        Отдельным классом сделана характеристика здоровья **Health**, которая ограничивает увеличение/уменьшение HP в рамках [0, maxHP]

        ```cpp
        class Health
        {
        public:
            Health(HitPoints maxHP);

            HitPoints HP() const;

            void decrease(HitPoints hp);
            void increase(HitPoints hp);

        private:
            HitPoints _maxHP;
            HitPoints _HP;
        };
        ```

- #### Пример комплексного взаимодействия систем

    *architecture_diagram_complex_sequence.puml*

- #### Замечания по расширению функционала
    Архитектура спроектирована так, чтобы упростить добавление новых характеристик, юнитов и механик. Например, рассмотрим процесс добавления ядовитой мины.
    Используем новый метод **ICombatSystem::anyUnitsInRangeAroundUnit** который детектит юниты вокруг, и если есть юнит рядом, то триггерим мину, которая наносит ядовитый урон. Для урона ядом нужно добавить в класс атаки параметр типа урона (физический, яд и т.д.). Далее, создать новый класс мины, который будет на каждом ходу проверять есть ли рядом юниты, и если есть, то взрываться и наносить ядовитый урон всем вокруг. Юниты же пораженные ядом, будут на каждом ходу уменьшать здоровье каждый ход и по истечении здоровья умирать. Мина не может быть атакована и не может перемещаться, поэтому не регистрируется в качестве цели и не имеет доступа к подсистеме марширования.

    ```cpp
    /*new enum*/
    enum class DamageType
    {
        Physical,
        Poison 
    };

    struct Attack
    {
        AttackType type;
        Damage<HitPoints> damage;
        DamageType damageType;/*new member*/
        TargetCoverage targetCoverage;
        AttackRange<Cells> minRange;
        AttackRange<Cells> maxRange;
    };

    /*new type*/
    template <typename T>
    using DetectRange = T;

    /*new ability*/
    class PowerTag;
    using Power = Ability<uint32_t, PowerTag>;

    struct ICombatSystem
    {
        using TakeDamageHandler = std::function<HitPoints (Damage<HitPoints>, DamageType/*new parameter*/, TargetStatusHandler)>;
        ...
    };

    class PoisonMine : public Unit
    {
    public:
        PoisonMine(
            EntityId id,
            Power power,
            ICombatSystem& combatSystem
        );

    protected:
        TurnStatus takeTurn() override
        {
            if (_isExploded)
            {
                return TurnStatus::NoActions;
            }

            if (unitDetected())
            {
                _combatSystem.attackByUnit(id(), explosionAttack());
                Exploded();
            }

            return TurnStatus::Acted;
        }

        Attack explosionAttack() const
        {
            return {AttackType::CloseAttack, _power.value(), DamageType::Poison, TargetCoverage::RandomTarget, 1, 3};
        }

        bool unitDetected()
        {
            _combatSystem.anyUnitsInRangeAroundUnit(id(), 2);
        }

        bool isAlive() const override { return !_isExploded; }

    private:
        void Exploded()
        {
            _isExploded = true;
            Unit::destroy();
        }

    private:
    ...
        bool _isExploded;
        ICombatSystem& _combatSystem;
        Power _power;
    };
    ```
    
- #### Обработка ошибок
    Дизайн по умолчанию использует исключения, хоть документ и не описывает конкретные типы исключений выбрасываемые компонентами. 