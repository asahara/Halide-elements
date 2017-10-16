#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <climits>

#include "HalideRuntime.h"
#include "HalideBuffer.h"

#include "div_scalar_u8.h"
#include "div_scalar_u16.h"
#include "div_scalar_u32.h"

#include "test_common.h"

template<typename T>
int test(int (*func)(struct halide_buffer_t *_src_buffer, int32_t _width, int32_t _height, float _value, struct halide_buffer_t *_dst_buffer))
{
    try {
        int ret = 0;

        //
        // Run
        //
        const int width = 1024;
        const int height = 768;
        const float value = 2.0;
        const std::vector<int32_t> extents{width, height};
        auto input = mk_rand_buffer<T>(extents);
        auto output = mk_null_buffer<T>(extents);

        func(input, width, height, value, output);

        for (int y=0; y<height; ++y) {
            for (int x=0; x<width; ++x) {
                T expect = input(x, y);
                T actual = output(x, y);
                float f = expect / value;
                f = std::min(static_cast<float>(std::numeric_limits<T>::max()), f);
                f = std::max(0.0f, f);
                expect = round_to_nearest_even<T>(f);
                if (expect != actual) {
                    throw std::runtime_error(format("Error: expect(%d, %d) = %d, actual(%d, %d) = %d", x, y, expect, x, y, actual).c_str());
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    printf("Success!\n");
    return 0;
}

int main()
{
    test<uint8_t>(div_scalar_u8);
    test<uint16_t>(div_scalar_u16);
    test<uint32_t>(div_scalar_u32);
}
