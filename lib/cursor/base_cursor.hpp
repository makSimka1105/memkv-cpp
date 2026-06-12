#pragma once

#include <cstddef>
#include <span>

namespace valkey::cursor {

template <class Elem>
class Savepoint;

template <class Elem>
class CursorBase {
public:
    explicit CursorBase(std::span<const Elem> items, size_t pos = 0)
        : items_(items), pos_(pos) {}

    const Elem* At(size_t offset = 0) const {
        size_t i = pos_ + offset;
        return i < items_.size() ? &items_[i] : nullptr;
    }

    void Advance(size_t n = 1) { pos_ += n; }
    bool AtEnd() const { return pos_ >= items_.size(); }
    size_t Pos() const { return pos_; }

    Savepoint<Elem> Save() { return Savepoint<Elem>(*this); }

protected:
    std::span<const Elem> items_;
    size_t pos_ = 0;

    friend class Savepoint<Elem>;
};

template <class Elem>
class Savepoint {
public:
    Savepoint(const Savepoint&) = delete;
    Savepoint& operator=(const Savepoint&) = delete;

    ~Savepoint() {
        if (!committed_) {
            cursor_.pos_ = saved_;
        }
    }

    void Commit() { committed_ = true; }

private:
    explicit Savepoint(CursorBase<Elem>& cursor)
        : cursor_(cursor), saved_(cursor.pos_) {}

    friend class CursorBase<Elem>;

    CursorBase<Elem>& cursor_;
    size_t saved_;
    bool committed_ = false;
};

}  // namespace valkey::cursor
