#ifndef RESULT_H_
#define RESULT_H_

#include <concepts>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>

#define PANIC(ERROR)                 \
    std::cerr << ERROR << std::endl; \
    exit(1);

namespace ftl {
    template<typename E>
    concept Error = requires(E err) {
        err.descrition();
    };

    template<typename F, typename Ret, typename ...Args>
    concept Fn = requires(F f, Args ...args) {
        { f(args...) } -> std::same_as<Ret>;
    };

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

        constexpr size_type size() { return end_iter - begin_iter; }

        constexpr value_type &operator[](size_type idx) {
            return begin_iter[idx];
        }
        bool operator==(str &other) {
            for (auto &[i, j] : { *this, other }) {
                if (*i != *j) return false;
            }
            return true;
        }
    private:
        iterator begin_iter;
        iterator end_iter;
    };

    template<typename T, typename E>
    struct Result;
    template<typename T>
    struct Option;

    template<typename T>
    constexpr Option<T> Some(T Some);

    template<>
    struct Result<void, void> {
        enum class Tag {
            Ok,
            Err,
        } tag;
        friend std::ostream &operator<<(std::ostream &out, Tag self) {
            switch (self) {
            case Tag::Ok:
                return out << "Ok";
            case Tag::Err:
                return out << "Err";
            }
        }
    };

    template<typename T, typename E>
    struct
    [[nodiscard("`Result` may be an `Err` variant, which should be handled")]]
    Result : Result<void, void> {
        constexpr Result(Result<T, void> &&temp) :
            Result<void, void>{ Tag::Ok },
            Ok(temp.Ok) { }
        constexpr Result(Result<void, E> &&temp) :
            Result<void, void>{ Tag::Err },
            Err(temp.unwrap_err()) { }
        ~Result() {
            switch (tag) {
            case Tag::Ok:
                Ok.~T();
                break;
            case Tag::Err:
                Err.~E();
                break;
            }
        }
        T &unwrap() {
            switch (tag) {
            case Tag::Ok:
                return Ok;
            case Tag::Err:
                PANIC("Tried to unwrap an Error, baka");
            }
        }
        E &unwrap_err() {
            switch (tag) {
            case Tag::Ok:
                PANIC("Not an error, you fucking idiot");
            case Tag::Err:
                return Err;
            }
        }
        friend std::ostream &operator<<(std::ostream &out, Result &self) {
            out << self.tag << '(';
            switch (self.tag) {
            case decltype(self.tag)::Ok:
                return out << self.Ok << ')';
            case decltype(self.tag)::Err:
                return out << self.Err << ')';
            }
        }
    private:
        union {
            T Ok;
            E Err;
        };
    };

    template<typename E>
    struct
    [[nodiscard("`Result` may be an `Err` variant, which should be handled")]]
    Result<void, E> : Result<void, void> {
        Result(Result<void, void> &&temp) :
            Result<void, void> { temp } {}
        Result(E Err) :
            Result<void, void> { Tag::Err },
            Err(Err) {}
        void unwrap() {
            switch (tag) {
            case Tag::Ok:
                break;
            case Tag::Err:
                PANIC("Tried to unwrap an error, baka");
            }
        }
        E &unwrap_err() {
            switch (tag) {
            case Tag::Ok:
                PANIC("Not an error, you fucking idiot");
            case Tag::Err:
                return Err;
            }
        }
        friend std::ostream &operator<<(std::ostream &out, Result &self) {
            return out << self.tag << '(' << self.Err << ')';
        }
    private:
        E Err;
    };
    template<typename T>
    struct Result<T, void> : Result<void, void> {
        constexpr static Tag tag = Tag::Ok;
        T Ok;
    };

    template<typename T>
    constexpr Result<T, void> Ok(T Ok) {
        return Result<T, void> { .Ok = Ok };
    }
    constexpr Result<void, void> Ok() {
        return Result<void, void> { Result<void, void>::Tag::Ok };
    }

    template<typename E>
    constexpr Result<void, E> Err(E Err) {
        return Result<void, E>(Err);
    }

    template<>
    struct Option<void> {
        enum class Tag {
            Some,
            None,
        } tag;
    };
    template<typename T>
    struct Option : Option<void> {
        Option(Option<void> &&temp) :
            Option<void> { temp } {}
        friend Option Some<T>(T);

        T &unwrap() {
            return Some;
        }
        bool is_some() {
            return tag == Tag::Some;
        }
        bool is_none() {
            return tag == Tag::None;
        }
        template<typename E>
        Result<T, E> ok_or_else(Fn<E> auto err) {
            return is_some() ? Ok(Some) : Err(err());
        }
    private:
        T Some;
    };

    template<typename T>
    constexpr Option<T> Some(T Some) {
        Option<T> temp(Option<void> { Option<void>::Tag::Some });
        temp.Some = Some;
        return temp;
    }
    constexpr Option<void> None() {
        return Option<void> { Option<void>::Tag::None };
    }
}

#endif // !RESULT_H_
