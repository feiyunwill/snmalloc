#pragma once

#include "pal_bsd.h"

namespace snmalloc
{
  /**
   * FreeBSD-specific platform abstraction layer.
   *
   * This adds aligned allocation using `MAP_ALIGNED` to the generic BSD
   * implementation.  This flag is supported by NetBSD and FreeBSD.
   */
  template<class OS>
  class PALBSD_Aligned : public PALBSD<OS>
  {
  public:
    /**
     * Bitmap of PalFeatures flags indicating the optional features that this
     * PAL supports.
     *
     * This class adds support for aligned allocation.
     */
    static constexpr uint64_t pal_features =
      AlignedAllocation | PALBSD<OS>::pal_features;

    /**
     * Reserve memory at a specific alignment.
     */
    template<bool committed>
    void* reserve(const size_t* size, size_t align) noexcept
    {
      // Alignment must be a power of 2.
      assert(align == bits::next_pow2(align));

      align = bits::max<size_t>(4096, align);

      size_t log2align = bits::next_pow2_bits(align);

      void* p = mmap(
        nullptr,
        *size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_ALIGNED(log2align),
        -1,
        0);

      if (p == MAP_FAILED)
        PALBSD<OS>::error("Out of memory");

      return p;
    }
  };
} // namespace snmalloc
