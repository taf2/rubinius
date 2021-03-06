/* The simple ByteArray class, used to implement String. */

#include "bytearray.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"

namespace rubinius {

  void ByteArray::init(STATE) {
    GO(bytearray).set(state->new_class("ByteArray"));
    G(bytearray)->set_object_type(state, ByteArrayType);
  }

  ByteArray* ByteArray::create(STATE, size_t bytes) {
    return (ByteArray*)state->om->new_object_bytes(G(bytearray), bytes);
  }

  void ByteArray::Info::mark(Object* t, ObjectMark& mark) {
    // @todo implement
  }

  char* ByteArray::to_chars(STATE) {
    native_int sz = this->size(state)->to_native();
    char* str = (char*)(this->bytes);
    char* out = ALLOC_N(char, sz);

    std::memcpy(out, str, sz);

    return out;
  }

  ByteArray* ByteArray::allocate(STATE, Integer* bytes) {
    return ByteArray::create(state, bytes->to_native());
  }

  Integer* ByteArray::size(STATE) {
    return Integer::from(state, SIZE_OF_BODY(this));
  }

  Fixnum* ByteArray::get_byte(STATE, Integer* index) {
    native_int size = SIZE_OF_BODY(this);
    native_int idx = index->to_native();

    if(idx < 0 || idx >= size) {
      Exception::object_bounds_exceeded_error(state, "index out of bounds");
    }

    return Fixnum::from(this->bytes[idx]);
  }

  Fixnum* ByteArray::set_byte(STATE, Integer* index, Fixnum* value) {
    native_int size = SIZE_OF_BODY(this);
    native_int idx = index->to_native();

    if(idx < 0 || idx >= size) {
      Exception::object_bounds_exceeded_error(state, "index out of bounds");
    }

    this->bytes[idx] = value->to_native();
    return Fixnum::from(this->bytes[idx]);
  }

  Integer* ByteArray::move_bytes(STATE, Integer* start, Integer* count, Integer* dest) {
    native_int size = SIZE_OF_BODY(this);
    native_int src = start->to_native();
    native_int cnt = count->to_native();
    native_int dst = dest->to_native();

    if(src < 0) {
      Exception::object_bounds_exceeded_error(state, "start less than zero");
    } else if(dst < 0) {
      Exception::object_bounds_exceeded_error(state, "dest less than zero");
    } else if(cnt < 0) {
      Exception::object_bounds_exceeded_error(state, "count less than zero");
    } else if(dst + cnt > size) {
      Exception::object_bounds_exceeded_error(state, "move is beyond end of bytearray");
    } else if(src + cnt > size) {
      Exception::object_bounds_exceeded_error(state, "move is more than available bytes");
    }

    std::memmove(this->bytes + dst, this->bytes + src, cnt);

    return count;
  }

  ByteArray* ByteArray::fetch_bytes(STATE, Integer* start, Integer* count) {
    native_int size = SIZE_OF_BODY(this);
    native_int src = start->to_native();
    native_int cnt = count->to_native();

    if(src < 0) {
      Exception::object_bounds_exceeded_error(state, "start less than zero");
    } else if(cnt < 0) {
      Exception::object_bounds_exceeded_error(state, "count less than zero");
    } else if(src + cnt > size) {
      Exception::object_bounds_exceeded_error(state, "fetch is more than available bytes");
    }

    ByteArray* ba = ByteArray::create(state, cnt + 1);
    std::memcpy(ba->bytes, this->bytes + src, cnt);
    ba->bytes[cnt] = 0;

    return ba;
  }

  Fixnum* ByteArray::compare_bytes(STATE, ByteArray* other, Integer* a, Integer* b) {
    native_int size = SIZE_OF_BODY(this);
    native_int osize = SIZE_OF_BODY(other);
    native_int slim = a->to_native();
    native_int olim = b->to_native();

    if(slim < 0) {
      Exception::object_bounds_exceeded_error(state,
          "bytes of self to compare is less than zero");
    } else if(olim < 0) {
      Exception::object_bounds_exceeded_error(state,
          "bytes of other to compare is less than zero");
    }

    // clamp limits to actual sizes
    native_int m = size < slim ? size : slim;
    native_int n = osize < olim ? osize : olim;

    // only compare the shortest string
    native_int len = m < n ? m : n;

    native_int cmp = std::memcmp(this->bytes, other->bytes, len);

    // even if substrings are equal, check actual requested limits
    // of comparison e.g. "xyz", "xyzZ"
    if(cmp == 0) {
      if(m < n) {
        return Fixnum::from(-1);
      } else if(m > n) {
        return Fixnum::from(1);
      } else {
        return Fixnum::from(0);
      }
    } else {
      return cmp < 0 ? Fixnum::from(-1) : Fixnum::from(1);
    }
  }

  ByteArray* ByteArray::dup_into(STATE, ByteArray* other) {
    native_int size = SIZE_OF_BODY(this);
    native_int osize = SIZE_OF_BODY(other);

    std::memcpy(other->bytes, this->bytes, size < osize ? size : osize);

    return other;
  }

  Object* ByteArray::locate(STATE, String* pattern, Integer* start) {
    native_int size = SIZE_OF_BODY(this);
    const char *pat = pattern->byte_address();
    native_int len = pattern->size();

    if(len == 0) {
      return start;
    }

    for(native_int i = start->to_native(); i <= size - len; i++) {
      if(this->bytes[i] == pat[0]) {
        native_int j;
        // match the rest of the pattern string
        for(j = 1; j < len; j++) {
          if(this->bytes[i+j] != pat[j]) break;
        }

        // if the full pattern matched, return the index
        // of the end of the pattern in 'this'.
        if(j == len) return Integer::from(state, i + len);
      }
    }

    return Qnil;
  }
}
