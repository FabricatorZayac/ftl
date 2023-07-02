#ifndef RESULT_H_
#define RESULT_H_

#include <cstddef>
#include <cstring>
#include <iostream>

namespace ftl {
    [[noreturn]] inline void panic(const char *error) {
        std::cerr << error << std::endl;
        exit(1);
    }
    struct str {
        using value_type = const char;
        using iterator = const char *;
        using reverse_iterator = const char *;
        using size_type = size_t;

        constexpr str(const char *begin, size_type size) :
            begin_iter(begin),
            end_iter(begin + size) {}
        constexpr str(const char *begin) :
            begin_iter(begin),
            end_iter(begin + std::char_traits<char>::length(begin)) {}

        constexpr iterator cbegin() { return this->begin_iter; }
        constexpr iterator cend() { return this->end_iter; }

        constexpr reverse_iterator crbegin() { return this->begin_iter; }
        constexpr reverse_iterator crend() { return this->end_iter; }

        constexpr size_type size() const { return end_iter - begin_iter; }

        constexpr value_type &operator[](size_type idx) const {
            return begin_iter[idx];
        }

        bool operator==(const str &other) const {
            return size() == other.size()
               and !memcmp(begin_iter, other.begin_iter, size());
        }
        bool operator==(const char *other) const {
            return other[size()] == '\0'
               and !memcmp(begin_iter, other, size());
        }

        friend std::ostream &operator<<(std::ostream &out, const str &self) {
            return out.write(self.begin_iter, self.size());
        }
    private:
        iterator begin_iter;
        iterator end_iter;
    };

    template<typename T = void, typename E = void>
    struct Result;
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
        friend std::ostream &operator<<(std::ostream &out, const Tag &self)  {
            switch (self) {
            case Tag::Ok:
                return out << "Ok";
            case Tag::Err:
                return out << "Err";
            }
        }
    };

    template<typename T>
    struct Result<T, void> : Result<> {
        constexpr static Tag tag = Tag::Ok;
        T ok;
    };

    template<typename E>
    struct
    [[nodiscard("`Result` may be an `Err` variant, which should be handled")]]
    Result<void, E> : Result<> {
        constexpr Result(Result<> &&temp) : Result<>{ temp } {}
        ~Result() {
            if (is_err()) err.~E();
        }
        friend Result Err<E>(E err) {
            Result temp(Result<>{ Result<>::Tag::Err });
            temp.err = err;
            return temp;
        }
        void unwrap() {
            switch (tag) {
            case Tag::Ok:
                break;
            case Tag::Err:
                panic("Tried to unwrap an error, baka");
            }
        }
        E unwrap_err() const {
            switch (tag) {
            case Tag::Ok:
                panic("Not an error, you fucking idiot");
            case Tag::Err:
                return err;
            }
        }

        template<typename F>
        auto map(F &&op) -> Result<decltype(op()), E> {
            if (is_ok()) return Ok(op());
            else return Err(err);
        }
        template<typename F>
        auto map_err(F &&op) -> Result<void, decltype(op(std::declval<E>()))> {
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

    template<typename T, typename E>
    struct
    [[nodiscard("`Result` may be an `Err` variant, which should be handled")]]
    Result : Result<> {
        constexpr Result(Result<T, void> &&temp) :
            Result<>{ temp.tag },
            ok(temp.ok) { }
        constexpr Result(Result<void, E> &&temp) :
            Result<>{ temp.tag },
            err(temp.unwrap_err()) { }
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
                panic("Tried to unwrap an Error, baka");
            }
        }
        E unwrap_err() const {
            switch (tag) {
            case Tag::Ok:
                panic("Not an error, you fucking idiot");
            case Tag::Err:
                return err;
            }
        }

        template<typename F>
        auto map(F &&op) -> Result<decltype(op(std::declval<T>())), E> {
            if (is_ok()) return Ok(op(ok));
            else return Err(err);
        }
        template<typename F>
        auto map_err(F &&op) -> Result<T, decltype(op(std::declval<E>()))> {
            if (is_ok()) return Ok(ok);
            else return Err(op(err));
        }

        bool operator==(const Result<void, E> &other) const {
            return (is_ok() && other.is_ok())
                or (is_err() && other.is_err() && err == other.unwrap_err());
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

    template<typename T>
    constexpr Result<T, void> Ok(T ok)  {
        return Result<T, void> { .ok = ok };
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
        ~Option() {
            if (is_some()) some.~T();
        }

        friend constexpr Option Some<T>(T some)  {
            Option temp(Option<> { Option<>::Tag::Some });
            new (&temp.some) T(some);
            return temp;
        }
        T &unwrap() {
            if (is_some()) return some;
            panic("Tried to unwrap a None option");
        }

        template<typename F>
        auto map(F &&op) -> Option<decltype(op(std::declval<T>()))> {
            if (is_some()) return Some(op(some));
            return None();
        }
        template<typename F>
        auto ok_or_else(F err) -> Result<T, decltype(err())> {
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
    private:
        union {
            T some;
        };
    };

    template<typename T>
    struct Option<T &> : Option<std::reference_wrapper<T>> {
        constexpr Option(Option<std::reference_wrapper<T>> &&temp)
            : Option<std::reference_wrapper<T>>(std::move(temp)) {}
        constexpr Option(Option<> &&temp)
            : Option<std::reference_wrapper<T>>(std::move(temp)) {}
    };

    constexpr Option<> None()  {
        return Option<> { Option<>::Tag::None };
    }
}

#define TRY(EXPR)                                                \
({                                                               \
    auto ___temp = EXPR;                                         \
    if (___temp.is_err()) return ftl::Err(___temp.unwrap_err()); \
    ___temp.unwrap();                                            \
})

#endif // !RESULT_H_
