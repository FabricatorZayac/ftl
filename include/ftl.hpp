#ifndef FTL_H_
#define FTL_H_

#include <cstddef>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>

namespace ftl {
    template<typename T = void, typename E = void>
    struct
    [[nodiscard("`Result` may be an `Err` variant, which should be handled")]]
    Result;

    template<typename T = void>
    struct Option;

    template<typename T>
    constexpr Option<T> Some(T);
    constexpr Option<> None();

    constexpr Result<void, void> Ok();
    template<typename T>
    constexpr Result<T, void> Ok(T);
    template<typename E>
    constexpr Result<void, E> Err(E);

    [[noreturn]] inline void panic(const char *error) noexcept {
        std::cerr << error << std::endl;
        exit(EXIT_FAILURE);
    }

    template<typename T>
    struct PhantomData {
        bool operator==(const PhantomData &) { return true; }
    };

    template<>
    struct Result<> {
        enum class Tag {
            Ok,
            Err,
        } tag;
        bool is_ok() const {
            return tag == Tag::Ok;
        }
        bool is_err() const {
            return tag == Tag::Err;
        }
        friend std::ostream &operator<<(std::ostream &out, const Tag &self) {
            switch (self) {
            case Tag::Ok:
                return out << "Ok";
            case Tag::Err:
                return out << "Err";
            }
        }
    };

    // Infallible
    template<typename T>
    struct Result<T, void> : Result<> {
        Result(T ok) : Result<> { Tag::Ok }, ok(ok) { }

        auto map(auto op) -> Result<decltype(op(std::declval<T>())), void> {
            return Ok(op(ok));
        }
        T unwrap() {
            return ok;
        }

        T ok;
    };

    template<typename E>
    struct Result<void, E> : Result<> {
        constexpr Result(Result<> &&temp) :
            Result<>{ temp } {}
        constexpr Result(const Result &other) :
            Result<>{ other.tag } {
            if (is_err()) new (&err) E(other.err);
        }
        ~Result() {
            if (is_err()) err.~E();
        }
        friend constexpr Result Err<E>(E err);

        void unwrap() {
            if (is_err()) panic("Tried to unwrap an `Err` variant");
        }
        E unwrap_err() const {
            switch (tag) {
            case Tag::Ok:
                panic("Tried to unwrap_err on `Ok` variant");
            case Tag::Err:
                return err;
            }
        }
        bool is_ok_and(const std::function<bool()> &f) const {
            return is_ok() && f();
        }
        bool is_err_and(const std::function<bool(const E &)> &f) const {
            return is_err() && f(err);
        }

        auto map(auto op) -> Result<decltype(op()), E> {
            if (is_ok()) return Ok(op());
            else return Err(err);
        }
        auto map_err(auto op) -> Result<void, decltype(op(std::declval<E>()))> {
            if (is_ok()) return Ok();
            else return Err(op(err));
        }

        bool operator==(const Result<> &other) const {
            return is_ok() && other.is_ok();
        }
        bool operator==(const Result<void, E> &other) const {
            return (is_ok() && other.is_ok())
                or (is_err() && other.is_err() && err == other.err);
        }

        friend std::ostream &operator<<(std::ostream &out, Result self) {
            return out << self.tag << '(' << self.err << ')';
        }
    private:
        union {
            E err;
        };
        Result(E err) :
            Result<> { Tag::Err },
            err(err) {}
    };
    template<typename E>
    constexpr Result<void, E> Err(E err) {
        Result<void, E> temp(Result<>{ Result<>::Tag::Err });
        new (&temp.err) E(err);
        return temp;
    }

    template<typename T, typename E>
    struct Result : Result<> {
        constexpr Result(Result<T, void> &&temp) :
            Result<>{ temp.tag },
            ok(temp.ok) { }
        constexpr Result(Result<void, E> &&temp) :
            Result<>{ temp.tag },
            err(temp.unwrap_err()) { }
        constexpr Result(const Result &other) :
            Result<> {other.tag} {
            switch (tag) {
            case Tag::Ok:
                new (&ok) T(other.ok);
                break;
            case Tag::Err:
                new (&err) E(other.err);
                break;
            }
        }
        ~Result() {
            switch (tag) {
            case Tag::Ok:
                ok.~T();
                break;
            case Tag::Err:
                err.~E();
                break;
            }
        }
        T unwrap() {
            switch (tag) {
            case Tag::Ok:
                return ok;
            case Tag::Err:
                panic("Tried to unwrap an `Err` variant");
            }
        }
        E unwrap_err() {
            switch (tag) {
            case Tag::Ok:
                panic("Tried to unwrap_err on an `Ok` variant");
            case Tag::Err:
                return err;
            }
        }
        bool is_ok_and(const std::function<bool(const T&)> &f) const {
            return is_ok() && f(ok);
        }
        bool is_err_and(const std::function<bool(const E&)> &f) const {
            return is_err() && f(err);
        }

        auto map(auto op) -> Result<decltype(op(std::declval<T>())), E> {
            if (is_ok()) return Ok(op(ok));
            else return Err(err);
        }
        auto map_err(auto op) -> Result<T, decltype(op(std::declval<E>()))> {
            if (is_ok()) return Ok(ok);
            else return Err(op(err));
        }

        bool operator==(const Result<void, E> &other) const {
            return (is_ok() && other.is_ok())
                or (is_err() && other.is_err_and([&](const E &a){ return err == a; }));
        }
        bool operator==(const Result<T, void> &other) const {
            return is_ok() && ok == other.ok;
        }
        bool operator==(const Result<T, E> &other) const {
            return (is_ok() && other.is_ok() && ok == other.ok)
                or (is_err() && other.is_err() && err == other.err);
        }

        friend std::ostream &operator<<(std::ostream &out, Result self) {
            out << self.tag << '(';
            switch (self.tag) {
            case Tag::Ok:
                return out << self.ok << ')';
            case Tag::Err:
                return out << self.err << ')';
            }
        }
    private:
        union {
            T ok;
            E err;
        };
    };

    template<typename T, typename E>
    struct Result<T &, E> : Result<std::reference_wrapper<T>, E> {
        constexpr Result(Result<std::reference_wrapper<T>, void> &&temp) :
            Result<std::reference_wrapper<T>, E>(std::move(temp)) {}
        constexpr Result(Result<void, E> &&temp) :
            Result<std::reference_wrapper<T>, E>(std::move(temp)) {}
    };

    template<typename T>
    constexpr Result<T, void> Ok(T ok)  {
        return Result<T, void>(ok);
    }
    constexpr Result<> Ok()  {
        return Result<> { Result<>::Tag::Ok };
    }

    template<>
    struct Option<> {
        enum class Tag {
            Some,
            None,
        } tag;
        bool is_some() const {
            return tag == Tag::Some;
        }
        bool is_none() const {
            return tag == Tag::None;
        }
        friend std::ostream &operator<<(std::ostream &out, const Tag &self) {
            switch (self) {
            case Tag::Some:
                return out << "Some";
            case Tag::None:
                return out << "None";
            }
        }
    };
    template<typename T>
    struct Option : Option<> {
        constexpr Option(Option<> &&temp) :
            Option<> { temp } {}
        constexpr Option(const Option &other) :
            Option<> { other.tag } {
                if (is_some()) some = other.some;
            }
        ~Option() {
            if (is_some()) some.~T();
        }
        friend constexpr Option Some<T>(T);

        T &unwrap() {
            if (is_some()) return some;
            panic("Tried to unwrap a None option");
        }
        bool is_some_and(std::function<bool(const T&)> f) {
            return is_some() && f(some);
        }

        auto map(auto op) -> Option<decltype(op(std::declval<T>()))> {
            if (is_some()) return Some(op(some));
            return None();
        }
        auto ok_or_else(auto err) -> Result<T, decltype(err())> {
            if (is_some()) return Ok(some);
            return Err(err());
        }
        Option<const T &> as_ref() {
            if (is_some()) return Some(std::cref(some));
            return None();
        }
        Option<T &> as_mut() {
            if (is_some()) return Some(std::ref(some));
            return None();
        }

        bool operator==(const Option<> &other) const {
            return is_none() && other.is_none();
        }
        bool operator==(const Option &other) const {
            return (is_none() && other.is_none())
                or (is_some() && other.is_some() && some == other.some);
        }

        friend std::ostream &operator<<(std::ostream &out, Option &&self) {
            out << self.tag;
            if (self.is_some()) {
                out << '(' << self.unwrap() << ')';
            }
            return out;
        }
    protected:
        union {
            T some;
        };
    };

    template<typename T>
    struct Option<T &> : Option<std::reference_wrapper<T>> {
        constexpr Option(Option<std::reference_wrapper<T>> &&temp) :
            Option<std::reference_wrapper<T>>(std::move(temp)) {}
        constexpr Option(Option<> &&temp) :
            Option<std::reference_wrapper<T>>(std::move(temp)) {}

        friend constexpr Option Some<T &>(T &);
    };

    template<typename T>
    constexpr Option<T> Some(T some)  {
        Option<T> temp(Option<> { Option<>::Tag::Some });
        new (&temp.some) T(some);
        return temp;
    }
    constexpr Option<> None()  {
        return Option<> { Option<>::Tag::None };
    }

    // TODO: add a bunch of concept checks like Ord and stuff
    template<typename Idx>
    struct Range {
        struct iterator {
            constexpr iterator(Idx index) : index(index) {}

            constexpr Idx operator*() { return index; }
            constexpr bool operator==(const iterator &other) const {
                return index == other.index;
            }
            constexpr iterator operator++() {
                return ++index;
            }
            constexpr operator Idx() {
                return index;
            }
        private:
            Idx index;
        };

        constexpr Range(Idx start_idx, Idx end_idx) :
            start_idx(start_idx),
            end_idx(end_idx) {}

        constexpr iterator begin() const { return start_idx; }
        constexpr iterator end() const { return end_idx; }

        constexpr Idx count() const { return end_idx - start_idx; }
    private:
        Idx start_idx;
        Idx end_idx;
    };

    Range(const int, const int) -> Range<size_t>;

    template<typename T>
    struct Slice {
        using value_type = T;

        using iterator = T *;
        using const_iterator = const T *;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        using size_type = size_t;

        constexpr Slice(std::initializer_list<T> values) :
            data(values.begin()),
            length(values.size()) {}
        constexpr Slice(T *data, size_type length) :
            data(data),
            length(length) {}
        template<size_type N>
        constexpr Slice(const T (&data)[N]) :
            data(data),
            length(N) {}

        constexpr iterator begin() { return data; }
        constexpr const_iterator begin() const { return data; }
        constexpr const_iterator cbegin() const { return data; }

        constexpr iterator end() { return data + length; }
        constexpr const_iterator end() const { return data + length; }
        constexpr const_iterator cend() const { return data + length; }

        constexpr reverse_iterator rbegin() { return data; }
        constexpr const_reverse_iterator rbegin() const { return data; }
        constexpr const_reverse_iterator crbegin() const { return data + length; }

        constexpr reverse_iterator rend() { return data; }
        constexpr const_reverse_iterator rend() const { return data; }
        constexpr const_reverse_iterator crend() const { return data + length; }

        value_type *as_ptr() { return data; }
        constexpr size_type len() const { return length; }

        constexpr Option<const value_type &> get(size_type idx) const {
            if (idx >= length) return None();
            return Some<const value_type &>(data[idx]);
        }
        constexpr Option<value_type &> get_mut(size_type idx) {
            if (idx >= length) return None();
            return Some(data[idx]);
        }
        constexpr value_type &operator[](size_type idx) const {
            return data[idx];
        }
        constexpr Slice operator[](const Range<size_type> &range) const {
            return {data + range.begin(), range.count()};
        }

        template<typename Tu>
        bool operator==(const Slice<Tu> &other) const {
            return len() == other.len()
               and !memcmp(cbegin(), other.cbegin(), len());
        }
        template<size_type N>
        bool operator==(const T (&other)[N]) const {
            return len() == N
               and !memcmp(cbegin(), other, N);
        }
    private:
        value_type *data;
        size_type length;
    };

    template<typename T>
    Slice(std::initializer_list<T>) -> Slice<const T>;

    struct str : Slice<const char> {
        constexpr str(const char *data, size_type length) :
            Slice<const char>(data, length) {}
        template<size_type N>
        constexpr str(const char (&data)[N]) :
            Slice<const char>(data, N - 1) {}

        bool operator==(const str &other) const {
            return len() == other.len()
               and !memcmp(cbegin(), other.cbegin(), len());
        }
        template<size_type N>
        bool operator==(const char (&other)[N]) const {
            return other[len()] == '\0'
               and !memcmp(cbegin(), other, N);
        }
        friend std::ostream &operator<<(std::ostream &out, const str &self) {
            return out.write(self.cbegin(), self.len());
        }
        friend std::string &operator+=(std::string &string, const str &self) {
            return string.append(self.cbegin(), self.len());
        }
    };
}

#define TRY(EXPR)                                                \
({                                                               \
    auto ___temp = EXPR;                                         \
    if (___temp.is_err()) return ftl::Err(___temp.unwrap_err()); \
    ___temp.unwrap();                                            \
})

#endif // !FTL_H_
