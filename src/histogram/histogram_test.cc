#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <climits>

#include "HalideRuntime.h"
#include "HalideBuffer.h"

#include "histogram.h"

#include "test_common.h"

int main()
{
    try {
        int ret = 0;

        //
        // Run
        //
        const int width = 1024;
        const int height = 768;
        const int hist_width = std::numeric_limits<uint8_t>::max() + 1;
        const std::vector<int32_t> extents{width, height}, extents_hist{hist_width};
        auto input = mk_rand_buffer<uint8_t>(extents);
        auto output = mk_null_buffer<uint32_t>(extents_hist);
        uint32_t expect[hist_width];
        uint32_t hist_size = std::numeric_limits<uint8_t>::max() + 1;
        uint32_t hist[hist_size];
        int bin_size = (hist_size + hist_width - 1) / hist_width;

        memset(hist, 0, sizeof(hist));
        for (int y=0; y<height; ++y) {
            for (int x=0; x<width; ++x) {
                hist[input(x, y)]++;
            }
        }

        int idx = 0;
        for (int i = 0; i < hist_width; i++) {
            uint32_t sum = 0;
            for (int k = 0; k < bin_size && idx < hist_size; ++k) {
                sum += hist[idx++];
            }
            expect[i] = sum;
        }

        histogram(input, output);

        for (int x=0; x<hist_width; ++x) {
            uint32_t actual = output(x);
            if (expect[x] != actual) {
                throw std::runtime_error(format("Error: expect(%d) = %d, actual(%d) = %d", x, expect[x], x, actual).c_str());
            }
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    printf("Success!\n");
    return 0;
}
