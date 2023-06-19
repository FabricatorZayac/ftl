#ifndef RESULT_H_
#define RESULT_H_

#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include <utility>

#define PANIC(ERROR)                 \
    std::cerr << ERROR << std::endl; \
    exit(1);

namespace ftl {
    template<typename E>
    concept Error = requires(E err) {
        err.descrition();
    };

    struct str {
        using value_type = const char;
        using iterator = const char *;
        using reverse_iterator = const char *;
        using size_type = size_t;

        constexpr str() {}
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

        constexpr size_type size() { return end_iter - begin_iter; }

        constexpr value_type &operator[](size_type idx) {
            return begin_iter[idx];
        }
        bool operator==(str &other) noexcept {
            for (auto &[i, j] : { *this, other }) {
                if (*i != *j) return false;
            }
            return true;
        }
        friend std::ostream &operator<<(std::ostream &out, str self) {
            return out.write(self.begin_iter, self.end_iter - self.begin_iter);
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
    constexpr Option<T> Some(T) noexcept;
    constexpr Option<> None() noexcept;

    constexpr Result<void, void> Ok() noexcept;
    template<typename T>
    constexpr Result<T, void> Ok(T) noexcept;
    template<typename E>
    constexpr Result<void, E> Err(E) noexcept;

    template<>
    struct Result<> {
        enum class Tag {
            Ok,
            Err,
        } tag;
        friend std::ostream &operator<<(std::ostream &out, Tag self) noexcept {
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
        friend Result Err<E>(E err) noexcept {
            Result temp(Result<>{ Result<>::Tag::Err });
            temp.err = err;
            return temp;
        }
        void unwrap() {
            switch (tag) {
            case Tag::Ok:
                break;
            case Tag::Err:
                PANIC("Tried to unwrap an error, baka");
            }
        }
        E unwrap_err() {
            switch (tag) {
            case Tag::Ok:
                PANIC("Not an error, you fucking idiot");
            case Tag::Err:
                return err;
            }
        }
        bool is_ok() {
            return tag == Tag::Ok;
        }
        bool is_err() {
            return tag == Tag::Err;
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

        friend std::ostream &operator<<(std::ostream &out, Result self) {
            return out << self.tag << '(' << self.err << ')';
        }
    private:
        union {
            E err;
        };
        Result(E err) :
            Result<void, void> { Tag::Err },
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
                PANIC("Tried to unwrap an Error, baka");
            }
        }
        E unwrap_err() {
            switch (tag) {
            case Tag::Ok:
                PANIC("Not an error, you fucking idiot");
            case Tag::Err:
                return err;
            }
        }
        bool is_ok() {
            return tag == Tag::Ok;
        }
        bool is_err() {
            return tag == Tag::Err;
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

        friend std::ostream &operator<<(std::ostream &out, Result self) {
            out << self.tag << '(';
            switch (self.tag) {
            case decltype(self.tag)::Ok:
                return out << self.ok << ')';
            case decltype(self.tag)::Err:
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
    constexpr Result<T, void> Ok(T ok) noexcept {
        return Result<T, void> { .ok = ok };
    }
    constexpr Result<void, void> Ok() noexcept {
        return Result<> { Result<>::Tag::Ok };
    }

    template<>
    struct Option<> {
        enum class Tag {
            Some,
            None,
        } tag;
        friend std::ostream &operator<<(std::ostream &out, Tag self) noexcept {
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
        constexpr Option(Option<void> &&temp) :
            Option<void> { temp } {}
        ~Option() {
            if (is_some()) some.~T();
        }

        friend constexpr Option Some<T>(T Some) noexcept {
            Option temp(Option<> { Option<>::Tag::Some });
            temp.some = Some;
            return temp;
        }
        T &unwrap() {
            return some;
        }
        bool is_some() noexcept {
            return tag == Tag::Some;
        }
        bool is_none() noexcept {
            return tag == Tag::None;
        }

        template<typename F>
        auto map(F &&op) -> Option<decltype(op(std::declval<T>()))> {
            if (is_some()) return Some(op(some));
            else return None();
        }
        template<typename F>
        auto ok_or_else(F err) -> Result<T, decltype(err())> {
            return is_some() ? Ok(some) : Err(err());
        }

        friend std::ostream &operator<<(std::ostream &out, Option self) {
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

    constexpr Option<> None() noexcept {
        return Option<> { Option<>::Tag::None };
    }
}

#endif // !RESULT_H_
