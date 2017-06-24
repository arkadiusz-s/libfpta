/*
 * Copyright 2016-2017 libfpta authors: please see AUTHORS file.
 *
 * This file is part of libfpta, aka "Fast Positive Tables".
 *
 * libfpta is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libfpta is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libfpta.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "details.h"

template <typename TYPE>
static TYPE confine_value(const fpta_value &value, const TYPE begin,
                          const TYPE end) {
  return confine_value(value, fptu::cast_wide(begin), fptu::cast_wide(end));
}

template <>
uint64_t confine_value<uint64_t>(const fpta_value &value, const uint64_t begin,
                                 const uint64_t end) {
  assert(begin < end);
  switch (value.type) {
  default:
    assert(false);
    return begin;
  case fpta_signed_int:
    if (value.sint < 0)
      return begin;
  // no break here
  case fpta_unsigned_int:
    if (value.uint < begin)
      return begin;
    if (value.uint > end)
      return end;
    return value.uint;
  case fpta_float_point:
    if (value.fp < 0)
      return begin;
    if (value.fp < begin)
      return begin;
    if (value.fp > end)
      return end;
    return (uint64_t)value.fp;
  }
}

template <>
int64_t confine_value<int64_t>(const fpta_value &value, const int64_t begin,
                               const int64_t end) {
  assert(begin < end);
  switch (value.type) {
  default:
    assert(false);
    return begin;
  case fpta_unsigned_int:
    if (value.uint > (uint64_t)INT64_MAX)
      return end;
  // no break here
  case fpta_signed_int:
    if (value.sint < begin)
      return begin;
    if (value.sint > end)
      return end;
    return value.sint;
  case fpta_float_point:
    if (value.fp < begin)
      return begin;
    if (value.fp > end)
      return end;
    return (int64_t)value.fp;
  }
}

template <>
double confine_value<double>(const fpta_value &value, const double begin,
                             const double end) {
  assert(begin < end);
  switch (value.type) {
  default:
    assert(false);
    return begin;
  case fpta_unsigned_int:
    if (value.uint < begin)
      return begin;
    if (value.uint > end)
      return end;
    return value.uint;
  case fpta_signed_int:
    if (value.sint < begin)
      return begin;
    if (value.sint > end)
      return end;
    return value.sint;
  case fpta_float_point:
    if (value.fp < begin)
      return begin;
    if (value.fp > end)
      return end;
    return value.fp;
  }
}

//----------------------------------------------------------------------------

template <fptu_type type> struct numeric_traits;

template <> struct numeric_traits<fptu_uint16> {
  typedef uint16_t native;
  typedef std::numeric_limits<native> native_limits;
  static native denil(const fpta_index_type index) {
    assert(fpta_index_is_nullable(index));
    return fpta_index_is_obverse(index) ? FPTA_DENIL_UINT16_OBVERSE
                                        : FPTA_DENIL_UINT16_REVERSE;
  }
  static fpta_value_type value_type() { return fpta_unsigned_int; }
  static fpta_value make_value(const native value) {
    return fpta_value_uint(value);
  }
};

template <> struct numeric_traits<fptu_uint32> {
  typedef uint32_t native;
  typedef std::numeric_limits<native> native_limits;
  static native denil(const fpta_index_type index) {
    assert(fpta_index_is_nullable(index));
    return fpta_index_is_obverse(index) ? FPTA_DENIL_UINT32_OBVERSE
                                        : FPTA_DENIL_UINT32_REVERSE;
  }
  static fpta_value_type value_type() { return fpta_unsigned_int; }
  static fpta_value make_value(const native value) {
    return fpta_value_uint(value);
  }
};

template <> struct numeric_traits<fptu_uint64> {
  typedef uint64_t native;
  typedef std::numeric_limits<native> native_limits;
  static native denil(const fpta_index_type index) {
    assert(fpta_index_is_nullable(index));
    return fpta_index_is_obverse(index) ? FPTA_DENIL_UINT64_OBVERSE
                                        : FPTA_DENIL_UINT64_REVERSE;
  }
  static fpta_value_type value_type() { return fpta_unsigned_int; }
  static fpta_value make_value(const native value) {
    return fpta_value_uint(value);
  }
};

template <> struct numeric_traits<fptu_int32> {
  typedef int32_t native;
  typedef std::numeric_limits<native> native_limits;
  static native denil(const fpta_index_type index) {
    assert(fpta_index_is_nullable(index));
    (void)index;
    return FPTA_DENIL_SINT32;
  }
  static fpta_value_type value_type() { return fpta_signed_int; }
  static fpta_value make_value(const native value) {
    return fpta_value_sint(value);
  }
};

template <> struct numeric_traits<fptu_int64> {
  typedef int64_t native;
  typedef std::numeric_limits<native> native_limits;
  static native denil(const fpta_index_type index) {
    assert(fpta_index_is_nullable(index));
    (void)index;
    return FPTA_DENIL_SINT64;
  }
  static fpta_value_type value_type() { return fpta_signed_int; }
  static fpta_value make_value(const native value) {
    return fpta_value_sint(value);
  }
};

template <> struct numeric_traits<fptu_fp32> {
  typedef float native;
  typedef std::numeric_limits<native> native_limits;
  static native denil(const fpta_index_type index) {
    assert(fpta_index_is_nullable(index));
    (void)index;
    return FPTA_DENIL_FP32;
  }
  static fpta_value_type value_type() { return fpta_float_point; }
  static fpta_value make_value(const native value) {
    return fpta_value_float(value);
  }
};

template <> struct numeric_traits<fptu_fp64> {
  typedef double native;
  typedef std::numeric_limits<native> native_limits;
  static native denil(const fpta_index_type index) {
    assert(fpta_index_is_nullable(index));
    (void)index;
    return FPTA_DENIL_FP64;
  }
  static fpta_value_type value_type() { return fpta_float_point; }
  static fpta_value make_value(const native value) {
    return fpta_value_float(value);
  }
};

template <fptu_type type> struct saturated {
  typedef numeric_traits<type> traits;
  typedef typename traits::native native;
  typedef typename traits::native_limits native_limits;

  static native bottom(const fpta_index_type index) {
    if (!fpta_index_is_nullable(index) || !native_limits::is_integer)
      return native_limits::lowest();
    const native lower = native_limits::min();
    return (lower != traits::denil(index)) ? lower : lower + 1;
  }

  static native top(const fpta_index_type index) {
    const native upper = native_limits::max();
    if (!fpta_index_is_nullable(index) || !native_limits::is_integer)
      return upper;
    return (upper != traits::denil(index)) ? upper : upper - 1;
  }

  static int confine(const fpta_index_type index, fpta_value &value) {
    assert(value.is_number());
    const native lower = bottom(index);
    const native upper = top(index);
    value = traits::make_value(confine_value<native>(value, lower, upper));
    return FPTA_SUCCESS;
  }

  static bool min(const fpta_index_type index, fptu_field *field,
                  const fpta_value &value, native &result) {
    const native ones = confine_value(value, bottom(index), top(index));
    if (unlikely(!field)) {
      result = ones;
      return true;
    }

    const native present = fptu::get_number<type, native>(field);
    if (ones < present) {
      result = ones;
      return true;
    }
    return false;
  }

  static bool max(const fpta_index_type index, fptu_field *field,
                  const fpta_value &value, native &result) {
    const native ones = confine_value(value, bottom(index), top(index));
    if (unlikely(!field)) {
      result = ones;
      return true;
    }

    const native present = fptu::get_number<type, native>(field);
    if (ones > present) {
      result = ones;
      return true;
    }
    return false;
  }

  static bool add(const fpta_index_type index, fptu_field *field,
                  const fpta_value &value, native &result) {
    if (value.is_negative())
      return sub(index, field, -value, result);

    const native lower = bottom(index);
    const native upper = top(index);
    const native addend = confine_value(value, (native)0, upper);
    if (unlikely(!field)) {
      if (addend == 0 && lower > 0)
        return false /* не допускаем появления не-нулевого значения при добавлении нуля к пустоте */;
      result = confine_value(value, lower, upper);
      return true;
    }

    const native present = fptu::get_number<type, native>(field);
    if (addend == 0 || upper == present)
      return false;

    result = (upper - present > addend) ? present + addend : upper;
    return true;
  }

  static bool sub(const fpta_index_type index, fptu_field *field,
                  const fpta_value &value, native &result) {
    if (value.is_negative())
      return add(index, field, -value, result);

    const native lower = bottom(index);
    const native upper = top(index);
    const native subtrahend = confine_value(value, (native)0, upper);
    if (unlikely(!field)) {
      if (subtrahend == 0 && lower > 0)
        return false /* не допускаем появления не-нулевого значения при вычитании нуля из пустоты */;
      result = (0 - lower > subtrahend) ? 0 - subtrahend : lower;
      return true;
    }

    const native present = fptu::get_number<type, native>(field);
    if (subtrahend == 0 || lower == present)
      return false;

    result = (present - lower > subtrahend) ? present - subtrahend : lower;
    return true;
  }

  static int inplace(const fpta_inplace op, const fpta_index_type index,
                     const fpta_value &value, fptu_rw *row,
                     const unsigned colnum) {
    native result;
    fptu_field *field = fptu_lookup(row, colnum, type);

    switch (op) {
    default:
      return FPTA_EINVAL;
    case fpta_saturated_add:
      if (!add(index, field, value, result))
        return FPTA_SUCCESS;
      break;
    case fpta_saturated_sub:
      if (!sub(index, field, value, result))
        return FPTA_SUCCESS;
      break;
    case fpta_min:
      if (!min(index, field, value, result))
        return FPTA_SUCCESS;
      break;
    case fpta_max:
      if (!max(index, field, value, result))
        return FPTA_SUCCESS;
      break;
    case fpta_saturated_mul:
    case fpta_saturated_div:
    case fpta_bes:
      return FPTA_ENOIMP /* TODO */;
    }
    return fptu::upsert_number<type, native>(row, colnum, result);
  }
};

FPTA_API int fpta_confine_number(fpta_value *value, fpta_name *column_id) {
  if (unlikely(!value || !fpta_id_validate(column_id, fpta_column)))
    return FPTA_EINVAL;

  const fptu_type coltype = fpta_shove2type(column_id->shove);
  const fpta_index_type index = fpta_name_colindex(column_id);
  if (unlikely((fptu_any_number & (INT32_C(1) << coltype)) == 0))
    return FPTA_ETYPE;

  switch (value->type) {
  case fpta_null:
    if (fpta_index_is_nullable(index))
      return FPTA_SUCCESS;
  // no break here
  default:
    return FPTA_EVALUE;
  case fpta_float_point:
    if (unlikely(std::isnan(value->fp))) {
      if ((fptu_any_fp & (INT32_C(1) << coltype)) == 0 ||
          FPTA_PROHIBIT_UPSERT_NAN)
        return FPTA_EVALUE;
      value->fp = std::nan("");
      return FPTA_SUCCESS;
    }
    if (coltype == fptu_fp64)
      return FPTA_SUCCESS;
  case fpta_signed_int:
  case fpta_unsigned_int:
    break;
  }

  switch (coltype) {
  default:
    assert(false);
    return FPTA_EOOPS;
  case fptu_uint16:
    return saturated<fptu_uint16>::confine(index, *value);
  case fptu_uint32:
    return saturated<fptu_uint32>::confine(index, *value);
  case fptu_uint64:
    return saturated<fptu_uint64>::confine(index, *value);
  case fptu_int32:
    return saturated<fptu_int32>::confine(index, *value);
  case fptu_int64:
    return saturated<fptu_int64>::confine(index, *value);
  case fptu_fp32:
    return saturated<fptu_fp32>::confine(index, *value);
  case fptu_fp64:
    return saturated<fptu_fp64>::confine(index, *value);
  }
}

FPTA_API int fpta_inplace_column(fptu_rw *row, const fpta_name *column_id,
                                 const fpta_inplace op, const fpta_value value,
                                 ...) {
  if (unlikely(!row || !fpta_id_validate(column_id, fpta_column)))
    return FPTA_EINVAL;
  if (unlikely(op < fpta_saturated_add || op > fpta_bes))
    return FPTA_EINVAL;

  const fptu_type coltype = fpta_shove2type(column_id->shove);
  if (unlikely((fptu_any_number & (INT32_C(1) << coltype)) == 0))
    return FPTA_EINVAL;

  assert(column_id->column.num <= fptu_max_cols);
  const unsigned colnum = (unsigned)column_id->column.num;

  switch (value.type) {
  default:
    return FPTA_ETYPE;
  case fpta_float_point:
    if (likely(!std::isnan(value.fp)))
      break;
    if (FPTA_PROHIBIT_UPSERT_NAN)
      return FPTA_EVALUE;
  // no break here
  case fpta_null:
    return FPTA_SUCCESS /* silently ignore null-arg as no-op */;
  case fpta_signed_int:
  case fpta_unsigned_int:
    break;
  }

  const fpta_index_type index = fpta_name_colindex(column_id);
  switch (coltype) {
  default:
    assert(false);
    return FPTA_EOOPS;
  case fptu_uint16:
    return saturated<fptu_uint16>::inplace(op, index, value, row, colnum);
  case fptu_uint32:
    return saturated<fptu_uint32>::inplace(op, index, value, row, colnum);
  case fptu_uint64:
    return saturated<fptu_uint64>::inplace(op, index, value, row, colnum);
  case fptu_int32:
    return saturated<fptu_int32>::inplace(op, index, value, row, colnum);
  case fptu_int64:
    return saturated<fptu_int64>::inplace(op, index, value, row, colnum);
  case fptu_fp32:
    return saturated<fptu_fp32>::inplace(op, index, value, row, colnum);
  case fptu_fp64:
    return saturated<fptu_fp64>::inplace(op, index, value, row, colnum);
  }
}
