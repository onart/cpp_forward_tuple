namespace onart
{
    template<size_t idx, class... T> struct __prsf;

    template<size_t idx> struct __prsf<idx> { using itype = void; };

    template<class F, class... T> struct __prsf<0, F, T...> { using itype = F; };

    template<size_t idx, class F, class... T> struct __prsf<idx, F, T...> { 
        static_assert(idx <= sizeof...(T));
        using itype = __prsf<idx - 1, T...>::template itype;
    };

    template<size_t idx, class... T>
    using variadicType = __prsf<idx, T...>::template itype;

    template<class... T> using firstType = variadicType<0, T...>;

    template<class... T> using lastType = variadicType<sizeof...(T) - 1, T...>;

    constexpr inline size_t rounded2multiple(size_t m1, size_t m2) { return (m1 % m2) ? (m1 / m2 + 1) * m2 : m1; }
    constexpr inline size_t constmax(size_t a, size_t b) { return a > b ? a : b; }

    namespace __private {
        template<size_t pack, size_t idx, size_t curoffset, size_t maxAlign, class F, class... T>
        constexpr inline size_t packedPartialOffset() {
            static_assert(maxAlign <= pack);
            static_assert(pack > 0 && (pack & (pack - 1)) == 0);
            static_assert(idx <= sizeof...(T));
            if constexpr (idx == 0) {
                return curoffset;
            }
            else {
                constexpr size_t V_SIZE = sizeof(F);
                constexpr size_t V_ALIGN = alignof(F);
                constexpr size_t begin = rounded2multiple(curoffset, V_ALIGN);
                if constexpr (begin / pack != (begin + V_SIZE) / pack) {
                    return packedPartialOffset<pack, idx - 1, rounded2multiple(curoffset, pack) + V_SIZE, constmax(maxAlign, V_ALIGN), T...>();
                }
                else {
                    return packedPartialOffset<pack, idx - 1, begin + V_SIZE, constmax(maxAlign, V_ALIGN), T...>();
                }
            }
        }

        template<size_t pack, class F, class... T>
        constexpr inline size_t packedSizeFunc() {
            static_assert(pack > 0 && (pack & (pack - 1)) == 0);

            constexpr size_t finalOff = __private::packedPartialOffset<pack, sizeof...(T), 0, 1, F, T...>();
            constexpr size_t V_SIZE = sizeof(lastType<T...>);
            constexpr size_t V_ALIGN = alignof(lastType<T...>);
            constexpr size_t begin = rounded2multiple(finalOff, V_ALIGN);
            if constexpr (begin / pack != (begin + V_SIZE) / pack) {
                constexpr size_t begin2 = rounded2multiple(finalOff, pack);
                constexpr size_t end2 = begin2 + V_SIZE;
                return rounded2multiple(end2, pack);
            }
            else {
                constexpr size_t end2 = begin + V_SIZE;
                return rounded2multiple(end2, pack);
            }
        }
    }

    template<size_t pack, size_t idx, class... T> constexpr inline size_t packedOffset = __private::packedPartialOffset<pack, idx, 0, 1, T...>();
    template<size_t pack, class... T> constexpr inline size_t packedSize = __private::packedSizeFunc<pack, T...>();

    template<size_t pack, class... T>
    struct alignas(pack) packedstruct;

    template<size_t pack>
    struct alignas(pack) packedstruct<pack> {};

    template<size_t pack, class T>
    struct alignas(pack) packedstruct<pack, T> {
        T attr;
        template<size_t idx>
        T& get() {
            static_assert(idx == 0);
            return attr;
        }

        template<size_t idx>
        const T& get() const {
            static_assert(idx == 0);
            return attr;
        }
    };

    template<size_t pack, class F, class... T>
    struct alignas(pack) packedstruct<pack, F, T...> {
    private:
        template<size_t idx = 0>
        inline static void init(packedstruct& obj, const F& f, const T&... t) {
            new (&obj.get<idx>()) F(f);
            if constexpr (idx < sizeof...(T)) {
                init<idx + 1>(obj, f, t...);
            }
        }
        
        template<size_t idx = 0>
        inline static void destruct(packedstruct& obj) {
            using type = decltype(obj.getcopy<idx>());
            obj.get<idx>().~type();
        }
    public:
        inline packedstruct() = default;
        inline packedstruct(const F& f, const T&... t) { init(*this, f, t...); }

        inline ~packedstruct() { destruct(*this); }

        template<size_t idx>
        auto& get() {
            static_assert(idx <= sizeof...(T));
            using typ = variadicType<idx, F, T...>;
            constexpr size_t off = packedOffset<pack, idx, F, T...>;
            return reinterpret_cast<typ&>(attrs[off]);
        }

        template<size_t idx>
        const auto& get() const {
            static_assert(idx <= sizeof...(T));
            using typ = variadicType<idx, F, T...>;
            constexpr size_t off = packedOffset<pack, idx, F, T...>;
            return reinterpret_cast<const typ&>(attrs[off]);
        }

        template<size_t idx>
        auto getcopy() {
            static_assert(idx <= sizeof...(T));
            using typ = variadicType<idx, F, T...>;
            constexpr size_t off = packedOffset<pack, idx, F, T...>;
            return *reinterpret_cast<typ*>(&attrs[off]);
        }
    private:
        uint8_t attrs[packedSize<pack, F, T...>];
    };
}
